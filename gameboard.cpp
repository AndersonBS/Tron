#include "gameboard.h"

GameBoard::GameBoard(QWidget *parent, QSize boardSize, QSize playerSize) : QFrame(parent) {
    _boardSize = boardSize;
    _playerSize = playerSize;
    _speedForce = QRect(-100, -100, _playerSize.width(), _playerSize.height());

    _timeAccumulator = 0;
    _maxFps = 60;
    _constantDt = 1000 / _maxFps;

    _resizeablePlayer1 = nullptr;
    _resizeablePlayer2 = nullptr;
    _aiPlayer1 = nullptr;
    _aiPlayer2 = nullptr;
    _player1Victories = 0;
    _player2Victories = 0;
}

GameBoard::~GameBoard() {
    if(_resizeablePlayer1 != nullptr) {
        delete _resizeablePlayer1;
        _resizeablePlayer1 = nullptr;
    }
    if(_resizeablePlayer2 != nullptr) {
        delete _resizeablePlayer2;
        _resizeablePlayer2 = nullptr;
    }
    if(_aiPlayer1 != nullptr) {
        delete _aiPlayer1;
        _aiPlayer1 = nullptr;
    }
    if(_aiPlayer2 != nullptr) {
        delete _aiPlayer2;
        _aiPlayer2 = nullptr;
    }
}

void GameBoard::configure() {
    _lastKey = QDateTime::currentMSecsSinceEpoch();
}

void GameBoard::reset(QVector2D player1Speed, QVector2D player2Speed) {
    _timeAccumulator = 0;
    _speedForce = QRect(-100, -100, _playerSize.width(), _playerSize.height());
    if(_resizeablePlayer1 != nullptr)
        _resizeablePlayer1->reset(QPoint(_playerSize.width(), _playerSize.height()), player1Speed);
    else if(_aiPlayer1 != nullptr)
        _aiPlayer1->reset(QPoint(_playerSize.width(), _playerSize.height()), player1Speed);
    _player1Crashed = false;
    _player1SpeedBuffer = player1Speed;
    _player1SpeedForceEngaged = false;

    if(_resizeablePlayer2 != nullptr)
        _resizeablePlayer2->reset(QPoint(_boardSize.width() - _playerSize.width(),
                                         _boardSize.height() - _playerSize.height()), player2Speed);
    else if(_aiPlayer2 != nullptr)
        _aiPlayer2->reset(QPoint(_boardSize.width() - _playerSize.width(),
                                 _boardSize.height() - _playerSize.height()), player2Speed);
    _player2Crashed = false;
    _player2SpeedBuffer = player2Speed;
    _player2SpeedForceEngaged = false;
}

void GameBoard::resize(QSize boardSize, QSize playerSize) {
    this->resizeSpeedForce(boardSize, playerSize);
    _boardSize = boardSize;
    _playerSize = playerSize;
    if(_resizeablePlayer1 != nullptr) {
        _resizeablePlayer1->resize(playerSize, boardSize);
    }
    if(_resizeablePlayer2 != nullptr) {
        _resizeablePlayer2->resize(playerSize, boardSize);
    }
}

void GameBoard::setPlayer1(ResizeablePlayer *resizeablePlayer) {
    _resizeablePlayer1 = resizeablePlayer;
}

void GameBoard::setPlayer1(AIPlayer *aiPlayer) {
    _aiPlayer1 = aiPlayer;
}

void GameBoard::setPlayer2(ResizeablePlayer *resizeablePlayer) {
    _resizeablePlayer2 = resizeablePlayer;
}

void GameBoard::setPlayer2(AIPlayer *aiPlayer) {
    _aiPlayer2 = aiPlayer;
}

