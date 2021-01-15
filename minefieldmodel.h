#ifndef MINEFIELDMODEL_H
#define MINEFIELDMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QColor>
#include "cell.h"

class MinefieldModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    int rows,
    columns,
    mineCount,
    mineDisplayCount,
    cellsClosed;
    QVector<QVector<Cell *>> cells;

    int countStatusNear(const QModelIndex &index, Cell::CellStatus status) const;
    bool indexInBounds(int i, int j) const;
    void floodFill(const QModelIndex &index);

public:
    explicit MinefieldModel(int rows = 5, int columns = 5, int mineCount = 8, QObject *parent = nullptr);
    ~MinefieldModel();
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    enum Role {
        OpenStatusRole = Qt::UserRole + 1,
        MineStatusRole,
        FlagStatusRole,
        MineCountRole
    };

public slots:
    void populateMines(int row, int col);

signals:
    void gameOver(bool gameWon);
    void mineDisplayUpdated(int mineDisplayCount);
};

#endif // MINEFIELDMODEL_H
