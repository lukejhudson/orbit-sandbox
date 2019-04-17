#include <list>
#include <iostream>
#include <QtWidgets>
#include <QMainWindow>
#include "simulationwidget.h"

/**
 * @brief SimulationWidget::SimulationWidget Creates the simulation
 * widget and begins displaying the bodies in the given Simulation.
 * @param sim The simulation to display in the widget
 */
SimulationWidget::SimulationWidget(Simulation *sim, Sprites sprites) {
    this->sim = sim;
    initialMousePos = new QPointF();
    mousePos = new QPointF();
    currentOffset = new QPointF(0, 0);
    backgroundOffset = new QPointF(0, 0);
    newOffset = new QPointF(0, 0);
    totalBackgrounds = new QPointF(0, 0);

    this->sprites = sprites;

    updateSimVisibleRegion();
    sim->resetSim();

    // Start a timer to be used to render at 60 fps
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000/60);
}

/**
 * @brief SimulationWidget::~SimulationWidget Destructor.
 */
SimulationWidget::~SimulationWidget() {
    delete initialMousePos;
    delete mousePos;
    delete currentOffset;
    delete backgroundOffset;
    delete newOffset;
    delete totalBackgrounds;
    delete timer;
}

/**
 * @brief SimulationWidget::resizeEvent Handles resizing of the widget.
 * The resize event is guaranteed to be called prior to the widget
 * being shown on screen and will also be called whenever the widget
 * is resized while on screen.
 * @param resizeEvent The resize event
 */
void SimulationWidget::resizeEvent(QResizeEvent *resizeEvent) {
    // Update the visible region of the simulation
    updateSimVisibleRegion();
    // Adjust camera so that the centre remains in the middle of the screen
    double dx = width() - resizeEvent->oldSize().width();
    double dy = height() - resizeEvent->oldSize().height();

    *currentOffset -= QPointF((dx / 2) / scale, (dy / 2) / scale);
}

/**
 * @brief SimulationWidget::mousePressEvent Handles mouse press events that
 * the widget receives. Left clicking on the widget spawns a new asteroid
 * which will join the simulation.
 * @param event The mouse press event to handle
 */
void SimulationWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && sim->getMode() == Simulation::Sandbox) {
        // Left click
        //std::cout << "Left click: " << event->x() << ", " << event->y() << std::endl;

        // Create the new body
        if (spawnType == Body::PlanetarySystem) {
            // Randomly choose type for the central body of the planetary system
            // (star, dwarf star or black hole)
            newBody = new Body(static_cast<Body::BodyType>(2 + std::rand() % 3));
        } else {
            newBody = new Body(spawnType);
        }
        // Set new body's position
        newBody->setPos(event->x() / scale + currentOffset->x(),
                        event->y() / scale + currentOffset->y());
        // Store location of click
        mousePos->setX(event->x() / scale);
        mousePos->setY(event->y() / scale);
        spawning = true;
    } else if ((event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) && sim->getMode() == Simulation::Sandbox) {
        // Adjust window offset so that the simulation screen can be moved around
        // when the middle mouse button is held
        mousePos->setX(event->x() / scale);
        mousePos->setY(event->y() / scale);
        initialMousePos->setX(event->x() / scale);
        initialMousePos->setY(event->y() / scale);
        movingCamera = true;
    } else {
        // Pass on other buttons to base class
        QWidget::mousePressEvent(event);
    }
}

/**
 * @brief SimulationWidget::mouseReleaseEvent Handles mouse release events
 * that the widget receives. After pressing the left mouse button, releasing
 * it will add the spawned asteroid to the simulation.
 * @param event The mouse release event to handle
 */
void SimulationWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && sim->getMode() == Simulation::Sandbox) {
        // Left click
        if (spawning) {
            //std::cout << "Release left click: " << event->x() << ", " << event->y() << std::endl;
            spawning = false;
            double vx = 0.05 * (newBody->getX() - (event->x() / scale) - currentOffset->x());
            double vy = 0.05 * (newBody->getY() - (event->y() / scale) - currentOffset->y());
            newBody->setVel(vx, vy);
            if (spawnType != Body::PlanetarySystem) {
                sim->addBody(newBody);
            } else {
                sim->spawnPlanetarySystem(newBody, false);
            }
        }
    } else if ((event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) && sim->getMode() == Simulation::Sandbox) {
        movingCamera = false;
        *currentOffset += *newOffset;
        if (scale > 1) {
            // Zoomed in --> BG less affected by camera movement
            *backgroundOffset -= QPointF(-newOffset->x() / scale, -newOffset->y() / scale);
        } else {
            // Zoomed out --> BG very slightly less affected by camera movement
            *backgroundOffset -= QPointF(-newOffset->x() * 0.9 * scale, -newOffset->y() * 0.9 * scale);
        }
        newOffset->setX(0);
        newOffset->setY(0);
    } else {
        // Pass on other buttons to base class
        QWidget::mouseReleaseEvent(event);
    }
}

/**
 * @brief SimulationWidget::mouseMoveEvent Handles mouse move events while
 * spawning an asteroid. Moving the mouse further from the left click
 * location gives the new asteroid a greater starting velocity.
 * @param event The mouse move event to handle
 */
void SimulationWidget::mouseMoveEvent(QMouseEvent *event) {
    if (spawning) {
        //std::cout << "Mouse move: " << event->x() << ", " << event->y() << std::endl;
        mousePos->setX(event->x() / scale);
        mousePos->setY(event->y() / scale);
    } else if (movingCamera) {
        mousePos->setX(event->x() / scale);
        mousePos->setY(event->y() / scale);
        newOffset->setX(initialMousePos->x() - mousePos->x());
        newOffset->setY(initialMousePos->y() - mousePos->y());
    } else {
        // Pass on other buttons to base class
        QWidget::mouseMoveEvent(event);
    }
}

/**
 * @brief SimulationWidget::wheelEvent Handles mouse wheel scroll events.
 * Scrolling down will zoom the simulation out, while scrolling up will
 * zoom in on the simulation.
 * @param event The wheel event to handle
 */
void SimulationWidget::wheelEvent(QWheelEvent *event) {
    //std::cout << "delta: " << event->delta() << std::endl;
    if (sim->getMode() != Simulation::Background) {
        // Calculate size of background images before the resize
        double reducedScale = (scale + 4) / 5;
        double backgroundWidthBefore = sprites.backgroundImage.width() * reducedScale;
        double backgroundHeightBefore = sprites.backgroundImage.height() * reducedScale;
        double backgroundWidthAfter, backgroundHeightAfter;
        //std::cout << "Before: " << scale;
        if (event->delta() > 0) {
            // Zoom in
            if (scale < 50) {
                scale *= 1.25;
                *currentOffset += QPointF((0.125 * width()) / scale, (0.125 * height()) / scale);
            }
        } else {
            // Zoom out
            if (scale > 0.2) {
                scale *= 0.8;
                *currentOffset -= QPointF((0.1 * width()) / scale, (0.1 * height()) / scale);
            }
        }
        //std::cout << ", After: " << scale << std::endl;
        // Calculate size of background images after the resize
        reducedScale = (scale + 4) / 5;
        backgroundWidthAfter = sprites.backgroundImage.width() * reducedScale;
        backgroundHeightAfter = sprites.backgroundImage.height() * reducedScale;
        // Calculate the number of background images we would have to travel
        // back over to reach the origin
        totalBackgrounds->setX(backgroundOffset->x() / backgroundWidthBefore);
        totalBackgrounds->setY(backgroundOffset->y() / backgroundHeightBefore);
        // Calculate how much we need to adjust the offset so that we zoom
        // in on the centre of the screen
        *backgroundOffset += QPointF((backgroundWidthAfter - backgroundWidthBefore) * totalBackgrounds->x(),
                                    (backgroundHeightAfter - backgroundHeightBefore) * totalBackgrounds->y());

        // Update the visible region of the simulation
        updateSimVisibleRegion();
    }
}

/**
 * @brief SimulationWidget::keyPressEvent Handles events generated by
 * keyboard key presses.
 * @param event The key press event to handle
 */
void SimulationWidget::keyPressEvent(QKeyEvent *event) {
    //std::cout << "simw Key pressed " << event->key() << std::endl;
    if (event->key() == Qt::Key_W) {
        // W pressed --> Turn rocket engines on
        sim->getRocket()->setFiring(true);
    } else if (event->key() == Qt::Key_A) {
        // A pressed --> Rotate left
        sim->getRocket()->setRotatingAntiCW(true);
    } else if (event->key() == Qt::Key_D) {
        // D pressed --> Rotate right
        sim->getRocket()->setRotatingCW(true);
    }
}

