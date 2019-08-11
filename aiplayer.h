#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <QRect>

#include "player.h"

class AIPlayer: public Player {

public:
    explicit AIPlayer(QSize playerSize, QSize boardSize, QPoint initialPosition);

    void reset(QPoint initialPosition, QVector2D speed);
    void move(float dt);

private:
    qint8 _speedConstant;
    bool _splitMap;
    bool _goingUp;
    bool _isFillingClockwise;

    bool colides(QRect r1, QRect r2);
    bool isAboutToColide(float dt);
    bool ambushAheadOnRight(float dt);
    bool ambushAheadOnLeft(float dt);
    bool ambushOnLeft(float dt);
    bool ambushOnRight(float dt);
    bool isTwiceLenghLong();
    bool isEnemyHeadUp();
    
    bool canTurnLeft();
    bool canTurnRight();
    void turnLeft();
    void turnRight();
};

#endif // AIPLAYER_H
