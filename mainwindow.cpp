#include "mainwindow.h"

MainWindow::MainWindow(Simulation *sim, Sprites sprites, SimulationWindow *simwin, QWidget *simContainer) {
    this->simwin = simwin;
    // Container for everything on the screen, including the simulation window and the control bar
    QWidget *container = new QWidget;
    container->setAutoFillBackground(true);
    container->setPalette(QPalette(QColor(50,50,50)));
    QHBoxLayout *hLayout = new QHBoxLayout(container);
    hLayout->addWidget(simContainer, 0);
    hLayout->setMargin(5); // Border around all components
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addLayout(vLayout);

    setCentralWidget(container);
    resize(1000, 500);

    // Title for body selector menu
    QLabel *spawnLabel = new QLabel("<b>Select Body to Spawn</b>");
    spawnLabel->setWordWrap(true);
    spawnLabel->setAlignment(Qt::AlignCenter);
    spawnLabel->setFixedWidth(50);
    vLayout->addWidget(spawnLabel, 0, Qt::AlignTop);

    /* ASTEROID SELECTOR */
    // Widget containing both button and label
    QWidget *asteroidContainer = new QWidget;
    asteroidContainer->setFixedWidth(50);
    asteroidContainer->setToolTip("Select to spawn asteroids:\nSmall, "
                                  "low mass, low gravitational force");
    QVBoxLayout *vAsteroidLayout = new QVBoxLayout(asteroidContainer);
    vAsteroidLayout->setMargin(0); // No border
    // Asteroid button
    asteroidButton = new QPushButton(container);
    asteroidButton->setIcon(QIcon(QDir::currentPath() + "/sprites/asteroid.png"));
    asteroidButton->setIconSize(QSize(40, 40));
    asteroidButton->setAutoFillBackground(true);
    asteroidButton->setPalette(QPalette(QColor(255,255,255)));
    asteroidButton->setMinimumHeight(50);
    asteroidButton->setCheckable(true);
    asteroidButton->setChecked(true);
    asteroidButton->setAutoExclusive(true); // Only one button can be selected at a time
    // What happens when we press the button
    connect(asteroidButton, &QPushButton::clicked, this, [=] {setSpawnType(1);});
    // Asteroid label
    QLabel *asteroidLabel = new QLabel("Asteroid");
    asteroidLabel->setAlignment(Qt::AlignCenter);
    // Combine button and label
    vAsteroidLayout->addWidget(asteroidButton);
    vAsteroidLayout->addWidget(asteroidLabel);
    // Add both button and label to the menu
    vLayout->addWidget(asteroidContainer, 0, Qt::AlignTop);

    /* STAR SELECTOR */
    // Widget
    QWidget *starContainer = new QWidget;
    starContainer->setFixedWidth(50);
    starContainer->setToolTip("Select to spawn stars:\nLarge, high mass, "
                              "high gravitational force");
    QVBoxLayout *vStarLayout = new QVBoxLayout(starContainer);
    vStarLayout->setMargin(0);
    // Button
    starButton = new QPushButton(container);
    starButton->setIcon(QIcon(QDir::currentPath() + "/sprites/star.png"));
    starButton->setIconSize(QSize(40, 40));
    starButton->setAutoFillBackground(true);
    starButton->setPalette(QPalette(QColor(255,255,255)));
    starButton->setCheckable(true);
    starButton->setAutoExclusive(true);
    starButton->setMinimumHeight(50);
    connect(starButton, &QPushButton::clicked, this, [=] {setSpawnType(0);});
    // Label
    QLabel *starLabel = new QLabel("Star");
    starLabel->setAlignment(Qt::AlignCenter);
    // Combine
    vStarLayout->addWidget(starButton);
    vStarLayout->addWidget(starLabel);
    // Add to menu
    vLayout->addWidget(starContainer, 0, Qt::AlignTop);

    /* WHITE DWARF SELECTOR */
    // Widget
    QWidget *whitedwarfContainer = new QWidget;
    whitedwarfContainer->setFixedWidth(50);
    whitedwarfContainer->setToolTip("Select to spawn white dwarfs:\nSmall, "
                                    "high mass, very high gravitational force");
    QVBoxLayout *vWhitedwarfLayout = new QVBoxLayout(whitedwarfContainer);
    vWhitedwarfLayout->setMargin(0);
    // Button
    whitedwarfButton = new QPushButton(container);
    whitedwarfButton->setIcon(QIcon(QDir::currentPath() + "/sprites/whitedwarf.png"));
    whitedwarfButton->setIconSize(QSize(40, 40));
    whitedwarfButton->setAutoFillBackground(true);
    whitedwarfButton->setPalette(QPalette(QColor(255,255,255)));
    whitedwarfButton->setMinimumHeight(50);
    whitedwarfButton->setCheckable(true);
    whitedwarfButton->setAutoExclusive(true);
    connect(whitedwarfButton, &QPushButton::clicked, this, [=] {setSpawnType(2);});
    // Label
    QLabel *whitedwarfLabel = new QLabel("White Dwarf");
    whitedwarfLabel->setWordWrap(true);
    whitedwarfLabel->setAlignment(Qt::AlignCenter);
    // Combine
    vWhitedwarfLayout->addWidget(whitedwarfButton);
    vWhitedwarfLayout->addWidget(whitedwarfLabel);
    // Add to menu
    vLayout->addWidget(whitedwarfContainer, 0, Qt::AlignTop);

    /* BLACK HOLE SELECTOR */
    // Widget
    QWidget *blackholeContainer = new QWidget;
    blackholeContainer->setFixedWidth(50);
    blackholeContainer->setToolTip("Select to spawn black holes:\nSmall, "
                                   "very high mass, extremely high gravitational force");
    QVBoxLayout *vBlackholeLayout = new QVBoxLayout(blackholeContainer);
    vBlackholeLayout->setMargin(0);
    // Button
    blackholeButton = new QPushButton(container);
    blackholeButton->setIcon(QIcon(QDir::currentPath() + "/sprites/blackhole.png"));
    blackholeButton->setIconSize(QSize(40, 40));
    blackholeButton->setAutoFillBackground(true);
    blackholeButton->setPalette(QPalette(QColor(255,255,255)));
    blackholeButton->setMinimumHeight(50);
    blackholeButton->setCheckable(true);
    blackholeButton->setAutoExclusive(true);
    connect(blackholeButton, &QPushButton::clicked, this, [=] {setSpawnType(3);});
    // Label
    QLabel *blackholeLabel = new QLabel("Black Hole");
    blackholeLabel->setWordWrap(true);
    blackholeLabel->setAlignment(Qt::AlignCenter);
    // Combine
    vBlackholeLayout->addWidget(blackholeButton);
    vBlackholeLayout->addWidget(blackholeLabel);
    // Add to menu
    vLayout->addWidget(blackholeContainer, 0, Qt::AlignTop);
}

/**
 * @brief MainWindow::setSpawnType When a body selector button is pressed,
 * passes the type of body to spawn onto the SimulationWindow class, and
 * unchecks all other buttons pressed.
 * @param type The type of body to spawn
 */
void MainWindow::setSpawnType(int type) {
    switch (type) {
        case 0:
            asteroidButton->setChecked(false);
            whitedwarfButton->setChecked(false);
            blackholeButton->setChecked(false);
            break;
        case 1:
            starButton->setChecked(false);
            whitedwarfButton->setChecked(false);
            blackholeButton->setChecked(false);
            break;
        case 2:
            asteroidButton->setChecked(false);
            starButton->setChecked(false);
            blackholeButton->setChecked(false);
            break;
        case 3:
            asteroidButton->setChecked(false);
            starButton->setChecked(false);
            whitedwarfButton->setChecked(false);
            break;
    }
    simwin->setSpawnType(type);
}












