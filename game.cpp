#include "game.h"

Game::Game(QWidget *parent): QFrame(parent) {
    _countdownTime = 4;
    _playerSpeed = 3;
    _boardSize = QRect(10, 110, 780, 480);
    _playerSize = QSize(15, 15);

    _menuSelectedOption = 0;
    _gameBoard = nullptr;
    _countdownCurrentTime = _countdownTime;
    _countdownTimer = new QTimer(this);
    _countdownTimer->setInterval(1000);
    _countdownTimer->setSingleShot(true);
    this->connect(_countdownTimer, SIGNAL(timeout()), this, SLOT(countdown()));
    _speedForceTimer = new QTimer(this);
    _speedForceTimer->setSingleShot(true);
    this->connect(_speedForceTimer, SIGNAL(timeout()), this, SLOT(startSpeedForce()));

    _menuBackgroundImage.load(":/Resources/Video/menuBackground.jpg");
    _blueBikeImage.load(":/Resources/Video/BlueBike.jpg");
    _redBikeImage.load(":/Resources/Video/RedBike.jpg");
    _checkedRadioButtonImage.load(":/Resources/Video/RadioChecked.png");
    _unchecedRadioButtonImage.load(":/Resources/Video/RadioUnchecked.png");
    _winnerFlagImage.load(":/Resources/Video/WinnerFlag.png");
    _speedBoostImage.load(":/Resources/Video/nitro.png");

    QMediaPlaylist *playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl("qrc:/Resources/Audio/ThemeSong.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    _musicThemePlayer = new QMediaPlayer(this);
    _musicThemePlayer->setPlaylist(playlist);
    _musicThemePlayer->play();

    _menuSoundPlayer = new QMediaPlayer(this);
    _menuSoundPlayer->setMedia(QUrl("qrc:/Resources/Audio/Menu.mp3"));

    _fps = 0;
    _lastGameTick = 0;
    _fpsBufferIndex = 0;
    _fpsSum = 0;
    for(int x = 0; x < 10; x++)
        _fpsBuffer[x] = 0;

    _isGameON = false;
    _isSoundON = true;
    _isGameOver = false;
    _isCountingDown = false;
    _displayFPS = true;   
}

Game::~Game() {
    if(_gameBoard != nullptr)
        delete _gameBoard;
    if(_countdownTimer != nullptr)
        delete _countdownTimer;
    if(_speedForceTimer != nullptr)
        delete _speedForceTimer;
    if(_musicThemePlayer != nullptr)
        delete _musicThemePlayer;
    if(_menuSoundPlayer != nullptr)
        delete _menuSoundPlayer;
}

void Game::paintEvent(QPaintEvent* event) {
    QFrame::paintEvent(event);

    QPainter painter(this);

    if(!_isGameON) {
        this->paintMenu(&painter);
    } else {
        this->paintScore(&painter);
        this->paintGameOn(&painter);
        if(_isCountingDown)
            this->paintCountdown(&painter);
        if(_isGameOver)
            this->paintGameOver(&painter);
    }
    if(_displayFPS)
        this->paintFps(&painter);
    this->calculateFPS();
}

void Game::paintMenu(QPainter *painter) {
    painter->drawImage(QRect(0, 0, this->width(), this->height()), _menuBackgroundImage);
    int baseHeight = this->height() / 12;
    painter->setFont(QFont("Helvetica", this->height() / 25, 0));

    QBrush brush;
    menuDefaultColor(painter, &brush);

    if(_menuSelectedOption == 0) { menuSelectedColor(painter, &brush); }
    painter->fillRect(0, baseHeight * 4, this->width() - 1, static_cast<int>(baseHeight / 1.2), brush);
    painter->drawText(0, baseHeight * 4, this->width() - 1, static_cast<int>(baseHeight / 1.2), Qt::AlignCenter, "Player VS Player");
    menuDefaultColor(painter, &brush);

    if(_menuSelectedOption == 1) { menuSelectedColor(painter, &brush); }
    painter->fillRect(0, baseHeight * 5, this->width() - 1, static_cast<int>(baseHeight / 1.2), brush);
    painter->drawText(0, baseHeight * 5, this->width() - 1, static_cast<int>(baseHeight / 1.2), Qt::AlignCenter, "Player VS AI");
    menuDefaultColor(painter, &brush);

    if(_menuSelectedOption == 2) { menuSelectedColor(painter, &brush); }
    painter->fillRect(0, baseHeight * 6, this->width() - 1, static_cast<int>(baseHeight / 1.2), brush);
    QString str;
    if(_isSoundON) { str = "Sound ON"; }
    else { str = "Sound OFF"; }
    painter->drawText(0, baseHeight * 6, this->width() - 1, static_cast<int>(baseHeight / 1.2), Qt::AlignCenter, str);
    menuDefaultColor(painter, &brush);

    if(_menuSelectedOption == 3) { menuSelectedColor(painter, &brush); }
    painter->fillRect(0, baseHeight * 7, this->width() - 1, static_cast<int>(baseHeight / 1.2), brush);
    painter->drawText(0, baseHeight * 7, this->width() - 1, static_cast<int>(baseHeight / 1.2), Qt::AlignCenter, "Exit");
    menuDefaultColor(painter, &brush);
}

void Game::paintGameOn(QPainter *painter) {
    _gameBoard->drawGameBoard(painter);

    QPen player(QColor(0, 0, 255));
    player.setWidth(qMax(_playerSize.width(), _playerSize.height()));
    painter->setPen(player);
    _gameBoard->drawPlayer1(painter);

    player.setColor(QColor(255, 0, 0));
    painter->setPen(player);
    _gameBoard->drawPlayer2(painter);

    _gameBoard->drawSpeedForce(painter, _speedBoostImage);
}

void Game::paintGameOver(QPainter *painter) {
    painter->setPen(QPen(QColor(0, 0, 0)));
    painter->setFont(QFont("Helvetica", this->height() / 12, QFont::Bold));
    painter->setOpacity(0.5);
    painter->fillRect(_boardSize.x(), _boardSize.y(), _boardSize.width(), _boardSize.height(), QColor(255, 255, 255));
    painter->setOpacity(1);
    if(_gameBoard->player1Victories() >= 3 ) {
        painter->drawImage(QRect(_boardSize.width() / 3 + _boardSize.x(), _boardSize.height() / 12 + _boardSize.y(),
                                 _boardSize.width() / 3, _boardSize.height() / 3), _winnerFlagImage);
        painter->drawText(_boardSize.x(), _boardSize.y(), _boardSize.width(), _boardSize.height(),
                          Qt::AlignCenter, "Player 1 WON");
        painter->setFont(QFont("Helvetica", this->height() / 36));
        painter->drawText(_boardSize.x(), _boardSize.height() / 5 * 3 + _boardSize.y(), _boardSize.width(), _boardSize.height() / 4,
                          Qt::AlignCenter, "Press ENTER to Restart\nPress ESC to Quit");
    } else if(_gameBoard->player2Victories() >= 3) {
        painter->drawImage(QRect(_boardSize.width() / 3 + _boardSize.x(), _boardSize.height() / 12 + _boardSize.y(),
                                 _boardSize.width() / 3, _boardSize.height() / 3), _winnerFlagImage);
        painter->drawText(_boardSize.x(), _boardSize.y(), _boardSize.width(), _boardSize.height(),
                          Qt::AlignCenter, "Player 2 WON");
        painter->setFont(QFont("Helvetica", this->height() / 36));
        painter->drawText(_boardSize.x(), _boardSize.height() / 5 * 3 + _boardSize.y(), _boardSize.width(), _boardSize.height() / 4,
                          Qt::AlignCenter, "Press ENTER to Restart\nPress ESC to Quit");
    } else {
        if(_gameBoard->player1Crashed()) {
            painter->drawText(_boardSize.x(), _boardSize.y(), _boardSize.width(), _boardSize.height() / 3 * 2,
                              Qt::AlignCenter, "Match Winner\nPlayer 2");
        } else if(_gameBoard->player2Crashed()) {
            painter->drawText(_boardSize.x(), _boardSize.y(), _boardSize.width(), _boardSize.height() / 3 * 2,
                              Qt::AlignCenter, "Match Winner\nPlayer 1");
        } else {
            painter->drawText(_boardSize.x(), _boardSize.y(), _boardSize.width(), _boardSize.height() / 3 * 2,
                              Qt::AlignCenter, "DRAW\nHead to Head Colision");
        }
        painter->setFont(QFont("Helvetica", this->height() / 36));
        painter->drawText(_boardSize.x(), _boardSize.height() / 5 * 3 + _boardSize.y(), _boardSize.width(), _boardSize.height() / 4,
                          Qt::AlignCenter, "Press ENTER to Continue\nPress ESC to Quit");
    }
}

void Game::paintCountdown(QPainter *painter) {
    painter->setPen(QPen(QColor(0, 0, 0, 255)));
    painter->setFont(QFont("Helvetica", this->height() / 12, QFont::Bold));
    painter->drawText(_boardSize.x(), _boardSize.y(), _boardSize.width(), _boardSize.height(),
                      Qt::AlignCenter, QString::number(_countdownCurrentTime));
}

void Game::paintFps(QPainter *painter) {
    painter->setPen(QPen(QColor(255, 255, 255)));
    painter->setFont(QFont("Helvetica", this->height() / 36));
    QString text;
    if(_isGameON)
        text.append(QString::number(_fps) + " fps ");
    if(_playerSpeed / 3 > 1)
        text.append(QString::number(_playerSpeed / 3) + "x");
    painter->drawText(0, 0, this->width() - 1, this->height() - 1, Qt::AlignRight, text);
}

void Game::paintScore(QPainter *painter) {
    painter->drawImage(QRect(0, 0, this->size().width() / 2, 110), _blueBikeImage);
    painter->drawImage(QRect(this->size().width() / 2, 0, this->size().width() / 2, 110), _redBikeImage);
    painter->setPen(QPen(QColor(255, 255, 255)));
    painter->setFont(QFont("Helvetica", 25));
    painter->drawText(0, 0, this->width() - 1, 50, Qt::AlignCenter, "VS");

    switch (_gameBoard->player1Victories()) {
    case 0: {
        painter->drawImage(QRect(20, 70, 30, 30), _unchecedRadioButtonImage);
        painter->drawImage(QRect(50, 70, 30, 30), _unchecedRadioButtonImage);
        painter->drawImage(QRect(80, 70, 30, 30), _unchecedRadioButtonImage);
        break;
    }
    case 1: {
        painter->drawImage(QRect(20, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(50, 70, 30, 30), _unchecedRadioButtonImage);
        painter->drawImage(QRect(80, 70, 30, 30), _unchecedRadioButtonImage);
        break;
    }
    case 2: {
        painter->drawImage(QRect(20, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(50, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(80, 70, 30, 30), _unchecedRadioButtonImage);
        break;
    }
    case 3: {
        painter->drawImage(QRect(20, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(50, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(80, 69, 30, 30), _checkedRadioButtonImage);
        break;
    }
    default:
        break;
    }

    switch (_gameBoard->player2Victories()) {
    case 0: {
        painter->drawImage(QRect(this->width() - 50, 70, 30, 30), _unchecedRadioButtonImage);
        painter->drawImage(QRect(this->width() - 80, 70, 30, 30), _unchecedRadioButtonImage);
        painter->drawImage(QRect(this->width() - 110, 70, 30, 30), _unchecedRadioButtonImage);
        break;
    }
    case 1: {
        painter->drawImage(QRect(this->width() - 50, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(this->width() - 80, 70, 30, 30), _unchecedRadioButtonImage);
        painter->drawImage(QRect(this->width() - 110, 70, 30, 30), _unchecedRadioButtonImage);
        break;
    }
    case 2: {
        painter->drawImage(QRect(this->width() - 50, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(this->width() - 80, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(this->width() - 110, 70, 30, 30), _unchecedRadioButtonImage);
        break;
    }
    case 3: {
        painter->drawImage(QRect(this->width() - 50, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(this->width() - 80, 69, 30, 30), _checkedRadioButtonImage);
        painter->drawImage(QRect(this->width() - 110, 69, 30, 30), _checkedRadioButtonImage);
        break;
    }
    default:
        break;
    }

}

void Game::calculateFPS() {
    qint64 key = QDateTime::currentMSecsSinceEpoch();

    _fpsSum -= static_cast<quint64>(_fpsBuffer[_fpsBufferIndex]);
    _fpsSum += static_cast<quint64>(key - _lastGameTick);
    _fpsBuffer[_fpsBufferIndex] = key - _lastGameTick;
    if(++_fpsBufferIndex == 10)
        _fpsBufferIndex = 0;
    _fps = static_cast<quint16>(1000 / (_fpsSum / 10.0));
    _lastGameTick = key;
}

void Game::menuSelectedColor(QPainter *painter, QBrush *brush) {
    *brush = QBrush(QColor(0, 0, 0, 250));
    (*painter).setPen(QPen(QColor(255, 255, 255)));
}

void Game::menuDefaultColor(QPainter *painter, QBrush *brush) {
    *brush = QBrush(QColor(0, 0, 0, 150));
    (*painter).setPen(QPen(QColor(255, 255, 255, 150)));
}

void Game::countdown() {
    if(_countdownCurrentTime > 1) {
        _isCountingDown = true;
        _countdownCurrentTime--;
        _countdownTimer->start();
        this->repaint();
    } else {
        _isCountingDown = false;
        _countdownCurrentTime = _countdownTime;
        _gameBoard->configure();
        _gameBoard->tick();
        if(!_menuSelectedOption)
            _speedForceTimer->start(1000);
    }
}

void Game::menuOptionSelected(qint8 selectedOption) {
    switch (selectedOption) {
    case 0: {
        _isGameON = true;
        if(_gameBoard != nullptr)
            delete _gameBoard;
        _gameBoard = new GameBoard(this, QSize(_boardSize.width(), _boardSize.height()), _playerSize);

        ResizeablePlayer *player1 = new ResizeablePlayer(_playerSize, QSize(_boardSize.width(), _boardSize.height()),
                QPoint(_boardSize.x() + _playerSize.width(), _boardSize.y() + _playerSize.height()));
        _gameBoard->setPlayer1(player1);

        ResizeablePlayer *player2 = new ResizeablePlayer(_playerSize, QSize(_boardSize.width(), _boardSize.height()),
                QPoint(_boardSize.x() + _boardSize.width() - _playerSize.width(),
                       _boardSize.y() + _boardSize.height() - _playerSize.height()));
        _gameBoard->setPlayer2(player2);

        _gameBoard->reset(QVector2D(_playerSpeed, 0), QVector2D(-_playerSpeed, 0));

        this->connect(_gameBoard, SIGNAL(tickDone()), this, SLOT(repaint()));
        this->connect(_gameBoard, SIGNAL(gameOver()), this, SLOT(displayWinner()));
        this->countdown();
        break;
    }
    case 1: {
        _isGameON = true;
        if(_gameBoard != nullptr)
            delete _gameBoard;
        _gameBoard = new GameBoard(this, QSize(_boardSize.width(), _boardSize.height()), _playerSize);

        ResizeablePlayer *player1 = new ResizeablePlayer(_playerSize, QSize(_boardSize.width(), _boardSize.height()),
                QPoint(_boardSize.x() + _playerSize.width(), _boardSize.y() + _playerSize.height()));
        _gameBoard->setPlayer1(player1);

        AIPlayer *player2 = new AIPlayer(_playerSize, QSize(_boardSize.width(), _boardSize.height()),
                QPoint(_boardSize.x() + _playerSize.width(), _boardSize.y() + _playerSize.height()));
        _gameBoard->setPlayer2(player2);

        _gameBoard->reset(QVector2D(_playerSpeed, 0), QVector2D(-_playerSpeed, 0));

        this->connect(_gameBoard, SIGNAL(tickDone()), this, SLOT(repaint()));
        this->connect(_gameBoard, SIGNAL(gameOver()), this, SLOT(displayWinner()));
        this->countdown();
        break;
    }
    case 2: {
        if(_isSoundON)
            _musicThemePlayer->stop();
        else
            _musicThemePlayer->play();
        _isSoundON = !_isSoundON;
        break;
    }
    default: {
        this->close();
        break;
    }
    }
}

void Game::displayWinner() {
    _isGameOver = true;
    _speedForceTimer->stop();
    this->repaint();
}

void Game::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Escape:
    case Qt::Key_F2: {
        _countdownTimer->stop();
        _countdownCurrentTime = _countdownTime;
        if(_gameBoard != nullptr) {
            delete _gameBoard;
            _gameBoard = nullptr;
        }
        _isGameON = false;
        _isGameOver = false;
        _isCountingDown = false;
        _menuSelectedOption = 0;
        break;
    }
    case Qt::Key_H: {
        _displayFPS = !_displayFPS;
        this->repaint();
        break;
    }
    case Qt::Key_Plus: {
        QRect desktopRect = QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen());
        if(this->frameGeometry().width() + 52 <= desktopRect.width() && this->frameGeometry().height() + 32 <= desktopRect.height()) {
            this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                        QSize(this->width() + 52, this->height() + 32),
                        QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen())));
            _boardSize.setWidth(_boardSize.width() + 52);
            _boardSize.setHeight(_boardSize.height() + 32);
            _playerSize.setWidth(_playerSize.width() + 1);
            _playerSize.setHeight(_playerSize.height() + 1);
            if(_gameBoard != nullptr) {
                _gameBoard->resize(QSize(_boardSize.width(), _boardSize.height()), _playerSize);
            }
        }
        break;
    }
    case Qt::Key_Minus: {
        if(this->size().width() - 52 >= 800 && this->size().height() - 32 >= 600) {
            this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                        QSize(this->width() - 52, this->height() - 32),
                        QApplication::desktop()->availableGeometry(QApplication::desktop()->primaryScreen())));
            _boardSize.setWidth(_boardSize.width() - 52);
            _boardSize.setHeight(_boardSize.height() - 32);
            _playerSize.setWidth(_playerSize.width() - 1);
            _playerSize.setHeight(_playerSize.height() - 1);
            if(_gameBoard != nullptr) {
                _gameBoard->resize(QSize(_boardSize.width(), _boardSize.height()), _playerSize);
            }
        }
        break;
    }
    case Qt::Key_Asterisk: {
        if(_playerSpeed < 96) {
            _playerSpeed *= 2;
            if(_gameBoard != nullptr) {
                _gameBoard->reajustSpeed(_playerSpeed);
            }
            this->repaint();
        }
        break;
    }
    case Qt::Key_Slash: {
        if(_playerSpeed > 3) {
            _playerSpeed /= 2;
            if(_gameBoard != nullptr) {
                _gameBoard->reajustSpeed(_playerSpeed);
            }
            this->repaint();
        }
        break;
    }
    default:
        break;
    }
    if(_isGameON) {
        if(_isGameOver) {
            switch (event->key()) {
                case Qt::Key_Return:
                case Qt::Key_Enter: {
                    _gameBoard->reset(QVector2D(_playerSpeed, 0), QVector2D(-_playerSpeed, 0));
                    _isGameOver = false;
                    if(_gameBoard->player1Victories() >= 3 || _gameBoard->player2Victories() >= 3)
                        _gameBoard->resetVictories();
                    this->countdown();
                }
            }
        } else if (_isCountingDown){
            switch (event->key()) {
                case Qt::Key_D:
                    _gameBoard->setPlayer1Speed(QVector2D(_playerSpeed, 0)); break;
                case Qt::Key_S:
                    _gameBoard->setPlayer1Speed(QVector2D(0, _playerSpeed)); break;
                case Qt::Key_Left:
                    _gameBoard->setPlayer2Speed(QVector2D(-_playerSpeed, 0)); break;
                case Qt::Key_Up:
                    _gameBoard->setPlayer2Speed(QVector2D(0, -_playerSpeed)); break;
                default: break;
            }
        } else {
            switch (event->key()) {
                case Qt::Key_A:
                    _gameBoard->setPlayer1Speed(QVector2D(-_playerSpeed, 0)); break;
                case Qt::Key_D:
                    _gameBoard->setPlayer1Speed(QVector2D(_playerSpeed, 0)); break;
                case Qt::Key_W:
                    _gameBoard->setPlayer1Speed(QVector2D(0, -_playerSpeed)); break;
                case Qt::Key_S:
                    _gameBoard->setPlayer1Speed(QVector2D(0, _playerSpeed)); break;
                case Qt::Key_Left:
                    _gameBoard->setPlayer2Speed(QVector2D(-_playerSpeed, 0)); break;
                case Qt::Key_Right:
                    _gameBoard->setPlayer2Speed(QVector2D(_playerSpeed, 0)); break;
                case Qt::Key_Up:
                    _gameBoard->setPlayer2Speed(QVector2D(0, -_playerSpeed)); break;
                case Qt::Key_Down:
                    _gameBoard->setPlayer2Speed(QVector2D(0, _playerSpeed)); break;
                default: break;
            }
        }
    } else {
        switch (event->key()) {
            case Qt::Key_W:
            case Qt::Key_Up:
                _menuSelectedOption = (_menuSelectedOption - 1) % 4; this->playMenuSound(); break;
            case Qt::Key_S:
            case Qt::Key_Down:
                _menuSelectedOption = (_menuSelectedOption + 1) % 4; this->playMenuSound(); break;
            case Qt::Key_Return:
            case Qt::Key_Enter:
                this->menuOptionSelected(_menuSelectedOption); this->playMenuSound(); break;
            default: break;
        }
        if(_menuSelectedOption < 0)
            _menuSelectedOption = 3;
        this->repaint();
    }
}

void Game::startSpeedForce() {
    if(_isGameON) {
        if(_gameBoard->addSpeedForce()) {
            _speedForceTimer->start(4000);
        } else {
            _speedForceTimer->start(10);
        }
    }
}

void Game::playMenuSound() {
    if(_isSoundON) {
        _menuSoundPlayer->stop();
        _menuSoundPlayer->play();
    }
}
