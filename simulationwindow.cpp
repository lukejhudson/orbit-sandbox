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
SimulationWindow::SimulationWindow(Simulation *sim, Sprites sprites, double width, double height) {
    m_backingStore = new QBackingStore(this);

    this->sim = sim;
    spawning = false;
    initialMousePos = new Vector();
    mousePos = new Vector();
    scale = 1;
    currentOffsetX = 0;
    currentOffsetY = 0;
    newOffsetX = 0;
    newOffsetY = 0;
    movingCamera = false;

    this->sprites = sprites;

    sim->setWidth(width);
    sim->setHeight(height);
    sim->resetSim();
    sim->setSprites(scale);

    // Start a timer to be used to render at 60 fps
    m_timerId = startTimer(1000/60);
}

/**
 * @brief SimulationWindow::event Handles all the different possible events.
 * @param event The event to handle
 * @return True if the event has been handled
 */
bool SimulationWindow::event(QEvent *event) {
    // Handle update event
    if (event->type() == QEvent::UpdateRequest) {
        // Render window
        renderNow();
        return true;
    }
    // Make sure the rest of events are handled
    return QWindow::event(event);
}

/**
 * @brief RasterWindow::renderLater Don't need to render immediately, so
 * instead request for an update when the system is ready to repaint.
 */
void SimulationWindow::renderLater() {
    requestUpdate();
}

/**
 * @brief RasterWindow::resizeEvent The resize event is guaranteed to
 * be called prior to the window being shown on screen and will also
 * be called whenever the window is resized while on screen. We use this
 * to resize the back buffer, and defer rendering to the corresponding/
 * following expose event.
 * @param resizeEvent The resize event
 */
void SimulationWindow::resizeEvent(QResizeEvent *resizeEvent) {
    m_backingStore->resize(resizeEvent->size());
    sim->setWidth(width());
    sim->setHeight(height());
    // Adjust camera so that the centre remains in the middle of the screen
    double dx = width() - resizeEvent->oldSize().width();
    double dy = height() - resizeEvent->oldSize().height();

    currentOffsetX -= (dx / 2) / scale;
    currentOffsetY -= (dy / 2) / scale;
}

/**
 * @brief RasterWindow::exposeEvent Called after the window is set to be
 * shown (window.show(); in main) to notify us that the window's exposure
 * in the windowing system has changed.
 * (Also called when the window becomes obscured)
 */
