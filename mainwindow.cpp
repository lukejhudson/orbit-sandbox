#include <iostream>
#include "mainwindow.h"

#define TITLE_STYLE "\"font-family:Sans Serif; font-size:70pt;\""
#define BUTTON_STYLE "QPushButton {background-color: #4f5154; color: white;}"

#define SB_SELECTOR_WIDTH 60

MainWindow::MainWindow() {
    sprites = new Sprites();
    setWindowTitle("Orbit Sandbox");
    resize(1000, 660);
    setMinimumSize(1000, 660);

    sim = new Simulation(*sprites);

    titleFont = new QFont("Fantasy", 70, QFont::Bold);
    buttonFont = new QFont("Fantasy", 14, QFont::Bold);
    labelFont = new QFont("Fantasy", 10);
    labelFontBold = new QFont("Fantasy", 10, QFont::Bold);

    createMainMenu();
}

/**
 * @brief MainWindow::createMainMenu Creates the main menu screen and
 * its components and puts them on the screen.
 */
void MainWindow::createMainMenu() {
    mode = MainMenu;
    sim->setMode(Simulation::Background);
    // Container widget for whole main menu, including the simulation background
    mmContainer = new QWidget;
    mmContainer->setAutoFillBackground(false);
    mmContainer->setPalette(QPalette(QColor(50,50,50)));

    // Puts the main menu onto the screen
    setCentralWidget(mmContainer);

    // Create simulation widget to have a basic simulation going on in the background
    simWidget = new SimulationWidget(sim, *sprites);
    // Set its parent to the main menu container (i.e. adds it)
    simWidget->setParent(mmContainer);
    simWidget->setGeometry(0, 0, width(), height());
    // Reduce opacity so the focus remains on the main menu
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;
    effect->setOpacity(0.8);
    simWidget->setGraphicsEffect(effect);
    sim->setPaused(false);

    // Add contents vertically
    mmvLayout = new QVBoxLayout(mmContainer);

    // Main menu title
    title = new QLabel("<p style=" TITLE_STYLE "><b>Orbit Sandbox</b></p>");
    mmvLayout->addWidget(title, 0, Qt::AlignCenter);
    // Credits
    credits = new QLabel("<h3>By Luke Hudson</h3>");
    mmvLayout->addWidget(credits, 0, Qt::AlignCenter);
    // Button to enter sandbox mode
    sandboxModeButton = new QPushButton("Sandbox Mode");
    sandboxModeButton->setFont(*buttonFont);
    sandboxModeButton->setStyleSheet(BUTTON_STYLE);
    sandboxModeButton->setFixedWidth(200);
    connect(sandboxModeButton, &QPushButton::clicked, this, [=]{
        sim->setPaused(true);
        createSandboxMode();
        clearMainMenu();
    });
    mmvLayout->addWidget(sandboxModeButton, 0, Qt::AlignCenter);

    // Button to enter exploration mode
    explorationModeButton = new QPushButton("Exploration Mode");
    explorationModeButton->setFont(*buttonFont);
    explorationModeButton->setStyleSheet(BUTTON_STYLE);
    explorationModeButton->setFixedWidth(200);
    connect(explorationModeButton, &QPushButton::clicked, this, [=]{
        sim->setPaused(true);
        createExplorationMode();
        clearMainMenu();
    });
    mmvLayout->addWidget(explorationModeButton, 0, Qt::AlignCenter);

    // Button to enter controls screen
    controlsScreenButton = new QPushButton("Controls");
    controlsScreenButton->setFont(*buttonFont);
    controlsScreenButton->setStyleSheet(BUTTON_STYLE);
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
 * Deleting the top-level component deletes all of its child components.
 */
void MainWindow::clearMainMenu() {
    delete mmContainer;
}

/**
 * @brief MainWindow::createSandboxMode Creates the sandbox game mode and
 * its components and puts them on the screen.
 */
void MainWindow::createSandboxMode() {
    mode = Sandbox;
    sim->setMode(Simulation::Sandbox);
    // Create simulation widget
    simWidget = new SimulationWidget(sim, *sprites);
    // Container for everything on the screen, including the simulation window and the control bar
    sbContainer = new QWidget;
    sbContainer->setAutoFillBackground(true);
    sbContainer->setPalette(QPalette(QColor(50,50,50)));

    setCentralWidget(sbContainer);

    sbhLayout = new QHBoxLayout(sbContainer);
    sbhLayout->addWidget(simWidget, 0);
    sbhLayout->setMargin(5); // Border around all components
    sbvLayout = new QVBoxLayout();
    sbhLayout->addLayout(sbvLayout);

    // Title for body selector menu
    spawnLabel = new QLabel("<b>Select Body to Spawn</b>");
    spawnLabel->setFont(*labelFontBold);
    spawnLabel->setWordWrap(true);
    spawnLabel->setAlignment(Qt::AlignCenter);
    spawnLabel->setFixedWidth(SB_SELECTOR_WIDTH);
    sbvLayout->addWidget(spawnLabel, 0, Qt::AlignTop);

    /* ASTEROID SELECTOR */
    // Widget containing both button and label
    asteroidContainer = new QWidget;
    asteroidContainer->setFixedWidth(SB_SELECTOR_WIDTH);
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
    asteroidLabel->setFont(*labelFont);
    asteroidLabel->setAlignment(Qt::AlignCenter);
    // Combine button and label
    vAsteroidLayout->addWidget(asteroidButton);
    vAsteroidLayout->addWidget(asteroidLabel);
    // Add both button and label to the menu
    sbvLayout->addWidget(asteroidContainer, 0, Qt::AlignTop);

    /* PLANET SELECTOR */
    // Widget containing both button and label
    planetContainer = new QWidget;
    planetContainer->setFixedWidth(SB_SELECTOR_WIDTH);
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
    planetLabel->setFont(*labelFont);
    planetLabel->setAlignment(Qt::AlignCenter);
    // Combine button and label
    vPlanetLayout->addWidget(planetButton);
    vPlanetLayout->addWidget(planetLabel);
    // Add both button and label to the menu
    sbvLayout->addWidget(planetContainer, 0, Qt::AlignTop);

    /* STAR SELECTOR */
    // Widget
    starContainer = new QWidget;
    starContainer->setFixedWidth(SB_SELECTOR_WIDTH);
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
    starLabel->setFont(*labelFont);
    starLabel->setAlignment(Qt::AlignCenter);
    // Combine
    vStarLayout->addWidget(starButton);
    vStarLayout->addWidget(starLabel);
    // Add to menu
    sbvLayout->addWidget(starContainer, 0, Qt::AlignTop);

    /* WHITE DWARF SELECTOR */
    // Widget
    whitedwarfContainer = new QWidget;
    whitedwarfContainer->setFixedWidth(SB_SELECTOR_WIDTH);
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
    whitedwarfLabel->setFont(*labelFont);
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
    blackholeContainer->setFixedWidth(SB_SELECTOR_WIDTH);
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
    blackholeLabel->setFont(*labelFont);
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
    plansysContainer->setFixedWidth(SB_SELECTOR_WIDTH);
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
    plansysLabel->setFont(*labelFont);
    plansysLabel->setWordWrap(true);
    plansysLabel->setAlignment(Qt::AlignCenter);
    plansysLabel->setFixedHeight(25);
    // Combine
    vPlansysLayout->addWidget(plansysButton);
    vPlansysLayout->addWidget(plansysLabel);
    // Add to menu
    sbvLayout->addWidget(plansysContainer, 0, Qt::AlignTop);

    /* GRAVITY SLIDER */
    // Widget
    gravityContainer = new QWidget;
    gravityContainer->setFixedWidth(SB_SELECTOR_WIDTH);
    gravityContainer->setToolTip("Use the slider to adjust the strength of gravity");
    vGravityLayout = new QVBoxLayout(gravityContainer);
    vGravityLayout->setMargin(0);
    // Slider
    sbGravitySlider = new QSlider(Qt::Horizontal, sbContainer);
    sbGravitySlider->setMinimum(-4);
    sbGravitySlider->setMaximum(4);
    sbGravitySlider->setValue(0);
    connect(sbGravitySlider, &QSlider::valueChanged, this, [=] {
        sim->setG(pow(2, sbGravitySlider->value()));
    });
    // Label
    gravityLabel = new QLabel("Gravity");
    gravityLabel->setFont(*labelFont);
    gravityLabel->setAlignment(Qt::AlignCenter);
    // Combine
    vGravityLayout->addWidget(sbGravitySlider);
    vGravityLayout->addWidget(gravityLabel);
    // Add to menu
    sbvLayout->addWidget(gravityContainer, 0, Qt::AlignTop);

    // Add stretch between last body selector and the home button
    // Keeps the button spacing consistent and differentiates the home button
    sbvLayout->addStretch();

    // Home button (back to main menu)
    sbHomeButton = new QPushButton(sbContainer);
    sbHomeButton->setIcon(QIcon(QDir::currentPath() + "/icons/home.png"));
    sbHomeButton->setFixedWidth(SB_SELECTOR_WIDTH);
    sbHomeButton->setIconSize(QSize(40, 40));
    connect(sbHomeButton, &QPushButton::clicked, this, [=]{
        sim->setPaused(true);
        createMainMenu();
        clearSandboxMode();
    });
    sbvLayout->addWidget(sbHomeButton, 0, Qt::AlignBottom);

    // Start the simulation
    sim->setPaused(false);
}

/**
 * @brief MainWindow::clearSandboxMode Deletes the sandbox mode components.
 */
void MainWindow::clearSandboxMode() {
    // Stop cycling icons on the planet button so the thread finishes
    iconCycleRunning = false;
    delete sbContainer;
}

/**
 * @brief MainWindow::createExplorationMode Creates the exploration game
 * mode and its components and puts them on the screen.
 */
void MainWindow::createExplorationMode() {
    mode = Exploration;
    sim->setMode(Simulation::Exploration);
    simWidget = new SimulationWidget(sim, *sprites);
    // Container for everything on the screen - simulation and home button
    exContainer = simWidget;
    exContainer->setAutoFillBackground(true);
    exContainer->setPalette(QPalette(QColor(50,50,50)));
    exContainer->setMinimumSize(500, 300);
    setCentralWidget(exContainer);

    // Home button (back to main menu)
    exHomeButton = new QPushButton(exContainer);
    exHomeButton->setIcon(QIcon(QDir::currentPath() + "/icons/home.png"));
    exHomeButton->setFixedWidth(50);
    exHomeButton->setIconSize(QSize(40, 40));
    connect(exHomeButton, &QPushButton::clicked, this, [=]{
        sim->setPaused(true);
        createMainMenu();
        clearExplorationMode();
    });
    exHomeButton->move(width() - 55, height() - 53);

    // Game over text
    QString str("GAME OVER");
    QFontMetrics fm(*titleFont);
    exGameOverWidth = fm.width(str); // Find width of text
    exGameOver = new QLabel(str, exContainer);
    exGameOver->setStyleSheet("QLabel {color: #dbdbdb}");
    exGameOver->setFont(*titleFont);
    exGameOver->move(width() / 2 - exGameOverWidth / 2, height() / 3);
    exGameOver->hide();

    // Main menu button (shows when game over)
    exMainMenu = new QPushButton("Main Menu", exContainer);
    exMainMenu->setFont(*buttonFont);
    exMainMenu->setStyleSheet(BUTTON_STYLE);
    exMainMenu->setFixedWidth(200);
    connect(exMainMenu, &QPushButton::clicked, this, [=]{
        sim->setPaused(true);
        createMainMenu();
        clearExplorationMode();
    });
    exMainMenu->move(width() / 2 - 100, height() / 3 + 125);
    exMainMenu->hide();

    // Listen for the game over signal from SimulationWidget
    connect(simWidget, &SimulationWidget::gameOverSignal, this, [=]{
        exHomeButton->hide();
        exGameOver->show();
        exMainMenu->show();
    });

    // Start the simulation
    sim->setPaused(false);
}

/**
 * @brief MainWindow::clearExplorationMode Deletes the exploration mode
 * components.
 */
void MainWindow::clearExplorationMode() {
    delete exContainer;
}

/**
 * @brief MainWindow::createControlsScreen Creates the controls screen and
 * its components and puts them on the screen.
 */
void MainWindow::createControlsScreen() {
    mode = ControlsScreen;
    // Container widget for the whole scontrols screen
    csContainer = new QWidget;
    csContainer->setAutoFillBackground(true);
    csContainer->setPalette(QPalette(QColor(50,50,50)));
    // Horizontal layout:
    // filler | text | home button
    cshLayout = new QHBoxLayout(csContainer);
    // Filler to ensure the text is centred on the screen and not offset
    // by adding the home button on the right
    QWidget *filler = new QWidget;
    filler->setFixedWidth(50);
    cshLayout->addWidget(filler);
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
    // Sandbox controls description
    csSandboxText = new QLabel("In the right bar, click on the icon of the type of body you want to spawn (default asteroid). "
                               "Hover over the icon to find out about the properties of that particular body. "
                               "Once you have chosen your celestial body, click and drag on the screen to spawn and fling it. "
                               "The further you drag, the greater the body's velocity as it spawns.");
    csSandboxText->setWordWrap(true);
    csSandboxText->setMinimumHeight(120);
    csvSandboxLayout->addWidget(csSandboxText, 0, Qt::AlignCenter);
    csvLayout->addWidget(csSandboxContainer, 0, Qt::AlignCenter);

    // Container widget for exploration controls (to keep them together)
    csExplorationContainer = new QWidget;
    csvExplorationLayout = new QVBoxLayout(csExplorationContainer);
    // Exploration controls title
    csExplorationTitle = new QLabel("<h3>Exploration Controls</h3>");
    csvExplorationLayout->addWidget(csExplorationTitle, 0, Qt::AlignCenter);
    // Exploration controls description
    csExplorationText = new QLabel("In the Exploration mode you control a rocket, placed in the centre of the screen. Press and "
                                   "hold W to fire the rocket's engines and increase your velocity in the direction you are facing. "
                                   "Use the A and D keys to rotate the rocket anti-clockwise and clockwise respectively. Fly the "
                                   "rocket around to explore the procedurally generated universe, but try not to crash!");
    csExplorationText->setWordWrap(true);
    csExplorationText->setMinimumHeight(120);
    csvExplorationLayout->addWidget(csExplorationText, 0, Qt::AlignCenter);
    csvLayout->addWidget(csExplorationContainer, 0, Qt::AlignCenter);

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
 * passes the type of body to spawn onto the SimulationWidget class, and
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
        default:
            // Do nothing
            break;
    }
    simWidget->setSpawnType(type);
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

/**
 * @brief MainWindow::getMode Returns which mode the window is currently in.
 * See MainWindow::Mode.
 * @return The current mode of the window
 */
int MainWindow::getMode() {
    return mode;
}

/**
 * @brief MainWindow::resizeEvent Handles the resizing of the window.
 * @param event The resize event
 */
void MainWindow::resizeEvent(QResizeEvent *event) {
    if (mode == MainMenu && simWidget) {
        simWidget->resize(event->size());
    } else if (mode == Exploration) {
        exHomeButton->move(width() - 55, height() - 53);
        exGameOver->move(width() / 2 - exGameOverWidth / 2, height() / 3);
        exMainMenu->move(width() / 2 - 100, height() / 3 + 125);
    } else {
        // Let the parent class perform any functions it needs to
        QMainWindow::resizeEvent(event);
    }
}

/**
 * @brief MainWindow::keyPressEvent Pass a received key press event
 * onto SimulationWidget to be handled.
 * @param event The key press event to handle
 */
void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (mode == Exploration) {
        QApplication::sendEvent(simWidget, event);
    } else {
        // Pass on key press to base class
        QMainWindow::keyPressEvent(event);
    }
}

/**
 * @brief MainWindow::keyReleaseEvent Pass a received key release event
 * onto SimulationWidget to be handled.
 * @param event The key release event to handle
 */
void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (mode == Exploration) {
        QApplication::sendEvent(simWidget, event);
    } else {
        // Pass on key release to base class
        QMainWindow::keyReleaseEvent(event);
    }
}





