/**
 * @brief SimulationWidget::keyReleaseEvent Handles events generated
 * by keyboard key releases.
 * @param event The key release event to handle
 */
void SimulationWidget::keyReleaseEvent(QKeyEvent *event) {
    //std::cout << "simw Key released " << event->key() << std::endl;
    if (event->key() == Qt::Key_W) {
        // W released --> Turn rocket engines on
        sim->getRocket()->setFiring(false);
    } else if (event->key() == Qt::Key_A) {
        // A released --> Stop rotating left
        sim->getRocket()->setRotatingAntiCW(false);
    } else if (event->key() == Qt::Key_D) {
        // D released --> Stop rotating right
        sim->getRocket()->setRotatingCW(false);
    }
}

/**
 * @brief SimulationWidget::paintEvent Renders the widget.
 */
void SimulationWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // Update the visible region of the simulation
    updateSimVisibleRegion();

    Rocket *rocket = nullptr;
    // Take a snapshot of the rocket, so position, velocity etc stay
    // constant during the painting
    // Any changes that need to be made (e.g. explosion animation)
    // will be applied to the original rocket
    Rocket *rocketCopy = nullptr;

    // A reduced version of the current scale to use with the background
    // --> Background isn't affected as much --> Try to give some parallax
    double reducedScale = (scale + 4) / 5;

    if (sim->getMode() == Simulation::Exploration) {
        rocket = sim->getRocket();
        rocketCopy = rocket->copy();
        // Adjust camera so that the rocket is in the centre of the screen
        currentOffset->setX((-width() / 2.0 / scale) + rocketCopy->getX());
        currentOffset->setY((-height() / 2.0 / scale) + rocketCopy->getY());
        // Adjust background position based on the movement of the rocket
        // Want to increase scale at low numbers, decrease at high numbers
        // to give a solid parallax effect
        double scaleFactor = (scale * 20 + 1) / (scale + 50);
        *backgroundOffset += QPointF(rocketCopy->getVelX() * scaleFactor,
                                     rocketCopy->getVelY() * scaleFactor);
    }

    std::list<Body*> *bodies = sim->getBodies();

    // Draw black background
    p.fillRect(0, 0, width(), height(), QColor(0, 0, 0));
    // Draw background image, looped and to the correct scale
    // Adjust size of the background image
    double backgroundWidth = sprites.backgroundImage.width() * reducedScale;
    double backgroundHeight = sprites.backgroundImage.height() * reducedScale;
    // Calculate number of background images needed to cover the screen
    int numBackgroundsX = static_cast<int>((width()) / backgroundWidth) + 3;
    int numBackgroundsY = static_cast<int>((height()) / backgroundHeight) + 3;

    double backgroundX, backgroundY;
    if (scale > 1) {
        // Zoomed in --> BG less affected by camera movement
            backgroundX = (-newOffset->x() / scale - backgroundOffset->x()) + width() / 2.0;
            backgroundY = (-newOffset->y() / scale - backgroundOffset->y()) + height() / 2.0;
    } else {
        // Zoomed out --> BG very slightly less affected by camera movement
            backgroundX = (-newOffset->x() * 0.9 * scale - backgroundOffset->x()) + width() / 2.0;
            backgroundY = (-newOffset->y() * 0.9 * scale - backgroundOffset->y()) + height() / 2.0;
    }
    // Calculate coordinates of first (top left) background to be drawn
    double firstX = fmod(backgroundX, backgroundWidth);
    double firstY = fmod(backgroundY, backgroundHeight);
    // Draw all background images
    for (int i = 0; i < numBackgroundsX; i++) {
        for (int j = 0; j < numBackgroundsY; j++) {
            p.drawPixmap(static_cast<int>(firstX) + (i - 1) * static_cast<int>(backgroundWidth),
                         static_cast<int>(firstY) + (j - 1) * static_cast<int>(backgroundHeight),
                         static_cast<int>(backgroundWidth),
                         static_cast<int>(backgroundHeight),
                         sprites.backgroundImage);
        }
    }

    for (std::list<Body*>::iterator iter = bodies->begin(), end = bodies->end(); iter != end; ++iter) {
        double bodyDiam = (*iter)->getDiameter();
        if ((*iter)->getType() == Body::PlayerRocket && sim->getMode() == Simulation::Exploration) {
            // If drawing the player controlled rocket
            // Save state of the painter so we can undo translations and rotations
            p.save();
            // Move the painter to the coordinates of the rocket
            // (We want the centre of any rotation to be the centre of the rocket)
            p.translate(scale * rocketCopy->getX() -
                            ((newOffset->x() + currentOffset->x()) * scale),
                        scale * rocketCopy->getY() -
                            ((newOffset->y() + currentOffset->y()) * scale));

            if (rocketCopy->isExploding()) {
                // If the the rocket has collided with another body and is now exploding
                if (rocketCopy->getExplodingCount() < 64) {
                    // Draw the explosion animation (slowed down 4x)
                    p.drawPixmap(static_cast<int>(-bodyDiam * scale),
                                 static_cast<int>(-bodyDiam * scale),
                                 sprites.getSpriteSheetImage(sprites.rocketExplosionSpriteSheet,
                                                             4, 4, rocketCopy->getExplodingCount() / 4,
                                                             static_cast<int>(2 * bodyDiam * scale),
                                                             static_cast<int>(2 * bodyDiam * scale)));
                    // Advance to next frame
                    rocket->incrementExplodingCount();
                } else {
                    // Explosion animation has finished, don't draw anything
                    // GAME OVER
                    if (!gameOver) {
                        // Send out a signal that a game over state has occurred
                        gameOverSignal();
                        gameOver = true;
                    }
                }
            } else {
                // Draw rocket normally
                // Rotate the painter so we can draw the rocket at the correct angle
                p.rotate(rocketCopy->getAngle());
                // Choose the correct sprite based on whether or not the rocket is firing
                QPixmap s;
                if (rocketCopy->isFiring()) {
                    s = sprites.rocketFiringImage;
                } else {
                    s = sprites.rocketIdleImage;
                }
                // Resize sprite
                s = s.scaled(static_cast<int>(rocketCopy->getDiameter() * scale),
                             static_cast<int>(rocketCopy->getDiameter() * scale));
                // Draw sprite
                p.drawPixmap(static_cast<int>((-bodyDiam / 2.0) * scale),
                             static_cast<int>((-bodyDiam / 2.0) * scale),
                             s);
            }
            // Restore the previous state of the painter
            p.restore();
        } else {
            // Drawing any other body
            p.drawPixmap(static_cast<int>(scale * ((*iter)->getX() - (bodyDiam / 2.0)) -
                                           ((newOffset->x() + currentOffset->x()) * scale)),
                         static_cast<int>(scale * ((*iter)->getY() - (bodyDiam / 2.0)) -
                                           ((newOffset->y() + currentOffset->y()) * scale)),
                         static_cast<int>((*iter)->getDiameter() * scale),
                         static_cast<int>((*iter)->getDiameter() * scale),
                         sprites.getImage(*iter));
        }
    }

    // Draw rocket angle, direction and speed
    if (sim->getMode() == Simulation::Exploration) {
        QPoint panelSize(100, 100); // Size of the panel
        QPoint rocketSize(panelSize.x() - 10, panelSize.y() - 10); // Size of rocket
        QPoint coords(5, height() - panelSize.y() - 5); // Top left coords of the panel
        // Draw grey background
        p.fillRect(coords.x(), coords.y(), panelSize.x(), panelSize.y(), QColor(50, 50, 50));
        // Draw rocket speed
        QString speedText = QString::number(rocketCopy->getVel()->getNormal(), 'g', 4);
        p.drawText(coords, QString("Speed: ") + speedText);
        // Save painter state
        p.save();
        // Position painter at the centre of the panel location
        p.translate(coords.x() + panelSize.x() / 2, coords.y() + panelSize.y() / 2);
        // Rotate painter to rocket's angle
        p.rotate(rocketCopy->getAngle());
        // Get correct sprite
        QPixmap s;
        if (rocketCopy->isFiring()) {
            s = sprites.rocketFiringImage;
        } else {
            s = sprites.rocketIdleImage;
        }
        // Resize sprite
        s = s.scaled(rocketSize.x(), rocketSize.y());
        // Draw sprite
        p.drawPixmap(-rocketSize / 2, s);
        // Restore painter state
        p.restore();
        // Draw velocity direction arrow (similarly to rocket)
        double angle = atan(rocketCopy->getVelY() / rocketCopy->getVelX()) * (180.0 / M_PI);
        if (rocketCopy->getVelX() < 0) angle += 180;
        p.save();
        p.translate(coords.x() + panelSize.x() / 2, coords.y() + panelSize.y() / 2);
        p.rotate(angle);
        s = sprites.arrowIcon.scaled(rocketSize.x(), rocketSize.y());
        p.drawPixmap(-rocketSize / 2, s);
        p.restore();
    }

    // Delete copy of bodies list
    for (std::list<Body*>::iterator iter = bodies->begin(), end = bodies->end(); iter != end; ++iter) {
        delete *iter;
    }
    delete bodies;
    delete rocketCopy;

    // Draw the map showing where the rocket has explored