void GameBoard::setPlayer1Speed(QVector2D speed) {
    if(_resizeablePlayer1 != nullptr) {
        if(_resizeablePlayer1->path().size() <= 2 || qAbs(_resizeablePlayer1->path().last().x() -
                _resizeablePlayer1->path().at(_resizeablePlayer1->path().size() - 2).x()) >
                qMax(_playerSize.width(), _playerSize.height()) || qAbs(_resizeablePlayer1->path().last().y() -
                _resizeablePlayer1->path().at(_resizeablePlayer1->path().size() - 2).y()) >
                qMax(_playerSize.width(), _playerSize.height())) {
            if(_player1SpeedForceEngaged)
                _resizeablePlayer1->setSpeed(speed * 2);
            else
                _resizeablePlayer1->setSpeed(speed);
        } else {
            _player1QuickTurnBuffer = speed;
            _player1SetQuickTurn = true;
        }
    }
    _player1SpeedBuffer = speed;
}

void GameBoard::setPlayer2Speed(QVector2D speed) {
    if(_resizeablePlayer2 != nullptr) {
        if(_resizeablePlayer2->path().size() <= 2 || qAbs(_resizeablePlayer2->path().last().x() -
                _resizeablePlayer2->path().at(_resizeablePlayer2->path().size() - 2).x()) >
                qMax(_playerSize.width(), _playerSize.height()) || qAbs(_resizeablePlayer2->path().last().y() -
                _resizeablePlayer2->path().at(_resizeablePlayer2->path().size() - 2).y()) >
                qMax(_playerSize.width(), _playerSize.height())) {
            if(_player2SpeedForceEngaged)
                _resizeablePlayer2->setSpeed(speed * 2);
            else
                _resizeablePlayer2->setSpeed(speed);
        } else {
            _player2QuickTurnBuffer = speed;
            _player2SetQuickTurn = true;
        }
    }
    _player2SpeedBuffer = speed;
}

void GameBoard::reajustSpeed(quint8 speed) {
    if(_player1SpeedBuffer.x() > 0)
        _player1SpeedBuffer.setX(speed);
    else if(_player1SpeedBuffer.x() < 0)
        _player1SpeedBuffer.setX(-speed);
    if(_player1SpeedBuffer.y() > 0)
        _player1SpeedBuffer.setY(speed);
    else if(_player1SpeedBuffer.y() < 0)
        _player1SpeedBuffer.setY(-speed);

    if(_resizeablePlayer1 != nullptr) {
        _resizeablePlayer1->setSpeed(_player1SpeedBuffer);
    } else if(_aiPlayer1 != nullptr) {
        _aiPlayer1->setSpeed(_player1SpeedBuffer);
    }

    if(_player2SpeedBuffer.x() > 0)
        _player2SpeedBuffer.setX(speed);
    else if(_player2SpeedBuffer.x() < 0)
        _player2SpeedBuffer.setX(-speed);
    if(_player2SpeedBuffer.y() > 0)
        _player2SpeedBuffer.setY(speed);
    else if(_player2SpeedBuffer.y() < 0)
        _player2SpeedBuffer.setY(-speed);

    if(_resizeablePlayer2 != nullptr) {
        _resizeablePlayer2->setSpeed(_player2SpeedBuffer);
    } else if(_aiPlayer2 != nullptr) {
        _aiPlayer2->setSpeed(_player2SpeedBuffer);
    }
}

void GameBoard::drawGameBoard(QPainter *painter) {
    painter->fillRect(0, 100, _boardSize.width() + 20, _boardSize.height() + 20, Qt::black);
    painter->fillRect(10, 110, _boardSize.width(), _boardSize.height(), Qt::white);
}

void GameBoard::drawPlayer1(QPainter *painter) {
    QVector<QPoint> path;
    if(_resizeablePlayer1 != nullptr)
        path = _resizeablePlayer1->path();
    else if(_aiPlayer1 != nullptr)
        path = _aiPlayer1->path();

    if(path.size()) {
        QPoint previousPoint = ajustPoint(path.first());
        if(path.size() <= 1) {
            painter->drawLine(previousPoint, previousPoint);
        } else {
            for(int index = 1; index < path.size(); index++) {
                QPoint currentPoint = ajustPoint(path.at(index));
                painter->drawLine(previousPoint, currentPoint);
                previousPoint = currentPoint;
            }
        }
    }
}

