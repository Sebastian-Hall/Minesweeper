#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QTableView>

class Gameboard : public QTableView
{
    Q_OBJECT
public:
    explicit Gameboard(QWidget *parent = nullptr);
    void disableView();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    bool disabled;
    bool started;

signals:
    void gameStarted(int row, int col);
};

#endif // GAMEBOARD_H
