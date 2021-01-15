#include "minefielddelegate.h"
#include "minefieldmodel.h"
#include <QPainter>


MinefieldDelegate::MinefieldDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    //Load background pixmaps
    minePixmap = QPixmap(QString(":/images/mine.png"));
    mineRedPixmap = QPixmap(QString(":/images/red_mine.png"));
    flagPixmap = QPixmap(":/images/flag.png");
    cellClosedPixmap = QPixmap(":/images/cell_closed.png");
    for(int i = 0; i <= 8; ++i) {
        cellNumbers.push_back(QPixmap(":/images/cell_" + QString::number(i) + ".png"));
    }
}

void MinefieldDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //Get data for cell
    bool isOpen = index.data(MinefieldModel::OpenStatusRole).toBool();
    bool isMine = index.data(MinefieldModel::MineStatusRole).toBool();
    bool isFlag = index.data(MinefieldModel::FlagStatusRole).toBool();
    int mineCount = index.data(MinefieldModel::MineCountRole).toInt();
    QSize cellSize(option.rect.width(), option.rect.height());

    painter->save();

    //Set brush to paint cell
    if(isOpen) {
        if(isMine) {
            painter->setBrush(QBrush(mineRedPixmap.scaled(cellSize)));
        } else {
            painter->setBrush(QBrush(cellNumbers[mineCount].scaled(cellSize)));
        }
    } else if(isFlag) {
        painter->setBrush(QBrush(flagPixmap.scaled(cellSize)));
    } else {
        painter->setBrush(QBrush(cellClosedPixmap.scaled(cellSize)));
    }

    //Paint cell
    painter->drawRect(option.rect);

    painter->restore();
}
