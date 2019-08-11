#ifndef PLAYER_H
#define PLAYER_H

#include <QSize>
#include <QVector2D>
#include <QPoint>
#include <QVector>
#include <qmath.h>

class Player {

public:
    explicit Player(QSize playerSize, QSize boardSize, QPoint initialPosition);

    void move(float dt);

    void setEnemyPath(QVector<QPoint> path);
    void setSpeed(QVector2D speed);
    void reset(QPoint initialPosition, QVector2D speed);

    QPoint position();
    QVector<QPoint> path();

protected:
    QSize _playerSize;
    QSize _boardSize;
    QPoint _position;

    QVector<QPoint> _enemyPath;
    QVector2D _speed;
    QVector<QPoint> _path;

};

#endif // PLAYER_H