void GameBoard::drawPlayer2(QPainter *painter) {
    QVector<QPoint> path;
    if(_resizeablePlayer2 != nullptr)
        path = _resizeablePlayer2->path();
    else if(_aiPlayer2 != nullptr)
        path = _aiPlayer2->path();

    if(path.size()) {
        QPoint previousPoint = ajustPoint(path.first());
        if(path.size() <= 1) {
            painter->drawLine(previousPoint, previousPoint);
        } else {
            for(int index = 1; index < path.size(); index++) {
                QPoint currentPoint = ajustPoint(path.at(index));
                painter->drawLine(previousPoint, currentPoint);
                previousPoint = currentPoint;
            }
        }
    }
}

void GameBoard::drawSpeedForce(QPainter *painter, QImage image) {
    painter->drawImage(QRect(ajustPoint(QPoint(_speedForce.x(), _speedForce.y())),
                             QSize(_speedForce.width(), _speedForce.height())), image);
}

QPoint GameBoard::ajustPoint(QPoint point) {
    point.setX(point.x() + 10);
    point.setY(point.y() + 110);
    return point;
}

ResizeablePlayer *GameBoard::resizeablePlayer1() {
    return _resizeablePlayer1;
}

ResizeablePlayer *GameBoard::resizeablePlayer2() {
    return _resizeablePlayer2;
}

AIPlayer *GameBoard::aiPlayer1() {
    return _aiPlayer1;
}

AIPlayer *GameBoard::aiPlayer2() {
    return _aiPlayer2;
}

bool GameBoard::detectColision() {
    QRect player1Head;
    QRect player2Head;

    if(_resizeablePlayer1 != nullptr)
        player1Head = QRect(_resizeablePlayer1->position().x() - qCeil(_playerSize.width() / 2.0),
                            _resizeablePlayer1->position().y() - qCeil(_playerSize.height() / 2.0),
                            _playerSize.width(), _playerSize.height());
    else if(_aiPlayer1 != nullptr)
        player1Head = QRect(_aiPlayer1->position().x() - qCeil(_playerSize.width() / 2.0),
                            _aiPlayer1->position().y() - qCeil(_playerSize.height() / 2.0),
                            _playerSize.width(), _playerSize.height());

    if(_resizeablePlayer2 != nullptr)
        player2Head = QRect(_resizeablePlayer2->position().x() - qCeil(_playerSize.width() / 2.0),
                            _resizeablePlayer2->position().y() - qCeil(_playerSize.height() / 2.0),
                            _playerSize.width(), _playerSize.height());
    else if(_aiPlayer2 != nullptr)
        player2Head = QRect(_aiPlayer2->position().x() - qCeil(_playerSize.width() / 2.0),
                            _aiPlayer2->position().y() - qCeil(_playerSize.height() / 2.0),
                            _playerSize.width(), _playerSize.height());

    if(colides(player1Head, player2Head))
        return true;

    if(colides(player1Head, _speedForce)) {
        _player1SpeedForceEngaged = true;
        this->setPlayer1Speed(_player1SpeedBuffer);
        _speedForce = QRect(-100, -100, _playerSize.width(), _playerSize.height());
        QTimer::singleShot(1500, this, SLOT(player1SpeedForceFinished()));
    }

    if(colides(player2Head, _speedForce)) {
        _player2SpeedForceEngaged = true;
        this->setPlayer2Speed(_player2SpeedBuffer);
        _speedForce = QRect(-100, -100, _playerSize.width(), _playerSize.height());
        QTimer::singleShot(1500, this, SLOT(player2SpeedForceFinished()));
    }

    if(player1Head.x() < 0 || player1Head.y() < 0 ||
            player1Head.x() + player1Head.width() > _boardSize.width() ||
            player1Head.y() + player1Head.height() > _boardSize.height()) {
        _player1Crashed = true;
        _player2Victories++;
        return true;
    }

    if(player2Head.x() < 0 || player2Head.y() < 0 ||
            player2Head.x() + player2Head.width() > _boardSize.width() ||
            player2Head.y() + player2Head.height() > _boardSize.height()) {
        _player2Crashed = true;
        _player1Victories++;
        return true;
    }

    quint16 player1PathSize;
    if(_resizeablePlayer1 != nullptr)
        player1PathSize = static_cast<quint16>(_resizeablePlayer1->path().size());
    else if(_aiPlayer1 != nullptr)
        player1PathSize = static_cast<quint16>(_aiPlayer1->path().size());
    else
        player1PathSize = 0;

    for(int index = 0; index < player1PathSize - 1; index++) {
        QPoint currentPoint, nextPoint;
        if(_resizeablePlayer1 != nullptr) {
            currentPoint = _resizeablePlayer1->path().at(index);
            nextPoint = _resizeablePlayer1->path().at(index + 1);
        } else if(_aiPlayer1 != nullptr) {
            currentPoint = _aiPlayer1->path().at(index);
            nextPoint = _aiPlayer1->path().at(index + 1);
        }

        QRect player1Body;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            player1Body.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            player1Body.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            player1Body.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            player1Body.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            player1Body.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            player1Body.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            player1Body.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            player1Body.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < player1PathSize - 3 && colides(player1Head, player1Body)) {
            _player1Crashed = true;
            _player2Victories++;
            return true;
        }

        if(colides(player2Head, player1Body)) {
            _player2Crashed = true;
            _player1Victories++;
            return true;
        }
    }

    quint16 player2PathSize;
    if(_resizeablePlayer2 != nullptr)
        player2PathSize = static_cast<quint16>(_resizeablePlayer2->path().size());
    else if(_aiPlayer2 != nullptr)
        player2PathSize = static_cast<quint16>(_aiPlayer2->path().size());
    else
        player2PathSize = 0;

    for(int index = 0; index < player2PathSize - 1; index++) {
        QPoint currentPoint, nextPoint;
        if(_resizeablePlayer2 != nullptr) {
            currentPoint = _resizeablePlayer2->path().at(index);
            nextPoint = _resizeablePlayer2->path().at(index + 1);
        } else if(_aiPlayer2 != nullptr) {
            currentPoint = _aiPlayer2->path().at(index);
            nextPoint = _aiPlayer2->path().at(index + 1);
        }

        QRect player2Body;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            player2Body.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            player2Body.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            player2Body.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            player2Body.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            player2Body.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            player2Body.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            player2Body.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            player2Body.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < player2PathSize - 3 && colides(player2Head, player2Body)) {
            _player2Crashed = true;
            _player1Victories++;
            return true;
        }

        if(colides(player1Head, player2Body)) {
            _player1Crashed = true;
            _player2Victories++;
            return true;
        }
    }

    return false;
}

