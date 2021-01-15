#include "gameboard.h"
#include "minefieldmodel.h"
#include <QMouseEvent>
#include <QHeaderView>


Gameboard::Gameboard(QWidget *parent) : QTableView(parent), disabled(false), started(false)
{
    //Set board styling
    this->setFont(QFont("Segoe UI", 14, 900));
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setFocusPolicy(Qt::NoFocus);
    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();
    this->setShowGrid(false);
}

void Gameboard::mousePressEvent(QMouseEvent *event)
{
    //Do not handle any events if game not playable
    if(disabled) {
        QTableView::mousePressEvent(event);
        return;
    }

    //Get cell clicked from local event coordinates
    int row = this->rowAt(event->position().y());
    int col = this->columnAt(event->position().x());
    QModelIndex index = this->model()->index(row, col);

    //Start game if not started
    if(!started) {
        started = true;
        emit gameStarted(row, col);
    }

    //Handle left and right clicks
    if(event->button() == Qt::LeftButton) {
        this->model()->setData(index, QVariant(), MinefieldModel::OpenStatusRole);
        event->accept();
    } else if(event->button() == Qt::RightButton) {
        this->model()->setData(index, QVariant(), MinefieldModel::FlagStatusRole);
        event->accept();
    } else {
        event->ignore();
    }

    //Propogate upward
    QTableView::mousePressEvent(event);
}

void Gameboard::mouseDoubleClickEvent(QMouseEvent *event)
{
    //Redirect to mouse press event to allow for fast consecutive clicks
    Gameboard::mousePressEvent(event);
}

void Gameboard::disableView()
{
    disabled = true;
}
