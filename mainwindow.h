#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QTableView>
#include <QElapsedTimer>
#include <QTimer>
#include "minefieldmodel.h"
#include "minefielddelegate.h"
#include "gameboard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool event(QEvent *event) override;//Used to handle resize events

private:
    Ui::MainWindow *ui;

    //Clock and Timing
    QTimer *clockUpdateTimer;
    QElapsedTimer gameTimer;

    //Minefield model
    MinefieldModel *minefield;
    MinefieldDelegate *delegate;

    //Gameboard view
    Gameboard *gameboard;
    QSize gameboardSize;

    //Game settings
    int cellSize;
    int gameRows;
    int gameCols;
    int mineCount;
    static const int MIN_CELL_SIZE = 20;
    static const int MAX_CELL_SIZE = 80;

    //Private methods for setting up game
    void loadGameSettings();
    void initMinefield();
    void initGameboard();
    void setGameboardCellSize(QHeaderView *hor, QHeaderView *ver);
    void initDelegate();
    void initTopDisplay();
    void initMainWindow();
    void constructGame();

public slots:
    void stopGame(bool gameWon);
    void resetGame();
    void startGame();
    void helpMessage();
    void updateClockDisplay();
    void updateMineCountDisplay(int mineCountDisplay);

private slots:
    void on_difficultyComboBox_activated(int index);
};
#endif // MAINWINDOW_H
