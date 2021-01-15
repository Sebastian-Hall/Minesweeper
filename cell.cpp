#include "cell.h"
Cell::Cell() : minesAdjacent(0), status(0)
{
}

void Cell::setMinesAdjacent(int minesAdjacent) {
    this->minesAdjacent = minesAdjacent;
}

int Cell::getMinesAdjacent() const {
    return this->minesAdjacent;
}

void Cell::setStatusFlag(CellStatus flag) {
    this->status |= flag;
}

void Cell::clearStatusFlag(CellStatus flag) {
    int flagNegation = ~(static_cast<int>(flag));
    this->status &= flagNegation;
}

bool Cell::isStatusFlagSet(CellStatus flag) const {
    return this->status & static_cast<int>(flag);
}

void Cell::resetStatus()
{
    this->status = 0;
}

void Cell::toggleStatusFlag(Cell::CellStatus flag)
{
    if(this->isStatusFlagSet(flag)) {
        this->clearStatusFlag(flag);
    } else {
        this->setStatusFlag(flag);
    }
}
