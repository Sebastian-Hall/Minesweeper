#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"
#include <QHeaderView>
#include <QSettings>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),
    clockUpdateTimer(new QTimer(this)), minefield(nullptr), delegate(nullptr), gameboard(nullptr)
{
    //Setup UI from form
    ui->setupUi(this);

    //Connect menu actions
    connect(ui->actionNew_Game, &QAction::triggered, this, &MainWindow::resetGame);
    connect(ui->actionQuit, &QAction::triggered, this, &QApplication::exit);
    connect(ui->actionHelp, &QAction::triggered, this, &MainWindow::helpMessage);
    connect(ui->actionSettings, &QAction::triggered, []() {
        SettingsDialog dialog;
        dialog.exec();
    });
    connect(ui->newGameButton, &QPushButton::clicked, this, &MainWindow::resetGame);

    //Setup game
    resetGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadGameSettings()
{
    //Load each value from settings, has default values
    QSettings settings("Sebastian Games", "Minesweeper", this);
    settings.beginGroup("userSettings");
    cellSize = settings.value("cellSize", 40).toInt();
    gameRows = settings.value("gameRows", 10).toInt();
    gameCols = settings.value("gameCols", 16).toInt();
    mineCount = settings.value("mineCount", 20).toInt();
    ui->difficultyComboBox->setCurrentText(settings.value("difficulty", "Intermediate").toString());
    settings.endGroup();
}

void MainWindow::initMinefield()
{
    minefield = new MinefieldModel(gameRows, gameCols, mineCount , this);
}

void MainWindow::initGameboard()
{
    //Create and set size
    gameboard = new Gameboard();
    gameboardSize = QSize(gameCols * cellSize + 2, gameRows * cellSize + 2);
    gameboard->setFixedSize(gameboardSize);
    setGameboardCellSize(gameboard->horizontalHeader(), gameboard->verticalHeader());
}

void MainWindow::initTopDisplay()
{
    //Set update timer stuff
    clockUpdateTimer->stop();
    clockUpdateTimer->setInterval(100);

    //Setup time display
    ui->timeLCDNumber->setDigitCount(3);
    ui->timeLCDNumber->display(0);
    ui->timeLCDNumber->setMinimumHeight(40);

    //Setup mines display
    ui->minesLCDNumber->setDigitCount(4);
    ui->minesLCDNumber->display(mineCount);
    ui->minesLCDNumber->setMinimumHeight(40);

    //Setup new game button
    ui->newGameButton->setIcon(QIcon(":/images/face_alive.png"));
}

void MainWindow::setGameboardCellSize(QHeaderView *hor, QHeaderView *ver)
{
    //Set size ranges and default size
    hor->setMinimumSectionSize(MIN_CELL_SIZE);
    ver->setMinimumSectionSize(MIN_CELL_SIZE);
    hor->setMaximumSectionSize(MAX_CELL_SIZE);
    ver->setMaximumSectionSize(MAX_CELL_SIZE);
    hor->setDefaultSectionSize(cellSize);
    ver->setDefaultSectionSize(cellSize);
}

void MainWindow::initDelegate()
{
    delegate = new MinefieldDelegate(this);
}

void MainWindow::initMainWindow()
{
    //Set title and icon
    this->setWindowIcon(QIcon(":/images/mine.png"));
    this->setWindowTitle("Minesweeper");
}

void MainWindow::constructGame()
{
    //Combine model with game view
    gameboard->setModel(minefield);
    gameboard->setItemDelegate(delegate);

    //Add game view to app
    ui->verticalLayout->addWidget(gameboard);

    //Connect game components
    connect(clockUpdateTimer, &QTimer::timeout, this, &MainWindow::updateClockDisplay);
    connect(minefield, &MinefieldModel::mineDisplayUpdated, this, &MainWindow::updateMineCountDisplay);
    connect(minefield, &MinefieldModel::gameOver, this, &MainWindow::stopGame);
    connect(gameboard, &Gameboard::gameStarted, minefield, &MinefieldModel::populateMines);
    connect(gameboard, &Gameboard::gameStarted, this, &MainWindow::startGame);
}

void MainWindow::stopGame(bool gameWon)
{
    //Disable gameboard mouse input events
    gameboard->disableView();

    //Stop timers
    clockUpdateTimer->stop();

    //Prepare output message
    double score = (gameTimer.isValid() ? gameTimer.elapsed() / 1000.0 : 0);
    QString output;

    if(gameWon) {
        output = QString("You Won In %0 Seconds").arg(score);
        ui->newGameButton->setIcon(QIcon(":/images/face_heidi.png"));
    } else {
        output = QString("You Lost In %0 Seconds").arg(score);
        ui->newGameButton->setIcon(QIcon(":/images/face_dead.png"));
    }

    //Show user game won / game loss message
    QMessageBox::information(this, "Game Over", output);
}

void MainWindow::resetGame()
{
    //Delete old game
    if(minefield != nullptr) delete minefield;
    if(delegate != nullptr) delete delegate;
    if(gameboard != nullptr) delete gameboard;

    //Load new game
    loadGameSettings();

    //Initialize pieces
    initMinefield();
    initTopDisplay();
    initDelegate();
    initGameboard();
    initMainWindow();

    //Build game
    constructGame();
}

void MainWindow::startGame()
{
    clockUpdateTimer->start();
    gameTimer.restart();
}

void MainWindow::helpMessage()
{
    QString msg("Try to open all cells without exploding any mines\n"\
                "Left click to open cells\n"\
                "Right click to flag potential mines\n"\
                "Create a custom game in the settings or choose your own difficulty\n"\
                "Settings changes will only be applied on new game");
    QMessageBox::information(this, "About Minesweeper", msg);
}

void MainWindow::updateClockDisplay()
{
    int elapsedTime = gameTimer.elapsed() / 1000;
    ui->timeLCDNumber->display(elapsedTime);
}

void MainWindow::updateMineCountDisplay(int mineCountDisplay)
{
    ui->minesLCDNumber->display(mineCountDisplay);
}

bool MainWindow::event(QEvent *event)
{
    if(event->type() == QEvent::LayoutRequest) {
        this->setFixedSize(this->sizeHint());
    }
    return QMainWindow::event(event);
}

void MainWindow::on_difficultyComboBox_activated(int index)
{
    //Get difficulty from list
    QString difficulty = ui->difficultyComboBox->itemData(index, Qt::DisplayRole).toString();

    //Access user settings
    QSettings settings("Sebastian Games", "Minesweeper", this);
    settings.beginGroup("userSettings");

    //Apply new settings based on difficulty
    settings.setValue("difficulty", difficulty);
    if(difficulty == "Beginner") {
        settings.setValue("gameRows", 8);
        settings.setValue("gameCols", 10);
        settings.setValue("mineCount", 7);
    } else if(difficulty == "Easy") {
        settings.setValue("gameRows", 10);
        settings.setValue("gameCols", 13);
        settings.setValue("mineCount", 16);
    } else if(difficulty == "Intermediate") {
        settings.setValue("gameRows", 15);
        settings.setValue("gameCols", 20);
        settings.setValue("mineCount", 40);
    } else if(difficulty == "Expert") {
        settings.setValue("gameRows", 19);
        settings.setValue("gameCols", 26);
        settings.setValue("mineCount", 99);
    } else if(difficulty == "Custom") {
        SettingsDialog dialog = SettingsDialog(this);
        dialog.exec();
    }
    settings.endGroup();

    resetGame();
}