bool GameBoard::colides(QRect r1, QRect r2) {
    if(r1.x() + r1.width() < r2.x() || r2.x() + r2.width() < r1.x() ||
            r1.y() + r1.height() < r2.y() || r2.y() + r2.height() < r1.y()) {
        return false;
    }
    return true;
}

void GameBoard::resizeSpeedForce(QSize boardSize, QSize playerSize) {
    float dx = static_cast<float>(boardSize.width() * 1.0 / _boardSize.width());
    float dy = static_cast<float>(boardSize.height() * 1.0 / _boardSize.height());
    if(_speedForce.x() >= 0 && _speedForce.y() >= 0) {
        _speedForce.setX(static_cast<int>(static_cast<double>(_speedForce.x() * dx) + 0.5));
        _speedForce.setY(static_cast<int>(static_cast<double>(_speedForce.y() * dy) + 0.5));
        _speedForce.setWidth(playerSize.width());
        _speedForce.setHeight(playerSize.height());
    }
}

void GameBoard::tick() {
    qint64 key = QDateTime::currentMSecsSinceEpoch();
    qint64 timeSinceLastKey = key - _lastKey;
    _timeAccumulator += timeSinceLastKey;

    while(_timeAccumulator >= _constantDt) {
        if(_resizeablePlayer1 != nullptr)
            _resizeablePlayer1->move(_constantDt);
        else if(_aiPlayer1 != nullptr) {
            if(_resizeablePlayer2 != nullptr)
                _aiPlayer1->setEnemyPath(_resizeablePlayer2->path());
            else if(_aiPlayer2 != nullptr)
                _aiPlayer1->setEnemyPath(_aiPlayer2->path());
            _aiPlayer1->move(_constantDt);
        }

        if(_resizeablePlayer2 != nullptr)
            _resizeablePlayer2->move(_constantDt);
        else if(_aiPlayer2 != nullptr) {
            if(_resizeablePlayer1 != nullptr)
                _aiPlayer2->setEnemyPath(_resizeablePlayer1->path());
            else if(_aiPlayer1 != nullptr)
                _aiPlayer2->setEnemyPath(_aiPlayer1->path());
            _aiPlayer2->move(_constantDt);
        }

        if(_player1SetQuickTurn) {
            _player1SetQuickTurn = false;
            this->setPlayer1Speed(_player1QuickTurnBuffer);
        }

        if(_player2SetQuickTurn) {
            _player2SetQuickTurn = false;
            this->setPlayer2Speed(_player2QuickTurnBuffer);
        }

        _timeAccumulator -= _constantDt;

        if(this->detectColision()) {
            emit gameOver();
            return;
        }
    }

    _lastKey = key;
    emit tickDone();
    QTimer::singleShot(1000 / _maxFps, this, SLOT(tick()));
}

