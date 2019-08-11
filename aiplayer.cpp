#include "aiplayer.h"

AIPlayer::AIPlayer(QSize playerSize, QSize boardSize, QPoint initialPosition)
    :Player(playerSize, boardSize, initialPosition) {
    _splitMap = true;
    _goingUp = false;
}

void AIPlayer::move(float dt) {
    if(_splitMap) {
        if(isAboutToColide(dt)) {
            _isFillingClockwise = !this->isEnemyHeadUp();
            if(_isFillingClockwise) {
                if(this->canTurnRight())
                    this->turnRight();
            } else {
                if(this->canTurnLeft())
                    this->turnLeft();
            }
            _splitMap = false;
        } else if(this->ambushAheadOnRight(dt) || this->ambushOnLeft(dt)) {
            _isFillingClockwise = !this->isEnemyHeadUp();
            if(_isFillingClockwise) {
                if(this->canTurnRight())
                    this->turnRight();
            } else {
                if(this->canTurnLeft())
                    this->turnLeft();
            }
            _splitMap = false;
        } else {
            if(_goingUp) {
                if(this->canTurnLeft()) {
                    this->turnLeft();
                    _goingUp = !_goingUp;
                }
            } else {
                if(this->isTwiceLenghLong() && this->canTurnRight()) {
                    this->turnRight();
                    _goingUp = !_goingUp;
                }
            }
        }
    } else {
        if(_isFillingClockwise) {
            if(isAboutToColide(dt)) {
                if(this->canTurnRight())
                    this->turnRight();
                else if(this->canTurnLeft())
                    this->turnLeft();
            } else if(this->ambushAheadOnRight(dt) && this->canTurnRight()) {
                this->turnRight();
            } else if(canTurnLeft() && !ambushOnLeft(dt)) {
                this->turnLeft();
            }
        } else {
            if(isAboutToColide(dt)) {
                if(this->canTurnLeft())
                    this->turnLeft();
                else if(this->canTurnRight())
                    this->turnRight();
            } else if(this->ambushAheadOnLeft(dt) && this->canTurnLeft()) {
                this->turnLeft();
            } else if(canTurnRight() && !ambushOnRight(dt)) {
                this->turnRight();
            }
        }
    }

    if(_speed.x() > 0)
       _position.setX(_position.x() + qCeil((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0));
    else
       _position.setX(_position.x() + qFloor((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0));

    if(_speed.y() > 0)
        _position.setY(_position.y() + qCeil((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0));
    else
        _position.setY(_position.y() + qFloor((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0));
}

bool AIPlayer::colides(QRect r1, QRect r2) {
    if(r1.x() + r1.width() < r2.x() || r2.x() + r2.width() < r1.x() ||
            r1.y() + r1.height() < r2.y() || r2.y() + r2.height() < r1.y()) {
        return false;
    }
    return true;
}

bool AIPlayer::isAboutToColide(float dt) {
    int newX, newY;
    if(_speed.x() > 0)
       newX = _position.x() + qCeil((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0);
    else
       newX = _position.x() + qFloor((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0);

    if(_speed.y() > 0)
        newY = _position.y() + qCeil((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0);
    else
        newY = _position.y() + qFloor((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0);

    QRect playerHead = QRect(newX - qCeil(_playerSize.width() / 2.0),
            newY - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());

    if(playerHead.x() < 0 || playerHead.y() < 0 ||
            playerHead.x() + playerHead.width() > _boardSize.width() ||
            playerHead.y() + playerHead.height() > _boardSize.height())
        return true;

    for(int index = 0; index < _path.size() - 1; index++) {
        QPoint currentPoint = _path.at(index), nextPoint = _path.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < _path.size() - 3 && colides(playerHead, playerBody))
            return true;
    }

    for(int index = 0; index < _enemyPath.size() - 1; index++) {
        QPoint currentPoint = _enemyPath.at(index), nextPoint = _enemyPath.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(playerHead, playerBody))
            return true;
    }

    return false;
}

bool AIPlayer::ambushAheadOnRight(float dt) {
    int newX, newY;
    if(_speed.x() < 0 || _speed.x() > 0) {
        if(_speed.x() > 0) {
            newY = static_cast<int>(_position.y() + qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newX = static_cast<int>(_position.x() + qCeil((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0));
        } else {
            newY = static_cast<int>(_position.y() - qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newX = static_cast<int>(_position.x() + qFloor((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0));
        }
    } else {
        if(_speed.y() > 0) {
            newX = static_cast<int>(_position.x() - qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newY = static_cast<int>(_position.y() + qCeil((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0));
        } else {
            newX = static_cast<int>(_position.x() + qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newY = static_cast<int>(_position.y() + qFloor((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0));
        }
    }

    QRect playerHead = QRect(newX - qCeil(_playerSize.width() / 2.0),
            newY - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());

    if(playerHead.x() < 0 || playerHead.y() < 0 ||
            playerHead.x() + playerHead.width() > _boardSize.width() ||
            playerHead.y() + playerHead.height() > _boardSize.height())
        return true;

    for(int index = 0; index < _path.size() - 1; index++) {
        QPoint currentPoint = _path.at(index), nextPoint = _path.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < _path.size() - 3 && colides(playerHead, playerBody))
            return true;
    }

    for(int index = 0; index < _enemyPath.size() - 1; index++) {
        QPoint currentPoint = _enemyPath.at(index), nextPoint = _enemyPath.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(playerHead, playerBody))
            return true;
    }

    return false;
}

bool AIPlayer::ambushAheadOnLeft(float dt) {
    int newX, newY;
    if(_speed.x() < 0 || _speed.x() > 0) {
        if(_speed.x() > 0) {
            newY = static_cast<int>(_position.y() - qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newX = static_cast<int>(_position.x() + qCeil((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0));
        } else {
            newY = static_cast<int>(_position.y() + qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newX = static_cast<int>(_position.x() + qFloor((static_cast<double>(_speed.x() * dt) * 60.0) / 1000.0));
        }
    } else {
        if(_speed.y() > 0) {
            newX = static_cast<int>(_position.x() + qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newY = static_cast<int>(_position.y() + qCeil((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0));
        } else {
            newX = static_cast<int>(_position.x() - qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newY = static_cast<int>(_position.y() + qFloor((static_cast<double>(_speed.y() * dt) * 60.0) / 1000.0));
        }
    }

    QRect playerHead = QRect(newX - qCeil(_playerSize.width() / 2.0),
            newY - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());

    if(playerHead.x() < 0 || playerHead.y() < 0 ||
            playerHead.x() + playerHead.width() > _boardSize.width() ||
            playerHead.y() + playerHead.height() > _boardSize.height())
        return true;

    for(int index = 0; index < _path.size() - 1; index++) {
        QPoint currentPoint = _path.at(index), nextPoint = _path.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < _path.size() - 3 && colides(playerHead, playerBody))
            return true;
    }

    for(int index = 0; index < _enemyPath.size() - 1; index++) {
        QPoint currentPoint = _enemyPath.at(index), nextPoint = _enemyPath.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(playerHead, playerBody))
            return true;
    }

    return false;
}

bool AIPlayer::ambushOnLeft(float dt) {
    QVector2D newSpeed = _speed;
    if(_speed.x() < 0 || _speed.x() > 0) {
        if(_speed.x() > 0)
            newSpeed.setY(-_speedConstant);
        else
            newSpeed.setY(_speedConstant);
        newSpeed.setX(0);
    } else {
        if(_speed.y() > 0)
            newSpeed.setX(_speedConstant);
        else
            newSpeed.setX(-_speedConstant);
        newSpeed.setY(0);
    }

    int newX, newY;
    if(newSpeed.x() < 0 || newSpeed.x() > 0) {
        if(newSpeed.x() > 0) {
            newY = static_cast<int>(_position.y() + qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newX = static_cast<int>(_position.x() + qCeil((static_cast<double>(newSpeed.x() * dt) * 60.0) / 1000.0));
        } else {
            newY = static_cast<int>(_position.y() - qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newX = static_cast<int>(_position.x() + qFloor((static_cast<double>(newSpeed.x() * dt) * 60.0) / 1000.0));
        }
    } else {
        if(newSpeed.y() > 0) {
            newX = static_cast<int>(_position.x() - qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newY = static_cast<int>(_position.y() + qCeil((static_cast<double>(newSpeed.y() * dt) * 60.0) / 1000.0));
        } else {
            newX = static_cast<int>(_position.x() + qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newY = static_cast<int>(_position.y() + qFloor((static_cast<double>(newSpeed.y() * dt) * 60.0) / 1000.0));
        }
    }

    QRect playerHead = QRect(newX - qCeil(_playerSize.width() / 2.0),
            newY - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());

    if(playerHead.x() < 0 || playerHead.y() < 0 ||
            playerHead.x() + playerHead.width() > _boardSize.width() ||
            playerHead.y() + playerHead.height() > _boardSize.height())
        return true;

    for(int index = 0; index < _path.size() - 1; index++) {
        QPoint currentPoint = _path.at(index), nextPoint = _path.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < _path.size() - 3 && colides(playerHead, playerBody))
            return true;
    }

    for(int index = 0; index < _enemyPath.size() - 1; index++) {
        QPoint currentPoint = _enemyPath.at(index), nextPoint = _enemyPath.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(playerHead, playerBody))
            return true;
    }

    return false;
}

bool AIPlayer::ambushOnRight(float dt) {
    QVector2D newSpeed = _speed;
    if(_speed.x() < 0 || _speed.x() > 0) {
        if(_speed.x() > 0)
            newSpeed.setY(_speedConstant);
        else
            newSpeed.setY(-_speedConstant);
        newSpeed.setX(0);
    } else {
        if(_speed.y() > 0)
            newSpeed.setX(-_speedConstant);
        else
            newSpeed.setX(_speedConstant);
        newSpeed.setY(0);
    }

    int newX, newY;
    if(newSpeed.x() < 0 || newSpeed.x() > 0) {
        if(newSpeed.x() > 0) {
            newY = static_cast<int>(_position.y() - qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newX = static_cast<int>(_position.x() + qCeil((static_cast<double>(newSpeed.x() * dt) * 60.0) / 1000.0));
        } else {
            newY = static_cast<int>(_position.y() + qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newX = static_cast<int>(_position.x() + qFloor((static_cast<double>(newSpeed.x() * dt) * 60.0) / 1000.0));
        }
    } else {
        if(newSpeed.y() > 0) {
            newX = static_cast<int>(_position.x() + qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newY = static_cast<int>(_position.y() + qCeil((static_cast<double>(newSpeed.y() * dt) * 60.0) / 1000.0));
        } else {
            newX = static_cast<int>(_position.x() - qMax(_playerSize.width(), _playerSize.height()) * 1.35);
            newY = static_cast<int>(_position.y() + qFloor((static_cast<double>(newSpeed.y() * dt) * 60.0) / 1000.0));
        }
    }

    QRect playerHead = QRect(newX - qCeil(_playerSize.width() / 2.0),
            newY - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());

    if(playerHead.x() < 0 || playerHead.y() < 0 ||
            playerHead.x() + playerHead.width() > _boardSize.width() ||
            playerHead.y() + playerHead.height() > _boardSize.height())
        return true;

    for(int index = 0; index < _path.size() - 1; index++) {
        QPoint currentPoint = _path.at(index), nextPoint = _path.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < _path.size() - 3 && colides(playerHead, playerBody))
            return true;
    }

    for(int index = 0; index < _enemyPath.size() - 1; index++) {
        QPoint currentPoint = _enemyPath.at(index), nextPoint = _enemyPath.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(playerHead, playerBody))
            return true;
    }

    return false;
}

bool AIPlayer::canTurnLeft() {
    QRect playerHead = QRect(_position.x() - qCeil(_playerSize.width() / 2.0),
            _position.y() - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());
    QRect playerLastTurn = QRect(_path.last().x() - qCeil(_playerSize.width() / 2.0),
            _path.last().y() - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());
    if(colides(playerHead, playerLastTurn))
        return false;

    int newX, newY;
    if(_speed.x() < 0 || _speed.x() > 0) {
        if(_speed.x() > 0)
            newY = static_cast<int>(_position.y() - qMax(_playerSize.width(), _playerSize.height()) * 1.2);
        else
            newY = static_cast<int>(_position.y() + qMax(_playerSize.width(), _playerSize.height()) * 1.2);
        newX = _position.x();
    } else {
        if(_speed.y() > 0)
            newX = static_cast<int>(_position.x() + qMax(_playerSize.width(), _playerSize.height()) * 1.2);
        else
            newX = static_cast<int>(_position.x() - qMax(_playerSize.width(), _playerSize.height()) * 1.2);
        newY = _position.y();
    }

    QRect playerNewHead = QRect(newX - qCeil(_playerSize.width() / 2.0),
            newY - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());

    if(playerNewHead.x() < 0 || playerNewHead.y() < 0 ||
            playerNewHead.x() + playerNewHead.width() > _boardSize.width() ||
            playerNewHead.y() + playerNewHead.height() > _boardSize.height())
        return false;

    for(int index = 0; index < _path.size() - 1; index++) {
        QPoint currentPoint = _path.at(index), nextPoint = _path.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < _path.size() - 2 && colides(playerNewHead, playerBody))
            return false;
    }

    for(int index = 0; index < _enemyPath.size() - 1; index++) {
        QPoint currentPoint = _enemyPath.at(index), nextPoint = _enemyPath.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(playerNewHead, playerBody))
            return false;
    }

    return true;
}

bool AIPlayer::canTurnRight() {
    QRect playerHead = QRect(_position.x() - qCeil(_playerSize.width() / 2.0),
            _position.y() - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());
    QRect playerLastTurn = QRect(_path.last().x() - qCeil(_playerSize.width() / 2.0),
            _path.last().y() - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());
    if(colides(playerHead, playerLastTurn))
        return false;

    int newX, newY;
    if(_speed.x() < 0 || _speed.x() > 0) {
        if(_speed.x() > 0)
            newY = static_cast<int>(_position.y() + qMax(_playerSize.width(), _playerSize.height()) * 1.2);
        else
            newY = static_cast<int>(_position.y() - qMax(_playerSize.width(), _playerSize.height()) * 1.2);
        newX = _position.x();
    } else {
        if(_speed.y() > 0)
            newX = static_cast<int>(_position.x() - qMax(_playerSize.width(), _playerSize.height()) * 1.2);
        else
            newX = static_cast<int>(_position.x() + qMax(_playerSize.width(), _playerSize.height()) * 1.2);
        newY = _position.y();
    }

    QRect playerNewHead = QRect(newX - qCeil(_playerSize.width() / 2.0),
            newY - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());

    if(playerNewHead.x() < 0 || playerNewHead.y() < 0 ||
            playerNewHead.x() + playerNewHead.width() > _boardSize.width() ||
            playerNewHead.y() + playerNewHead.height() > _boardSize.height())
        return false;

    for(int index = 0; index < _path.size() - 1; index++) {
        QPoint currentPoint = _path.at(index), nextPoint = _path.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(index < _path.size() - 2 && colides(playerNewHead, playerBody))
            return false;
    }

    for(int index = 0; index < _enemyPath.size() - 1; index++) {
        QPoint currentPoint = _enemyPath.at(index), nextPoint = _enemyPath.at(index + 1);

        QRect playerBody;
        if(currentPoint.x() - nextPoint.x() >= 0) {
            playerBody.setX(nextPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(currentPoint.x() - nextPoint.x() + _playerSize.width());
        } else {
            playerBody.setX(currentPoint.x() - qCeil(_playerSize.width() / 2.0));
            playerBody.setWidth(nextPoint.x() - currentPoint.x() + _playerSize.width());
        }
        if(currentPoint.y() - nextPoint.y() >= 0) {
            playerBody.setY(nextPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(currentPoint.y() - nextPoint.y() + _playerSize.height());
        } else {
            playerBody.setY(currentPoint.y() - qCeil(_playerSize.height() / 2.0));
            playerBody.setHeight(nextPoint.y() - currentPoint.y() + _playerSize.height());
        }

        if(colides(playerNewHead, playerBody))
            return false;
    }

    return true;
}

bool AIPlayer::isTwiceLenghLong() {
    QRect playerHead = QRect(_position.x() -_playerSize.width() - qCeil(_playerSize.width() / 2.0),
            _position.y() - _playerSize.height() - qCeil(_playerSize.height() / 2.0),
            static_cast<int>(_playerSize.width() * 2.8), static_cast<int>(_playerSize.height() * 2.8));
    QRect playerLastTurn = QRect(_path.last().x() - qCeil(_playerSize.width() / 2.0),
            _path.last().y() - qCeil(_playerSize.height() / 2.0), _playerSize.width(), _playerSize.height());
    if(colides(playerHead, playerLastTurn))
        return false;
    return true;
}

void AIPlayer::reset(QPoint initialPosition, QVector2D speed) {
    _position = initialPosition;
    _speed = speed;
    _speedConstant = static_cast<qint8>(qMax(qAbs(_speed.x()), qAbs(_speed.y())));
    _splitMap = true;
    _goingUp = false;
    _enemyPath.clear();
    _path.clear();
    _path.append(_position);
}

bool AIPlayer::isEnemyHeadUp() {
    QPoint enemyHead = _enemyPath.last();
    if(enemyHead.x() > enemyHead.y() / (_boardSize.height() * 1.0 / _boardSize.width()))
        return true;
    return false;
}

void AIPlayer::turnLeft() {
    _path.append(_position);
    if(_speed.x() < 0 || _speed.x() > 0) {
        if(_speed.x() > 0)
            _speed.setY(-_speedConstant);
        else
            _speed.setY(_speedConstant);
        _speed.setX(0);
    } else {
        if(_speed.y() > 0)
            _speed.setX(_speedConstant);
        else
            _speed.setX(-_speedConstant);
        _speed.setY(0);
    }
}

void AIPlayer::turnRight() {
    _path.append(_position);
    if(_speed.x() < 0 || _speed.x() > 0) {
        if(_speed.x() > 0)
            _speed.setY(_speedConstant);
        else
            _speed.setY(-_speedConstant);
        _speed.setX(0);
    } else {
        if(_speed.y() > 0)
            _speed.setX(-_speedConstant);
        else
            _speed.setX(_speedConstant);
        _speed.setY(0);
    }
}
