#include <QApplication>
#include <QStyle>

#include "game.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Game *menu = new Game;
    menu->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                QSize(800, 600), a.desktop()->availableGeometry(a.desktop()->primaryScreen())));
    menu->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    menu->show();

    return a.exec();
}
