#ifndef CELL_H
#define CELL_H

class Cell
{
public:
    enum CellStatus { Flagged = (1 << 0),
                      Opened = (1 << 1),
                      HasMine = (1 << 2) };
private:
    int minesAdjacent,
        status;

public:
    Cell();
    void setMinesAdjacent(int minesAdjacent);
    int getMinesAdjacent() const;
    void setStatusFlag(CellStatus flag);
    void toggleStatusFlag(CellStatus flag);
    void clearStatusFlag(CellStatus flag);
    bool isStatusFlagSet(CellStatus flag) const;
    void resetStatus();

};

#endif // CELL_H
