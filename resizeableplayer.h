#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include <QRect>

#include "player.h"

class ResizeablePlayer: public Player {

public:
    explicit ResizeablePlayer(QSize playerSize, QSize boardSize, QPoint initialPosition);
    void resize(QSize playerSize, QSize boardSize);

    QSize playerSize();
    QSize boardSize();
};

#endif // HUMANPLAYER_H
