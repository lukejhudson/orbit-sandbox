#include <iostream>
#include "sprites.h"

Sprites::Sprites() {
    // Load all images
    char invalidPath[] = "/sprites/invalid.png";
    invalidImage = loadImage(invalidPath);

    char backgroundPath[] = "/sprites/background.png";
    backgroundImage = loadImage(backgroundPath);

    char asteroidPath[] = "/sprites/asteroid.png";
    asteroidImage = loadImage(asteroidPath);

    char planet1Path[] = "/sprites/planet1.png";
    planet1Image = loadImage(planet1Path);

    char planet2Path[] = "/sprites/planet2.png";
    planet2Image = loadImage(planet2Path);

    char planet3Path[] = "/sprites/planet3.png";
    planet3Image = loadImage(planet3Path);

    char planet4Path[] = "/sprites/planet4.png";
    planet4Image = loadImage(planet4Path);

    char planet5Path[] = "/sprites/planet5.png";
    planet5Image = loadImage(planet5Path);

    char starPath[] = "/sprites/star.png";
    starImage = loadImage(starPath);

    char whitedwarfPath[] = "/sprites/whitedwarf.png";
    whitedwarfImage = loadImage(whitedwarfPath);

    char blackholePath[] = "/sprites/blackhole.png";
    blackholeImage = loadImage(blackholePath);

    char plansysPath[] = "/sprites/plansys.png";
    plansysImage = loadImage(plansysPath);

    char rocketIdlePath[] = "/sprites/rocket.png";
    rocketIdleImage = loadImage(rocketIdlePath);

    char rocketFiringPath[] = "/sprites/rocketfiring.png";
    rocketFiringImage = loadImage(rocketFiringPath);

    char rocketExplosionPath[] = "/sprites/rocketexplosion.png";
    rocketExplosionSpriteSheet = loadImage(rocketExplosionPath);
}

/**
 * @brief Sprites::loadImage Reads and returns the image file from the given
 * path.
 * @param path The path from the executable to the image
 * @return The requested image as a QPixmap
 */
QPixmap Sprites::loadImage(char path[]) {
    QPixmap img(QDir::currentPath() + path);
    if (img.isNull()) {
        std::cout << "Could not load sprite: " << std::endl;// << reader.errorString().toStdString() << std::endl;
    }
    return img;
}

/**
 * @brief Sprites::getImage
 * @param b
 * @return
 */
QPixmap Sprites::getImage(Body* b) {
    Rocket *r;
    switch (b->getType()) {
    case Body::Asteroid:
        return asteroidImage;
    case Body::Planet:
        return getPlanetImage(b->getPlanetType());
    case Body::Star:
        return starImage;
    case Body::WhiteDwarf:
        return whitedwarfImage;
    case Body::BlackHole:
        return blackholeImage;
    case Body::PlayerRocket:
        r = static_cast<Rocket*>(b);
        if (r->isFiring()) {
            return rocketFiringImage;
        } else {
            return rocketIdleImage;
        }
    default:
        return invalidImage;
    }
}

/**
 * @brief Sprites::getPlanetImage Returns the planet image corresponding to
 * the given planet type.
 * @param t The planet type
 * @return The planet image corresponding to the given planet type
 */
QPixmap Sprites::getPlanetImage(int t) {
    switch (t) {
        case 1:
            return planet1Image;
        case 2:
            return planet2Image;
        case 3:
            return planet3Image;
        case 4:
            return planet4Image;
        case 5:
            return planet5Image;
        default:
            return planet1Image;
    }
}

/**
 * @brief Sprites::getSpriteSheetImage Returns the n'th sprite in the sprite
 * sheet spriteSheet, where there are width number of images in each row of
 * the sprite sheet, and height number of images in each column.
 * @param width The number of images in each row of the sprite sheet
 * @param height The number of images in each column of the sprite sheet
 * @param n The number of the sprite in the sprite sheet, counting from top
 * left to bottom right
 * @param spriteWidth Width in pixels of the sprite to be drawn
 * @param spriteHeight Height in pixels of the sprite to be drawn
 * @return The cropped sprite sheet containing just the image requested
 */
QPixmap Sprites::getSpriteSheetImage(QPixmap spriteSheet, int width, int height, int n, int spriteWidth, int spriteHeight) {
    // Find row and column of where the requested sprite is located within the sprite sheet
    // Top left = (0, 0)
    int row = n / width;
    int col = n % width;

    int sheetWidth = spriteSheet.width();
    int sheetHeight = spriteSheet.height();
    // Width in pixels for each sprite = number of pixels per row / number of sprites per row
    int widthPerSprite = sheetWidth / width;
    int heightPerSprite = sheetHeight / height;

    QPixmap sprite(widthPerSprite, heightPerSprite);
    // Wipe the sprite clean and fill it with transparent pixels
    sprite.fill(QColor(0,0,0,0));
    QPainter p(&sprite);
    // Draw the sprite in the sprite sheet onto its own QPixmap
    p.drawPixmap(0, 0,                                          // Origin of target (sprite)
                 widthPerSprite, heightPerSprite,               // Width/height of target (sprite)
                 spriteSheet,                                   // Source to copy from (spriteSheet)
                 col * widthPerSprite, row * heightPerSprite,   // Origin of area to copy from (spriteSheet)
                 widthPerSprite, heightPerSprite);              // Width of area to copy from (spriteSheet)
    // Scale the image to the correct dimensions
    return sprite.scaled(spriteWidth, spriteHeight);
}
























