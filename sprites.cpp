#include <iostream>
#include "sprites.h"

Sprites::Sprites() {
    // Load all images
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
}

/**
 * @brief Sprites::loadImage Reads and returns the image file from the given path.
 * @param path The path from the executable to the image
 * @return The requested image as a QImage
 */
QImage Sprites::loadImage(char path[]) {
    QImageReader reader(QDir::currentPath() + path);
    reader.setAutoTransform(true);
    // Read image from file
    QImage img = reader.read();
    if (img.isNull()) {
        std::cout << "Could not load sprite: " << reader.errorString().toStdString() << std::endl;
    }
    return img;
}

/**
 * @brief Sprites::getPlanetImage Returns the planet image corresponding to the given planet type.
 * @param t The planet type
 * @return The planet image corresponding to the given planet type
 */
QImage Sprites::getPlanetImage(int t) {
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


























