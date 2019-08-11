#include "player.h"

Player::Player(QSize playerSize, QSize boardSize, QPoint initialPosition) {
    _playerSize = playerSize;
    _boardSize = boardSize;
    _position = initialPosition;
}

void Player::move(float dt) {
    if(_speed.x() > 0)
       _position.setX(_position.x() + qCeil((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0));
    else
       _position.setX(_position.x() + qFloor((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0));

    if(_speed.y() > 0)
        _position.setY(_position.y() + qCeil((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0));
    else
        _position.setY(_position.y() + qFloor((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0));
}

void Player::setEnemyPath(QVector<QPoint> path) {
    _enemyPath = path;
}

void Player::setSpeed(QVector2D speed) {
    if((!(_speed.x() < 0 || _speed.x() > 0) && (speed.x() < 0 || speed.x() > 0)) ||
            (!(_speed.y() < 0 || _speed.y() > 0) && (speed.y() < 0 || speed.y() > 0))) {
        _path.append(_position);
        _speed = speed;
    }
    if(qAbs(_speed.x()) != qAbs(speed.x()) || qAbs(_speed.y()) != qAbs(speed.y()))
        _speed = speed;
}

void Player::reset(QPoint initialPosition, QVector2D speed) {
    _position = initialPosition;
    _speed = speed;
    _enemyPath.clear();
    _path.clear();
    _path.append(_position);
}

QPoint Player::position() {
    return _position;
}

QVector<QPoint> Player::path() {
    QVector<QPoint> path = _path;
    path.append(_position);
    return path;
}

