#include <QtWidgets/QApplication>
#include "rasterwindow.h"
#include "analogclockwindow.h"
#include "simulationwindow.h"
#include "sprites.h"
#include "simulation.h"
#include "mainwindow.h"

int main(int argc, char **argv) {
    /*
     * The entry point for a QWindow based application is the QGuiApplication class.
     * It manages the GUI application's control flow and main settings.
     * We pass the command line arguments which can be used to pick up certain system wide options.
     */
    QApplication app(argc, argv);

    Sprites sprites;

    Simulation sim(sprites);

    SimulationWindow *simwin = new SimulationWindow(&sim, sprites);
    QWidget *container = QWidget::createWindowContainer(simwin);

    // RasterWindow window; // Initialise class
    //AnalogClockWindow window;
    //SimulationWindow *window = new SimulationWindow(&sim, sprites);
    MainWindow *window = new MainWindow(&sim, sprites, simwin, container);
    window->show(); // Make window visible

    // Enter application's event loop so the application can run
    return app.exec();
}
