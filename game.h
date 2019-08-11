#ifndef MENU_H
#define MENU_H

#include <QFrame>
#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>
#include <QMediaPlayer>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QDateTime>
#include <QMediaPlaylist>

#include "gameboard.h"
#include "resizeableplayer.h"
#include "aiplayer.h"

class Game : public QFrame {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game();

private:
    quint8 _countdownTime;
    quint8 _playerSpeed;
    QRect _boardSize;
    QSize _playerSize;

    qint8 _menuSelectedOption;
    GameBoard *_gameBoard;
    quint8 _countdownCurrentTime;
    QTimer *_countdownTimer;
    QTimer *_speedForceTimer;

    QImage _menuBackgroundImage;
    QImage _blueBikeImage;
    QImage _redBikeImage;
    QImage _unchecedRadioButtonImage;
    QImage _checkedRadioButtonImage;
    QImage _winnerFlagImage;
    QImage _speedBoostImage;

    QMediaPlayer *_musicThemePlayer;
    QMediaPlayer *_menuSoundPlayer;

    quint16 _fps;
    qint64 _lastGameTick;
    quint8 _fpsBufferIndex;
    quint64 _fpsSum;
    qint64 _fpsBuffer[10];

    bool _isGameON;
    bool _isSoundON;
    bool _isGameOver;
    bool _isCountingDown;
    bool _displayFPS;

    void paintEvent(QPaintEvent *event);
    void paintMenu(QPainter *painter);
    void paintGameOn(QPainter *painter);
    void paintGameOver(QPainter *painter);
    void paintCountdown(QPainter *painter);
    void paintFps(QPainter *painter);
    void paintScore(QPainter *painter);

    void keyPressEvent(QKeyEvent *event);
    void menuSelectedColor(QPainter *painter, QBrush *brush);
    void menuDefaultColor(QPainter *painter, QBrush *brush);
    void menuOptionSelected(qint8 selectedOption);
    void calculateFPS();
    void playMenuSound();

private slots:
    void displayWinner();
    void countdown();
    void startSpeedForce();
};

#endif // MENU_H