void SimulationWindow::exposeEvent(QExposeEvent *) {
    if (isExposed()) // Is the window showing?
        renderNow(); // Yes --> Draw immediately
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

        // Flag to swap spawnType back after the switch statement
        bool spawningPlanetarySystem = false;
        if (spawnType == Body::PlanetarySystem) {
            // Randomly choose type for the central body of the planetary system
            // (star, dwarf star or black hole)
            spawnType = static_cast<Body::BodyType>(2 + std::rand() % 3);
            spawningPlanetarySystem = true;
        }
        // Create the body of the specified type
        newBody = new Body(spawnType);
        newBody->setPos(event->x() / scale + currentOffsetX,
                        event->y() / scale + currentOffsetY);
        // Set the body's sprite based on its type
        switch (spawnType) {
            default:
            case Body::Asteroid:
                newBody->setSprite(sprites.asteroidImage.scaled(static_cast<int>(scale * newBody->getDiameter()),
                                                                static_cast<int>(scale * newBody->getDiameter())));
                break;
            case Body::Planet:
                newBody->setSprite(sprites.getPlanetImage(newBody->getPlanetType()).scaled(static_cast<int>(scale * newBody->getDiameter()),
                                                                                           static_cast<int>(scale * newBody->getDiameter())));
                break;
            case Body::Star:
                newBody->setSprite(sprites.starImage.scaled(static_cast<int>(scale * newBody->getDiameter()),
                                                            static_cast<int>(scale * newBody->getDiameter())));
                break;
            case Body::WhiteDwarf:
                newBody->setSprite(sprites.whitedwarfImage.scaled(static_cast<int>(scale * newBody->getDiameter()),
                                                                  static_cast<int>(scale * newBody->getDiameter())));
                break;
            case Body::BlackHole:
                newBody->setSprite(sprites.blackholeImage.scaled(static_cast<int>(scale * newBody->getDiameter()),
                                                                 static_cast<int>(scale * newBody->getDiameter())));
                break;
        }
        if (spawningPlanetarySystem) {
            spawnType = Body::PlanetarySystem;
        }
        //std::cout << "Spawning: " << newBody->getX() << ", " << newBody->getY() << std::endl;
        // Store location of click
        mousePos.set(event->x() / scale, event->y() / scale);
        spawning = true;
    } else if (event->button() == Qt::MiddleButton) {
        // Adjust window offset so that screen can be moved around
        mousePos.set(event->x() / scale, event->y() / scale);
        initialMousePos.set(event->x() / scale, event->y() / scale);
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
        if (spawning) {
            //std::cout << "Release left click: " << event->x() << ", " << event->y() << std::endl;
            spawning = false;
            double vx = 0.1 * (newBody->getX() - (event->x() / scale) - currentOffsetX);
            double vy = 0.1 * (newBody->getY() - (event->y() / scale) - currentOffsetY);
            newBody->setVel(vx, vy);
            if (spawnType != Body::PlanetarySystem) {
                sim->addBody(newBody);
            } else {
                sim->spawnPlanetarySystem(newBody);
            }
        }
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
        newOffsetX = initialMousePos.getX() - mousePos.getX();
        newOffsetY = initialMousePos.getY() - mousePos.getY();
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
    std::list<Body*> *bodies = sim->getBodies();

    //std::cout << "Drawing bodies " << bodies.size() << std::endl;
    for (std::list<Body*>::iterator iter = bodies->begin(); iter != bodies->end(); ++iter) {
        int bodyDiam = static_cast<int>((*iter)->getDiameter());
        p->drawImage(static_cast<int>(scale * ((*iter)->getX() - (bodyDiam / 2)) -
                                        ((newOffsetX + currentOffsetX) * scale)),
                     static_cast<int>(scale * ((*iter)->getY() - (bodyDiam / 2)) -
                                        ((newOffsetY + currentOffsetY) * scale)),
                     *(*iter)->getSprite());
    }
    // Delete copy of bodies list
    for (std::list<Body*>::iterator iter = bodies->begin(); iter != bodies->end(); ++iter) {
        delete *iter;
    }
    delete bodies;

    // If we are spawning a new body (left click is down), draw it and
    // an arrow to show its direction and give an indication of its velocity
    if (spawning) {
        //std::cout << "Drawing spawn stuff" << std::endl;
        int mouseX = static_cast<int>(mousePos.getX() * scale);
        //std::cout << "mouseX: " << mouseX;
        int mouseY = static_cast<int>(mousePos.getY() * scale);
        //std::cout << ", mouseY: " << mouseY << std::endl;
        int bodyX = static_cast<int>((newBody->getX() - currentOffsetX) * scale);
        //std::cout << "bodyX: " << bodyX;
        int bodyY = static_cast<int>((newBody->getY() - currentOffsetY) * scale);
        //std::cout << ", bodyY: " << bodyY << std::endl;
        //std::cout << "Offset: " << currentOffsetX << ", " << currentOffsetY << std::endl;

        int bodyDiam = static_cast<int>(scale * newBody->getDiameter());
        // Draw the body being spawned
        p->drawImage(static_cast<int>(bodyX - (bodyDiam / 2)),
                     static_cast<int>(bodyY - (bodyDiam / 2)),
                     *newBody->getSprite());

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

    //std::cout << "Render end" << std::endl;
}

void SimulationWindow::renderNow() {
    if (!isExposed())
        return; // Not showing --> Don't bother rendering

    // Area we want to paint
    QRect rect(0, 0, width(), height());
    // Paint area
    m_backingStore->beginPaint(rect);

    // Get the QPaintDevice of the back buffer
    QPaintDevice *device = m_backingStore->paintDevice();
    // Creae a QPainter to render to the paint device
    QPainter painter(device);

    // Draw the gradient background on the window
    painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0));
    render(&painter);
    painter.end();

    // Done rendering
    m_backingStore->endPaint();
    // Present the contents in the back buffer
    m_backingStore->flush(rect);
}

/**
 * @brief SimulationWindow::setSpawnType Sets the type of body
 * that will spawn when the user clicks and drags the mouse.
 * @param type The type of body to spawn (see Body::BodyType)
 */
void SimulationWindow::setSpawnType(Body::BodyType type) {
    spawnType = type;
}
















