#include "rasterwindow.h"
#include "analogclockwindow.h"
#include "simulationwindow.h"
#include "simulation.h"

int main(int argc, char **argv) {
    /*
     * The entry point for a QWindow based application is the QGuiApplication class.
     * It manages the GUI application's control flow and main settings.
     * We pass the command line arguments which can be used to pick up certain system wide options.
     */
    QGuiApplication app(argc, argv);
    Simulation sim;

    // RasterWindow window; // Initialise class
    //AnalogClockWindow window;
    SimulationWindow *window = new SimulationWindow(&sim);
    window->show(); // Make window visible

    // Enter application's event loop so the application can run
    return app.exec();
}
