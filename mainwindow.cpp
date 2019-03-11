#include <iostream>
#include "mainwindow.h"

MainWindow::MainWindow() {
    sprites = new Sprites();
    setWindowTitle("Orbit Sandbox");
    resize(1000, 600);
    createMainMenu();
    sim = new Simulation(*sprites);
}

/**
 * @brief MainWindow::createMainMenu Creates the main menu screen and
 * its components and puts them on the screen.
 */
void MainWindow::createMainMenu() {
    // Container widget for whole main menu
    mmContainer = new QWidget;
    mmContainer->setAutoFillBackground(true);
    mmContainer->setPalette(QPalette(QColor(50,50,50)));
    // Add contents vertically
    mmvLayout = new QVBoxLayout(mmContainer);

    // Puts the main menu onto the screen
    setCentralWidget(mmContainer);

    // Main menu title
    title = new QLabel("<p style=\"font-family:Sans Serif; font-size:70pt;\"><b>Orbit Sandbox</b></p>");
    mmvLayout->addWidget(title, 0, Qt::AlignCenter);
    // Credits
    credits = new QLabel("<h3>By Luke Hudson</h3>");
    mmvLayout->addWidget(credits, 0, Qt::AlignCenter);
    // Button to enter sandbox mode
    sandboxModeButton = new QPushButton("Sandbox Mode", mmContainer);
    QFont f("Fantasy", 15, QFont::Bold);
    sandboxModeButton->setFont(f);
    sandboxModeButton->setStyleSheet("QPushButton {background-color: #4f5154; color: white;}");
    sandboxModeButton->setFixedWidth(200);
    connect(sandboxModeButton, &QPushButton::clicked, this, [=]{
        createSandboxMode();
        clearMainMenu();
    });
    mmvLayout->addWidget(sandboxModeButton, 0, Qt::AlignCenter);

    // Button to enter controls screen
    controlsScreenButton = new QPushButton("Controls", mmContainer);
    controlsScreenButton->setFont(f);
    controlsScreenButton->setStyleSheet("QPushButton {background-color: #4f5154; color: white;}");
    controlsScreenButton->setFixedWidth(200);
    connect(controlsScreenButton, &QPushButton::clicked, this, [=]{
        createControlsScreen();
        clearMainMenu();
    });
    mmvLayout->addWidget(controlsScreenButton, 0, Qt::AlignCenter);

    // Push components together at the top of the screen
    mmvLayout->addStretch();
    // Add some spacing between each component
    mmvLayout->setSpacing(50);
}

/**
 * @brief MainWindow::clearMainMenu Deletes the main menu components.
 */
void MainWindow::clearMainMenu() {
    delete mmContainer;
}

/**
 * @brief MainWindow::createSandboxMode Creates the sandbox game mode and
 * its components and puts them on the screen.
 */
