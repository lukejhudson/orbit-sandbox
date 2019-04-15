#ifndef SPRITES_H
#define SPRITES_H

#include <QtGui>
#include "body.h"
#include "rocket.h"

class Sprites {
public:
    Sprites();
    QPixmap getImage(Body* b);
    QPixmap getSpriteSheetImage(QPixmap spriteSheet, int width, int height, int n, int spriteWidth, int spriteHeight);

    QPixmap invalidImage;
    QPixmap backgroundImage;

    QPixmap asteroidImage;
    QPixmap planet1Image;
    QPixmap planet2Image;
    QPixmap planet3Image;
    QPixmap planet4Image;
    QPixmap planet5Image;
    QPixmap starImage;
    QPixmap whitedwarfImage;
    QPixmap blackholeImage;
    QPixmap plansysImage;

    QPixmap rocketIdleImage;
    QPixmap rocketFiringImage;
    QPixmap rocketExplosionSpriteSheet;

private:
    QPixmap loadImage(char path[]);
    QPixmap getPlanetImage(int t);
};

#endif // SPRITES_H
