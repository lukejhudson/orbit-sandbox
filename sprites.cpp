#include <iostream>
#include "sprites.h"

Sprites::Sprites() {
    char starPath[] = "/sprites/star.png";
    starImage = loadImage(starPath);

    char whitedwarfPath[] = "/sprites/whitedwarf.png";
    whitedwarfImage = loadImage(whitedwarfPath);

    char blackholePath[] = "/sprites/blackhole.png";
    blackholeImage = loadImage(blackholePath);

    char asteroidPath[] = "/sprites/asteroid.png";
    asteroidImage = loadImage(asteroidPath);

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




























