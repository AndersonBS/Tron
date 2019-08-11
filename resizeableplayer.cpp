#include "resizeableplayer.h"

ResizeablePlayer::ResizeablePlayer(QSize playerSize, QSize boardSize, QPoint initialPosition)
    :Player(playerSize, boardSize, initialPosition) {

}

void ResizeablePlayer::resize(QSize playerSize, QSize boardSize) {
    float dx = static_cast<float>(boardSize.width() * 1.0 / _boardSize.width());
    float dy = static_cast<float>(boardSize.height() * 1.0 / _boardSize.height());
    for(int index = 0; index < _path.size(); index++) {
        QPoint point = _path.at(index);
        point.setX(static_cast<int>(static_cast<double>(point.x() * dx) + 0.5));
        point.setY(static_cast<int>(static_cast<double>(point.y() * dy) + 0.5));
        _path[index] = point;
    }
    _position.setX(static_cast<int>(static_cast<double>(_position.x() * dx) + 0.5));
    _position.setY(static_cast<int>(static_cast<double>(_position.y() * dy) + 0.5));
    _playerSize = playerSize;
    _boardSize = QSize(boardSize.width(), boardSize.height());
}

QSize ResizeablePlayer::playerSize() {
    return _playerSize;
}

QSize ResizeablePlayer::boardSize() {
    return _boardSize;
}