void MainWindow::createSandboxMode() {
    simwin = new SimulationWindow(sim, *sprites, width() - 50, height());

    // SimulationWindow converted to a QWidget to place on the main window
    QWidget *simContainer = QWidget::createWindowContainer(simwin);
    // Container for everything on the screen, including the simulation window and the control bar
    sbContainer = new QWidget;
    sbContainer->setAutoFillBackground(true);
    sbContainer->setPalette(QPalette(QColor(50,50,50)));

    setCentralWidget(sbContainer);

    sbhLayout = new QHBoxLayout(sbContainer);
    sbhLayout->addWidget(simContainer, 0);
    sbhLayout->setMargin(5); // Border around all components
    sbvLayout = new QVBoxLayout();
    sbhLayout->addLayout(sbvLayout);

    // Title for body selector menu
    spawnLabel = new QLabel("<b>Select Body to Spawn</b>");
    spawnLabel->setWordWrap(true);
    spawnLabel->setAlignment(Qt::AlignCenter);
    spawnLabel->setFixedWidth(50);
    sbvLayout->addWidget(spawnLabel, 0, Qt::AlignTop);

    /* ASTEROID SELECTOR */
    // Widget containing both button and label
    asteroidContainer = new QWidget;
    asteroidContainer->setFixedWidth(50);
    asteroidContainer->setToolTip("Select to spawn an asteroid:\nVery small, "
                                  "very low mass, very low gravitational force");
    vAsteroidLayout = new QVBoxLayout(asteroidContainer);
    vAsteroidLayout->setMargin(0); // No border
    // Asteroid button
    asteroidButton = new QPushButton(sbContainer);
    asteroidButton->setIcon(QIcon(QDir::currentPath() + "/sprites/asteroid.png"));
    asteroidButton->setIconSize(QSize(40, 40));
    asteroidButton->setAutoFillBackground(true);
    asteroidButton->setPalette(QPalette(QColor(255,255,255)));
    asteroidButton->setMinimumHeight(50);
    asteroidButton->setCheckable(true);
    asteroidButton->setChecked(true);
    // What happens when we press the button
    connect(asteroidButton, &QPushButton::clicked, this, [=] {setSpawnType(Body::Asteroid);});
    // Asteroid label
    asteroidLabel = new QLabel("Asteroid");
    asteroidLabel->setAlignment(Qt::AlignCenter);
    // Combine button and label
    vAsteroidLayout->addWidget(asteroidButton);
    vAsteroidLayout->addWidget(asteroidLabel);
    // Add both button and label to the menu
    sbvLayout->addWidget(asteroidContainer, 0, Qt::AlignTop);

    /* PLANET SELECTOR */
    // Widget containing both button and label
    planetContainer = new QWidget;
    planetContainer->setFixedWidth(50);
    planetContainer->setToolTip("Select to spawn a planet:\nSmall, "
                                "low mass, low gravitational force");
    vPlanetLayout = new QVBoxLayout(planetContainer);
    vPlanetLayout->setMargin(0); // No border
    // Planet button
    planetButton = new QPushButton(sbContainer);
    planetButton->setAutoFillBackground(true);
    planetButton->setPalette(QPalette(QColor(255,255,255)));
    planetButton->setMinimumHeight(50);
    planetButton->setCheckable(true);
    // Thread to cycle the icon between the 5 possible planet sprites
    iconCycleRunning = true;
    std::thread iconThread(&MainWindow::cyclePlanetIcons, this);
    iconThread.detach();
    // What happens when we press the button
    connect(planetButton, &QPushButton::clicked, this, [=] {setSpawnType(Body::Planet);});
    // Planet label
    planetLabel = new QLabel("Planet");
    planetLabel->setAlignment(Qt::AlignCenter);
    // Combine button and label
    vPlanetLayout->addWidget(planetButton);
    vPlanetLayout->addWidget(planetLabel);
    // Add both button and label to the menu
    sbvLayout->addWidget(planetContainer, 0, Qt::AlignTop);

    /* STAR SELECTOR */
    // Widget
    starContainer = new QWidget;
    starContainer->setFixedWidth(50);
    starContainer->setToolTip("Select to spawn a star:\nLarge, high mass, "
                              "high gravitational force");
    vStarLayout = new QVBoxLayout(starContainer);
    vStarLayout->setMargin(0);
    // Button
    starButton = new QPushButton(sbContainer);
    starButton->setIcon(QIcon(QDir::currentPath() + "/sprites/star.png"));
    starButton->setIconSize(QSize(40, 40));
    starButton->setAutoFillBackground(true);
    starButton->setPalette(QPalette(QColor(255,255,255)));
    starButton->setCheckable(true);
    starButton->setMinimumHeight(50);
    connect(starButton, &QPushButton::clicked, this, [=] {setSpawnType(Body::Star);});
    // Label
    starLabel = new QLabel("Star");
    starLabel->setAlignment(Qt::AlignCenter);
    // Combine
    vStarLayout->addWidget(starButton);
    vStarLayout->addWidget(starLabel);
    // Add to menu
    sbvLayout->addWidget(starContainer, 0, Qt::AlignTop);

    /* WHITE DWARF SELECTOR */
    // Widget
    whitedwarfContainer = new QWidget;
    whitedwarfContainer->setFixedWidth(50);
    whitedwarfContainer->setToolTip("Select to spawn a white dwarf:\nSmall, "
                                    "high mass, very high gravitational force");
    vWhitedwarfLayout = new QVBoxLayout(whitedwarfContainer);
    vWhitedwarfLayout->setMargin(0);
    // Button
    whitedwarfButton = new QPushButton(sbContainer);
    whitedwarfButton->setIcon(QIcon(QDir::currentPath() + "/sprites/whitedwarf.png"));
    whitedwarfButton->setIconSize(QSize(40, 40));
    whitedwarfButton->setAutoFillBackground(true);
    whitedwarfButton->setPalette(QPalette(QColor(255,255,255)));
    whitedwarfButton->setMinimumHeight(50);
    whitedwarfButton->setCheckable(true);
    connect(whitedwarfButton, &QPushButton::clicked, this, [=] {setSpawnType(Body::WhiteDwarf);});
    // Label
    whitedwarfLabel = new QLabel("White Dwarf");
    whitedwarfLabel->setWordWrap(true);
    whitedwarfLabel->setAlignment(Qt::AlignCenter);
    whitedwarfLabel->setFixedHeight(25);
    // Combine
    vWhitedwarfLayout->addWidget(whitedwarfButton);
    vWhitedwarfLayout->addWidget(whitedwarfLabel);
    // Add to menu
    sbvLayout->addWidget(whitedwarfContainer, 0, Qt::AlignTop);

    /* BLACK HOLE SELECTOR */
    // Widget
    blackholeContainer = new QWidget;
    blackholeContainer->setFixedWidth(50);
    blackholeContainer->setToolTip("Select to spawn a black hole:\nSmall, "
                                   "very high mass, extremely high gravitational force");
    vBlackholeLayout = new QVBoxLayout(blackholeContainer);
    vBlackholeLayout->setMargin(0);
    // Button
    blackholeButton = new QPushButton(sbContainer);
    blackholeButton->setIcon(QIcon(QDir::currentPath() + "/sprites/blackhole.png"));
    blackholeButton->setIconSize(QSize(40, 40));
    blackholeButton->setAutoFillBackground(true);
    blackholeButton->setPalette(QPalette(QColor(255,255,255)));
    blackholeButton->setMinimumHeight(50);
    blackholeButton->setCheckable(true);
    connect(blackholeButton, &QPushButton::clicked, this, [=] {setSpawnType(Body::BlackHole);});
    // Label
    blackholeLabel = new QLabel("Black Hole");
    blackholeLabel->setWordWrap(true);
    blackholeLabel->setAlignment(Qt::AlignCenter);
    // Combine
    vBlackholeLayout->addWidget(blackholeButton);
    vBlackholeLayout->addWidget(blackholeLabel);
    // Add to menu
    sbvLayout->addWidget(blackholeContainer, 0, Qt::AlignTop);

    /* PLANETARY SYSTEM SELECTOR */
    // Widget
    plansysContainer = new QWidget;
    plansysContainer->setFixedWidth(50);
    plansysContainer->setToolTip("Select to spawn a planetary system:\nA random central "
                                 "body surrounded by a number of randomly generated planets");
    vPlansysLayout = new QVBoxLayout(plansysContainer);
    vPlansysLayout->setMargin(0);
    // Button
    plansysButton = new QPushButton(sbContainer);
    plansysButton->setIcon(QIcon(QDir::currentPath() + "/sprites/plansys.png"));
    plansysButton->setIconSize(QSize(40, 40));
    plansysButton->setAutoFillBackground(true);
    plansysButton->setPalette(QPalette(QColor(255,255,255)));
    plansysButton->setMinimumHeight(50);
    plansysButton->setCheckable(true);
    connect(plansysButton, &QPushButton::clicked, this, [=] {setSpawnType(Body::PlanetarySystem);});
    // Label
    plansysLabel = new QLabel("Planetary System");
    plansysLabel->setWordWrap(true);
    plansysLabel->setAlignment(Qt::AlignCenter);
    plansysLabel->setFixedHeight(25);
    // Combine
    vPlansysLayout->addWidget(plansysButton);
    vPlansysLayout->addWidget(plansysLabel);
    // Add to menu
    sbvLayout->addWidget(plansysContainer, 0, Qt::AlignTop);

    // Add stretch between last body selector and the home button
    // Keeps the button spacing consistent and differentiates the home button
    sbvLayout->addStretch();

    // Home button (back to main menu)
    sbHomeButton = new QPushButton(sbContainer);
    sbHomeButton->setIcon(QIcon(QDir::currentPath() + "/icons/home.png"));
    sbHomeButton->setFixedWidth(50);
    sbHomeButton->setIconSize(QSize(40, 40));
    connect(sbHomeButton, &QPushButton::clicked, this, [=]{
        sim->setPaused(true);
        createMainMenu();
        clearSandboxMode();
    });
    sbvLayout->addWidget(sbHomeButton, 0, Qt::AlignBottom);

    sim->setPaused(false);
}

