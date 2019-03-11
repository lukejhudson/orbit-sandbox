#ifndef SPRITES_H
#define SPRITES_H

#include <QtGui>

class Sprites {
public:
    Sprites();
    QImage getPlanetImage(int t);

    QImage asteroidImage;
    QImage planet1Image;
    QImage planet2Image;
    QImage planet3Image;
    QImage planet4Image;
    QImage planet5Image;
    QImage starImage;
    QImage whitedwarfImage;
    QImage blackholeImage;
    QImage plansysImage;

private:
    QImage loadImage(char path[]);
};

#endif // SPRITES_H
