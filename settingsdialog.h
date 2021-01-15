#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void on_cellSizeSlider_valueChanged(int value);
    void on_rowSlider_valueChanged(int value);
    void on_columnSlider_valueChanged(int value);
    void on_minesSlider_valueChanged(int value);

private:
    Ui::SettingsDialog *ui;
    void updateMineCountMax();

    // QDialog interface
public slots:
    void accept() override;
};

#endif // SETTINGSDIALOG_H