quint8 GameBoard::player1Victories() {
    return _player1Victories;
}

quint8 GameBoard::player2Victories() {
    return _player2Victories;
}

bool GameBoard::player1Crashed() {
    return _player1Crashed;
}

bool GameBoard::player2Crashed() {
    return _player2Crashed;
}

void GameBoard::resetVictories() {
    _player1Victories = 0;
    _player2Victories = 0;
}

void GameBoard::player1SpeedForceFinished() {
    _player1SpeedForceEngaged = false;
    this->setPlayer1Speed(_player1SpeedBuffer);
}

void GameBoard::player2SpeedForceFinished() {
    _player2SpeedForceEngaged = false;
    this->setPlayer2Speed(_player2SpeedBuffer);
}

bool GameBoard::addSpeedForce() {
    QRect randomRect((qrand() % (_boardSize.width() - _playerSize.width())),
                     (qrand() % (_boardSize.height() - _playerSize.width())),
                     _playerSize.width(), _playerSize.height());

    quint16 player1PathSize;
    if(_resizeablePlayer1 != nullptr)
        player1PathSize = static_cast<quint16>(_resizeablePlayer1->path().size());
    else if(_aiPlayer1 != nullptr)
        player1PathSize = static_cast<quint16>(_aiPlayer1->path().size());
    else
        player1PathSize = 0;

    for(int index = 0; index < player1PathSize - 1; index++) {
        QPoint currentPoint, nextPoint;
        if(_resizeablePlayer1 != nullptr) {
            currentPoint = _resizeablePlayer1->path().at(index);
            nextPoint = _resizeablePlayer1->path().at(index + 1);
        } else if(_aiPlayer1 != nullptr) {
            currentPoint = _aiPlayer1->path().at(index);
            nextPoint = _aiPlayer1->path().at(index + 1);
        }

        QRect player1Body;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            player1Body.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            player1Body.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            player1Body.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            player1Body.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            player1Body.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            player1Body.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            player1Body.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            player1Body.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(randomRect, player1Body)) {
            return false;
        }
    }

    quint16 player2PathSize;
    if(_resizeablePlayer2 != nullptr)
        player2PathSize = static_cast<quint16>(_resizeablePlayer2->path().size());
    else if(_aiPlayer2 != nullptr)
        player2PathSize = static_cast<quint16>(_aiPlayer2->path().size());
    else
        player2PathSize = 0;

    for(int index = 0; index < player2PathSize - 1; index++) {
        QPoint currentPoint, nextPoint;
        if(_resizeablePlayer2 != nullptr) {
            currentPoint = _resizeablePlayer2->path().at(index);
            nextPoint = _resizeablePlayer2->path().at(index + 1);
        } else if(_aiPlayer2 != nullptr) {
            currentPoint = _aiPlayer2->path().at(index);
            nextPoint = _aiPlayer2->path().at(index + 1);
        }

        QRect player2Body;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            player2Body.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            player2Body.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            player2Body.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            player2Body.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            player2Body.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            player2Body.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            player2Body.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            player2Body.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(randomRect, player2Body)) {
            return false;
        }
    }

    _speedForce = randomRect;
    return true;
}