/**
 * @brief MainWindow::clearSandboxMode Deletes the sandbox mode components.
 */
void MainWindow::clearSandboxMode() {
    iconCycleRunning = false;
    sim->resetSim();
    delete sbContainer;
}

/**
 * @brief MainWindow::createControlsScreen Creates the controls screen and
 * its components and puts them on the screen.
 */
void MainWindow::createControlsScreen() {
    // Container widget for the whole scontrols screen
    csContainer = new QWidget;
    csContainer->setAutoFillBackground(true);
    csContainer->setPalette(QPalette(QColor(50,50,50)));
    // Horizontal layout:
    // filler | text | home button
    cshLayout = new QHBoxLayout(csContainer);
    // Filler to ensure the text is centred on the screen and not offset
    // by adding the home button on the right
    QWidget filler;
    filler.setMinimumWidth(50);
    cshLayout->addWidget(&filler);
    // Vertical layout for the text
    csvLayout = new QVBoxLayout();

    // Show the controls screen
    setCentralWidget(csContainer);

    // Controls screen title
    csTitle = new QLabel("<p style=\"font-family:Sans Serif; font-size:50pt;\"><b>Controls</b></p>");
    csvLayout->addWidget(csTitle, 0, Qt::AlignCenter);

    // Container widget for sandbox controls (to keep them together)
    csSandboxContainer = new QWidget;
    csvSandboxLayout = new QVBoxLayout(csSandboxContainer);
    // Sandbox controls title
    csSandboxTitle = new QLabel("<h3>Sandbox Controls</h3>");
    csvSandboxLayout->addWidget(csSandboxTitle, 0, Qt::AlignCenter);
    // Sandbox controls
    csSandboxText = new QLabel("In the right bar, click on the icon of the type of body you want to spawn (default asteroid). "
                               "Hover over the icon to find out about the properties of that particular body. "
                               "Once you have chosen your celestial body, click and drag on the screen to spawn and fling it. "
                               "The further you drag, the greater the body's velocity as it spawns.");
    csSandboxText->setWordWrap(true);
    csSandboxText->setMinimumHeight(100);
    csvSandboxLayout->addWidget(csSandboxText, 0, Qt::AlignCenter);
    csvLayout->addWidget(csSandboxContainer, 0, Qt::AlignCenter);

    // Push components to the top
    csvLayout->addStretch();

    // Add central text component
    cshLayout->addLayout(csvLayout);

    // Home button (back to main menu)
    csHomeButton = new QPushButton(csContainer);
    csHomeButton->setIcon(QIcon(QDir::currentPath() + "/icons/home.png"));
    csHomeButton->setFixedWidth(50);
    csHomeButton->setIconSize(QSize(40, 40));
    connect(csHomeButton, &QPushButton::clicked, this, [=]{
        createMainMenu();
        clearControlsScreen();
    });
    cshLayout->addWidget(csHomeButton, 0, Qt::AlignBottom);
}

