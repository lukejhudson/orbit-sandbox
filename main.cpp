#include <QtWidgets/QApplication>
#include "rasterwindow.h"
#include "analogclockwindow.h"
#include "simulationwidget.h"
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

    // RasterWindow window; // Initialise class
    //AnalogClockWindow window;
    MainWindow *window = new MainWindow();
    window->show(); // Make window visible

    // Enter application's event loop so the application can run
    return app.exec();
}
