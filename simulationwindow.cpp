#include <list>
#include <iostream>
#include <QtWidgets>
#include <QMainWindow>
#include "simulationwindow.h"

/**
 * @brief SimulationWindow::SimulationWindow Creates the simulation
 * window and begins displaying the bodies in the given Simulation.
 * @param sim The simulation to display in the window
 */
SimulationWindow::SimulationWindow(Simulation *sim, Sprites sprites) {
    this->sim = sim;
    setTitle("Orbit Sandbox");
    resize(1000, 500);
    spawning = false;
    initialMousePos = new Vector();
    mousePos = new Vector();
    // Start a timer to be used to render at 60 fps
    scale = 1;
    currentOffsetX = 0;
    currentOffsetY = 0;
    newOffsetX = 0;
    newOffsetY = 0;
    movingCamera = false;

    this->sprites = sprites;

    sim->setSprites(scale);

    m_timerId = startTimer(1000/60);
}

/**
 * @brief SimulationWindow::timerEvent Triggered 60 times per second
 * by the timer to re-render the window.
 * @param event The timer event to check if it is time to render the window
 */
void SimulationWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_timerId)
        renderLater();
}

/**
 * @brief SimulationWindow::mousePressEvent Handles mouse press events that
 * the window receives. Left clicking on the window spawns a new asteroid
 * which will join the simulation.
 * @param event The mouse press event to handle
 */
void SimulationWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // Left click
        //std::cout << "Left click: " << event->x() << ", " << event->y() << std::endl;
        newBody = *new Body(spawnType);
        newBody.setPos(event->x() / scale + currentOffsetX,
                       event->y() / scale + currentOffsetY);
        switch (spawnType) {
            case 0: // Star
                newBody.setSprite(sprites.starImage.scaled(static_cast<int>(newBody.getDiameter() * scale),
                                                           static_cast<int>(newBody.getDiameter() * scale)));
                break;
            case 1: // Asteroid
                newBody.setSprite(sprites.asteroidImage.scaled(static_cast<int>(newBody.getDiameter() * scale),
                                                               static_cast<int>(newBody.getDiameter() * scale)));
                break;
            case 2: // White dwarf
                newBody.setSprite(sprites.whitedwarfImage.scaled(static_cast<int>(newBody.getDiameter() * scale),
                                                                 static_cast<int>(newBody.getDiameter() * scale)));
                break;
            case 3: // Black hole
                newBody.setSprite(sprites.blackholeImage.scaled(static_cast<int>(newBody.getDiameter() * scale),
                                                                static_cast<int>(newBody.getDiameter() * scale)));
                break;
        }
        //std::cout << "Spawning: " << newBody.getX() << ", " << newBody.getY() << std::endl;
        mousePos.setX(event->x() / scale);
        mousePos.setY(event->y() / scale);
        spawning = true;
    } else if (event->button() == Qt::MiddleButton) {
        // Adjust window offset so that screen can be moved around
        mousePos.setX(event->x() / scale);
        mousePos.setY(event->y() / scale);
        initialMousePos.setX(event->x() / scale);
        initialMousePos.setY(event->y() / scale);
        movingCamera = true;
    } else {
        // Pass on other buttons to base class
        QWindow::mousePressEvent(event);
    }
}

/**
 * @brief SimulationWindow::mouseReleaseEvent Handles mouse release events
 * that the window receives. After pressing the left mouse button, releasing
 * it will add the spawned asteroid to the simulation.
 * @param event The mouse release event to handle
 */
void SimulationWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        // Left click
        //std::cout << "Release left click: " << event->x() << ", " << event->y() << std::endl;
        spawning = false;
        newBody.setVelX(0.1 * (newBody.getX() - (event->x() / scale) - currentOffsetX));
        newBody.setVelY(0.1 * (newBody.getY() - (event->y() / scale) - currentOffsetY));
        sim->addBody(newBody);
    } else if (event->button() == Qt::MiddleButton) {
        movingCamera = false;
        currentOffsetX += newOffsetX;
        newOffsetX = 0;
        currentOffsetY += newOffsetY;
        newOffsetY = 0;
    } else {
        // Pass on other buttons to base class
        QWindow::mouseReleaseEvent(event);
    }
}

/**
 * @brief SimulationWindow::mouseMoveEvent Handles mouse move events while
 * spawning an asteroid. Moving the mouse further from the left click
 * location gives the new asteroid a greater starting velocity.
 * @param event The mouse move event to handle
 */
