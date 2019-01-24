#include <list>
#include <iostream>
#include "simulationwindow.h"

/**
 * @brief SimulationWindow::SimulationWindow Creates the simulation
 * window and begins displaying the bodies in the given Simulation.
 * @param sim The simulation to display in the window
 */
SimulationWindow::SimulationWindow(Simulation *sim) {
    this->sim = sim;
    setTitle("Orbit Sandbox");
    resize(500, 500);
    spawning = false;
    mousePos = new Vector();
    // Start a timer to be used to render at 60 fps
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
        newBody = *new Body(5, 5, new Vector(event->x(), event->y()), new Vector(), 1);
        spawning = true;
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
        newBody.setVel(new Vector(0.1 * (newBody.getX() - event->x()), 0.1 * (newBody.getY() - event->y())));
        sim->addBody(newBody);
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
        mousePos.setX(event->x());
        mousePos.setY(event->y());
    } else {
        // Pass on other buttons to base class
        QWindow::mouseMoveEvent(event);
    }
}

/**
 * @brief SimulationWindow::render Renders the window.
 * @param p The painter object used to draw on the window
 */
void SimulationWindow::render(QPainter *p) {
    //std::cout << "Getting bodies" << std::endl;
    std::list<Body> bodies = sim->getBodies();
    QColor starColour(234, 200, 46);
    QColor asteroidColour(81, 21, 13);
    p->setPen(Qt::NoPen); // No outline

    //std::cout << "Drawing bodies" << std::endl;
    for (std::list<Body>::iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
        switch (iter->getType()) {
            case 0:
                p->setBrush(starColour);
                break;
            case 1:
                p->setBrush(asteroidColour);
                break;
        }
        int bodyDiam = static_cast<int>(iter->getDiameter());
        p->drawEllipse(static_cast<int>(iter->getX() - (bodyDiam / 2)), static_cast<int>(iter->getY() - (bodyDiam / 2)),
                       bodyDiam, bodyDiam);
    }

    // If we are spawning a new body (left click is down), draw it and
    // an arrow to show its direction and give an indication of its velocity
    if (spawning) {
        //std::cout << "Drawing spawn stuff" << std::endl;
        int mouseX = static_cast<int>(mousePos.getX());
        int mouseY = static_cast<int>(mousePos.getY());
        int bodyX = static_cast<int>(newBody.getX());
        int bodyY = static_cast<int>(newBody.getY());

        int bodyDiam = static_cast<int>(newBody.getDiameter());
        // Draw the body being spawned
        p->drawEllipse(static_cast<int>(bodyX - (bodyDiam / 2)), static_cast<int>(bodyY - (bodyDiam / 2)),
                       bodyDiam, bodyDiam);

        QColor black(0, 0, 0);
        p->setBrush(black);
        p->setPen(Qt::PenStyle::SolidLine);
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




