/**
 * @brief MainWindow::clearControlsScreen Deletes the controls screen components.
 */
void MainWindow::clearControlsScreen() {
    delete csContainer;
}

/**
 * @brief MainWindow::setSpawnType When a body selector button is pressed,
 * passes the type of body to spawn onto the SimulationWindow class, and
 * unchecks all other buttons pressed.
 * @param type The type of body to spawn
 */
void MainWindow::setSpawnType(Body::BodyType type) {
    switch (type) {
        case Body::Asteroid:
            planetButton->setChecked(false);
            starButton->setChecked(false);
            whitedwarfButton->setChecked(false);
            blackholeButton->setChecked(false);
            plansysButton->setChecked(false);
            break;
        case Body::Planet:
            asteroidButton->setChecked(false);
            starButton->setChecked(false);
            whitedwarfButton->setChecked(false);
            blackholeButton->setChecked(false);
            plansysButton->setChecked(false);
            break;
        case Body::Star:
            asteroidButton->setChecked(false);
            planetButton->setChecked(false);
            whitedwarfButton->setChecked(false);
            blackholeButton->setChecked(false);
            plansysButton->setChecked(false);
            break;
        case Body::WhiteDwarf:
            asteroidButton->setChecked(false);
            planetButton->setChecked(false);
            starButton->setChecked(false);
            blackholeButton->setChecked(false);
            plansysButton->setChecked(false);
            break;
        case Body::BlackHole:
            asteroidButton->setChecked(false);
            planetButton->setChecked(false);
            starButton->setChecked(false);
            whitedwarfButton->setChecked(false);
            plansysButton->setChecked(false);
            break;
        case Body::PlanetarySystem:
            asteroidButton->setChecked(false);
            planetButton->setChecked(false);
            starButton->setChecked(false);
            whitedwarfButton->setChecked(false);
            blackholeButton->setChecked(false);
            break;
    }
    simwin->setSpawnType(type);
}

/**
 * @brief MainWindow::cyclePlanetIcons Cycles through the various planet
 * sprites and sets the planet button icon to the next planet ever second.
 */
void MainWindow::cyclePlanetIcons() {
    int i = 1;
    QString fileName;
    while (iconCycleRunning) {
        switch (i) {
            case 1:
                fileName = "/sprites/planet1.png";
                break;
            case 2:
                fileName = "/sprites/planet2.png";
                break;
            case 3:
                fileName = "/sprites/planet3.png";
                break;
            case 4:
                fileName = "/sprites/planet4.png";
                break;
            case 5:
                fileName = "/sprites/planet5.png";
                break;
        }
        planetButton->setIcon(QIcon(QDir::currentPath() + fileName));
        planetButton->setIconSize(QSize(40, 40));
        if (i == 5) i = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        i++;
    }
}










