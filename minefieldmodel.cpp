#include "minefieldmodel.h"
#include <QRandomGenerator>
#include <QStack>


MinefieldModel::MinefieldModel(int rows, int columns, int mineCount, QObject *parent) : QAbstractTableModel(parent),
    rows(rows), columns(columns), mineCount(mineCount), mineDisplayCount(mineCount), cellsClosed(rows * columns)
{
    //Fill minefield with cells
    for(int i = 0; i < rows; ++i) {
        cells.push_back(QVector<Cell *>(columns));
        for(int j = 0; j < columns; ++j) {
            cells[i][j] = new Cell();
        }
    }
}

MinefieldModel::~MinefieldModel() {
    for(int i = 0; i < cells.size(); ++i) {
        qDeleteAll(cells[i]);
    }
}

int MinefieldModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rows;
}

int MinefieldModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns;
}

QVariant MinefieldModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);

    Cell *currentCell = cells[index.row()][index.column()];

    if(role == MinefieldModel::OpenStatusRole) {
        return currentCell->isStatusFlagSet(Cell::Opened);
    } else if(role == MinefieldModel::MineStatusRole) {
        return currentCell->isStatusFlagSet(Cell::HasMine);
    } else if(role == MinefieldModel::FlagStatusRole) {
        return currentCell->isStatusFlagSet(Cell::Flagged);
    } else if(role == MinefieldModel::MineCountRole) {
        return currentCell->getMinesAdjacent();
    }

    return QVariant();
}

bool MinefieldModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value);

    //Get cell clicked on in model
    Cell *currentCell = cells[index.row()][index.column()];

    //Handle flagging and opening rolls
    if(role == MinefieldModel::FlagStatusRole) {//Attempt to flag cell
        if(!currentCell->isStatusFlagSet(Cell::Opened)) {//Do not do any flagging if cell open
            if(currentCell->isStatusFlagSet(Cell::Flagged)) {
                currentCell->clearStatusFlag(Cell::Flagged);
                ++mineDisplayCount;
            } else {
                currentCell->setStatusFlag(Cell::Flagged);
                --mineDisplayCount;
            }

            //Notify
            emit mineDisplayUpdated(mineDisplayCount);
            emit dataChanged(index, index);
        }
    } else if(role == MinefieldModel::OpenStatusRole) {//Attempt to open cell
        if(!currentCell->isStatusFlagSet(Cell::Flagged)) {//Do not open if cell flagged or open
            //Open and mark changed
            if(!currentCell->isStatusFlagSet(Cell::Opened)) --cellsClosed;
            currentCell->setStatusFlag(Cell::Opened);
            emit dataChanged(index, index);

            //Check for mine, else attempt floodfill
            if(currentCell->isStatusFlagSet(Cell::HasMine)) {
                emit gameOver(false);
                return true;
            } else {
                floodFill(index);
            }

            //Game win condition
            if(cellsClosed == mineCount) emit gameOver(true);
        }
    }

    return true;
}

void MinefieldModel::populateMines(int clickedRow, int clickedCol)
{
    //Represent 2D minefield as 1D array of indices
    int size = qMax(1, rows * columns);//Just to quiet debug statments
    int indices[size];
    for(int i = 0; i < size; ++i) {
        indices[i] = i;
    }

    //Random shuffle indices to choose which have mines
    for(int i = 0; i < 2 * size; ++i) {
        int a = QRandomGenerator::global()->generate() % (size);
        int b = QRandomGenerator::global()->generate() % (size);
        int tmp = indices[a];
        indices[a] = indices[b];
        indices[b] = tmp;
    }

    //Choose first mineCount indices to have mines
    for(int i = 0, lim = mineCount; i < lim; ++i) {
        int curRow = indices[i] / columns, curCol = indices[i] % columns;

        //Increase limit by one and continue to not have first click be mine
        if(curRow == clickedRow && curCol == clickedCol) {
            ++lim;
            continue;
        }
        cells[curRow][curCol]->setStatusFlag(Cell::HasMine);
    }

    //Count adjacent mines for each cell
    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < columns; ++j) {
            //If has mine do not bother counting adjacent mines
            if(!cells[i][j]->isStatusFlagSet(Cell::HasMine))
                cells[i][j]->setMinesAdjacent(this->countStatusNear(this->index(i, j), Cell::HasMine));
        }
    }
}

int MinefieldModel::countStatusNear(const QModelIndex &index, Cell::CellStatus status) const
{
    int count = 0, offset[3] = {-1, 0, 1};


    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            QModelIndex adjIndex = this->index(index.row() + offset[i], index.column() + offset[j]);
            if(adjIndex.isValid() && cells[adjIndex.row()][adjIndex.column()]->isStatusFlagSet(status)) {
                    ++count;
            }
        }
    }

    return count;
}

bool MinefieldModel::indexInBounds(int i, int j) const
{
    return (i >= 0) && (j >= 0) && (i < rows) && (j < columns);
}

void MinefieldModel::floodFill(const QModelIndex &index)
{
    int rowIndex = index.row(), colIndex = index.column();
    Cell *cell = cells[rowIndex][colIndex];

    //Floodfill possible if cell has no bombs adjacent or bombs adjacent is equal to flags adjacent
    if(cell->getMinesAdjacent() == 0 || (this->countStatusNear(index, Cell::HasMine) == this->countStatusNear(index, Cell::Flagged))) {
        int offset[3] = {-1, 0, 1};//Used in double for loop to get all adjacent cells

        //Add clicked cell to dfs stack
        QStack<QPair<int, int>> dfs;
        dfs.push({rowIndex, colIndex});

        //While cells to open
        while(!dfs.empty()) {
            QPair<int, int> curIndex = dfs.top();
            dfs.pop();

            //All cells in 3x3 square around cell are adjacent to it
            for(int i = 0; i < 3; ++i) {
                for(int j = 0; j < 3; ++j) {
                    int a = curIndex.first + offset[i], b = curIndex.second + offset[j];

                    //If invalid index skip, if flagged don't open, if open then already visited so skip
                    if(!indexInBounds(a, b) || cells[a][b]->isStatusFlagSet(Cell::Flagged) || cells[a][b]->isStatusFlagSet(Cell::Opened))
                        continue;

                    //Set cell as opened and let view know
                    if(!cells[a][b]->isStatusFlagSet(Cell::Opened)) --cellsClosed;
                    cells[a][b]->setStatusFlag(Cell::Opened);
                    emit dataChanged(this->index(a, b), this->index(a, b));

                    //If opening mine, else add to dfs if no adjacent mines
                    if(cells[a][b]->isStatusFlagSet(Cell::HasMine)) {
                        emit gameOver(false);
                        return;
                    } else if(cells[a][b]->getMinesAdjacent() == 0) {
                        dfs.push({a, b});
                    }
                }
            }
        }
    }
}

