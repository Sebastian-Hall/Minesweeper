#ifndef MINEFIELDDELEGATE_H
#define MINEFIELDDELEGATE_H

#include <QStyledItemDelegate>
#include <QPixmap>

class MinefieldDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit MinefieldDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    //Cell styling and background
    QPixmap minePixmap,
            mineRedPixmap,
            flagPixmap,
            cellClosedPixmap;
    QVector<QPixmap> cellNumbers;
};

#endif // MINEFIELDDELEGATE_H
