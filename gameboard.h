#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QFrame>
#include <QDateTime>
#include <QPainter>
#include <QTimer>
#include <qmath.h>

#include "resizeableplayer.h"
#include "aiplayer.h"

class GameBoard : public QFrame {
    Q_OBJECT

public:
    explicit GameBoard(QWidget *parent, QSize boardSize, QSize playerSize);
    ~GameBoard();

    void configure();
    void reset(QVector2D player1Speed, QVector2D player2Speed);
    void resize(QSize boardSize, QSize playerSize);

    void setPlayer1(ResizeablePlayer *resizeablePlayer);
    void setPlayer1(AIPlayer *aiPlayer);
    void setPlayer2(ResizeablePlayer *resizeablePlayer);
    void setPlayer2(AIPlayer *aiPlayer);
    void setPlayer1Speed(QVector2D speed);
    void setPlayer2Speed(QVector2D speed);
    void reajustSpeed(quint8 speed);
    bool addSpeedForce();

    void drawGameBoard(QPainter *painter);
    void drawPlayer1(QPainter *painter);
    void drawPlayer2(QPainter *painter);
    void drawSpeedForce(QPainter *painter, QImage image);
    QPoint ajustPoint(QPoint);

    ResizeablePlayer *resizeablePlayer1();
    ResizeablePlayer *resizeablePlayer2();
    AIPlayer *aiPlayer1();
    AIPlayer *aiPlayer2();
    quint8 player1Victories();
    quint8 player2Victories();
    bool player1Crashed();
    bool player2Crashed();
    void resetVictories();

private:
    ResizeablePlayer *_resizeablePlayer1;
    ResizeablePlayer *_resizeablePlayer2;
    AIPlayer *_aiPlayer1;
    AIPlayer *_aiPlayer2;
    quint8 _player1Victories;
    quint8 _player2Victories;
    bool _player1SpeedForceEngaged;
    bool _player2SpeedForceEngaged;
    bool _player1Crashed;
    bool _player2Crashed;
    bool _player1SetQuickTurn;
    bool _player2SetQuickTurn;
    QVector2D _player1QuickTurnBuffer;
    QVector2D _player2QuickTurnBuffer;
    QVector2D _player1SpeedBuffer;
    QVector2D _player2SpeedBuffer;

    QSize _boardSize;
    QSize _playerSize;
    QRect _speedForce;
    qint64 _lastKey;
    qint64 _timeAccumulator;
    quint8 _maxFps;
    float _constantDt;

    bool detectColision();
    bool colides(QRect r1, QRect r2);
    void resizeSpeedForce(QSize boardSize, QSize playerSize);

signals:
    void tickDone();
    void gameOver();

public slots:
    void tick();
    void player1SpeedForceFinished();
    void player2SpeedForceFinished();

};

#endif // GAMEBOARD_H