//    QImage *map = sim->getMap();
//    p.drawImage(QRect(0, height() - 250, 250, 250), *map);

    // If we are spawning a new body (left click is down), draw it and
    // an arrow to show its direction and give an indication of its velocity
    if (spawning) {
        int mouseX = static_cast<int>(mousePos->x() * scale);
        int mouseY = static_cast<int>(mousePos->y() * scale);
        int bodyX = static_cast<int>((newBody->getX() - currentOffset->x()) * scale);
        int bodyY = static_cast<int>((newBody->getY() - currentOffset->y()) * scale);

        int bodyDiam = static_cast<int>(scale * newBody->getDiameter());
        // Draw the body being spawned
        p.drawPixmap(static_cast<int>(bodyX - (bodyDiam / 2.0)),
                     static_cast<int>(bodyY - (bodyDiam / 2.0)),
                     static_cast<int>(newBody->getDiameter() * scale),
                     static_cast<int>(newBody->getDiameter() * scale),
                     sprites.getImage(newBody));

        p.setPen(QColor(255, 255, 255));
        // Line from mouse to new asteroid
        p.drawLine(mouseX, mouseY, bodyX, bodyY);
        // Line from asteroid forwards
        int forwardX = bodyX + (bodyX - mouseX);
        int forwardY = bodyY + (bodyY - mouseY);
        p.drawLine(bodyX, bodyY, forwardX, forwardY);
        // Draw arrow head
        // Vector from "back" of arrow head (on the line) to tip of the arrow
        int backOfArrowHeadToFrontX = static_cast<int>(0.2 * (forwardX - bodyX));
        int backOfArrowHeadToFrontY = static_cast<int>(0.2 * (forwardY - bodyY));
        // Coordinate of the back of the arrow head
        int backOfArrowHeadX = forwardX - backOfArrowHeadToFrontX;
        int backOfArrowHeadY = forwardY - backOfArrowHeadToFrontY;
        // First arm of the arrow
        int arrow1X = backOfArrowHeadX - backOfArrowHeadToFrontY;
        int arrow1Y = backOfArrowHeadY + backOfArrowHeadToFrontX;
        // Second arm of the arrow
        int arrow2X = backOfArrowHeadX + backOfArrowHeadToFrontY;
        int arrow2Y = backOfArrowHeadY - backOfArrowHeadToFrontX;
        // Draw arrow arms
        p.drawLine(forwardX, forwardY, arrow1X, arrow1Y);
        p.drawLine(forwardX, forwardY, arrow2X, arrow2Y);
    }
}

/**
 * @brief SimulationWidget::setSpawnType Sets the type of body
 * that will spawn when the user clicks and drags the mouse.
 * @param type The type of body to spawn (see Body::BodyType)
 */
void SimulationWidget::setSpawnType(Body::BodyType type) {
    spawnType = type;
}

/**
 * @brief SimulationWidget::updateSimVisibleRegion Updates the
 * visible region of the Simulation.
 */
void SimulationWidget::updateSimVisibleRegion() {
    sim->setVisibleRegion(currentOffset->x() + newOffset->x(),
                          currentOffset->y() + newOffset->y(),
                          width() / scale,
                          height() / scale,
                          scale);
}