void SimulationWindow::mouseMoveEvent(QMouseEvent *event) {
    if (spawning) {
        //std::cout << "Mouse move: " << event->x() << ", " << event->y() << std::endl;
        mousePos.setX(event->x() / scale);
        mousePos.setY(event->y() / scale);
    } else if (movingCamera) {
        mousePos.setX(event->x() / scale);
        mousePos.setY(event->y() / scale);
        newOffsetX = static_cast<int>(initialMousePos.getX() - mousePos.getX());
        newOffsetY = static_cast<int>(initialMousePos.getY() - mousePos.getY());
    } else {
        // Pass on other buttons to base class
        QWindow::mouseMoveEvent(event);
    }
}

/**
 * @brief SimulationWindow::wheelEvent Handles mouse wheel scroll events.
 * Scrolling down will zoom the simulation out, while scrolling up will
 * zoom in on the simulation.
 * @param event The wheel event to handle
 */
void SimulationWindow::wheelEvent(QWheelEvent *event) {
    //std::cout << "delta: " << event->delta() << std::endl;
    if (event->delta() > 0) {
        scale += scale * 0.1;
        currentOffsetX += (0.05 * width()) / scale;
        currentOffsetY += (0.05 * height()) / scale;
    } else {
        scale -= scale * 0.1;
        currentOffsetX -= (0.05 * width()) / scale;
        currentOffsetY -= (0.05 * height()) / scale;
    }
    sim->setSprites(scale);
}

/**
 * @brief SimulationWindow::render Renders the window.
 * @param p The painter object used to draw on the window
 */
void SimulationWindow::render(QPainter *p) {
    //std::cout << "Getting bodies" << std::endl;
    std::list<Body> bodies = sim->getBodies();
    QColor starColour(234, 200, 46);
    // QColor asteroidColour(81, 21, 13);
    QColor asteroidColour(76, 243, 255);
    p->setPen(Qt::NoPen); // No outline

    //std::cout << "Drawing bodies" << std::endl;
    for (std::list<Body>::iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
        int bodyDiam = static_cast<int>(iter->getDiameter());
        p->drawImage(static_cast<int>(scale * (iter->getX() - (bodyDiam / 2)) -
                                        ((newOffsetX + currentOffsetX) * scale)),
                     static_cast<int>(scale * (iter->getY() - (bodyDiam / 2)) -
                                        ((newOffsetY + currentOffsetY) * scale)),
                     *iter->getSprite());
    }

    // If we are spawning a new body (left click is down), draw it and
    // an arrow to show its direction and give an indication of its velocity
    if (spawning) {
        //std::cout << "Drawing spawn stuff" << std::endl;
        int mouseX = static_cast<int>(mousePos.getX() * scale);
        //std::cout << "mouseX: " << mouseX;
        int mouseY = static_cast<int>(mousePos.getY() * scale);
        //std::cout << ", mouseY: " << mouseY << std::endl;
        int bodyX = static_cast<int>((newBody.getX() - currentOffsetX) * scale);
        //std::cout << "bodyX: " << bodyX;
        int bodyY = static_cast<int>((newBody.getY() - currentOffsetY) * scale);
        //std::cout << ", bodyY: " << bodyY << std::endl;
        //std::cout << "Offset: " << currentOffsetX << ", " << currentOffsetY << std::endl;

        int bodyDiam = static_cast<int>(scale * newBody.getDiameter());
        // Draw the body being spawned
        p->drawImage(static_cast<int>(bodyX - (bodyDiam / 2)),
                     static_cast<int>(bodyY - (bodyDiam / 2)),
                     *newBody.getSprite());

        p->setPen(QColor(255, 255, 255));
        // Line from mouse to new asteroid
        p->drawLine(mouseX, mouseY, bodyX, bodyY);
        // Line from asteroid forwards
        int forwardX = bodyX + (bodyX - mouseX);
        int forwardY = bodyY + (bodyY - mouseY);
        p->drawLine(bodyX, bodyY, forwardX, forwardY);
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
        p->drawLine(forwardX, forwardY, arrow1X, arrow1Y);
        p->drawLine(forwardX, forwardY, arrow2X, arrow2Y);
    }
}

/**
 * @brief SimulationWindow::setSpawnType Sets the type of body
 * that will spawn when the user clicks and drags the mouse.
 * @param type The type of body to spawn
 */
void SimulationWindow::setSpawnType(int type) {
    spawnType = type;
}


















