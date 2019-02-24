#ifndef SPRITES_H
#define SPRITES_H

#include <QtGui>

class Sprites {
public:
    Sprites();
    QImage starImage;
    QImage whitedwarfImage;
    QImage blackholeImage;
    QImage asteroidImage;

private:
    QImage loadImage(char path[]);
};

#endif // SPRITES_H
