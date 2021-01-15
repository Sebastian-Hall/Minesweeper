#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    //Load last settings
    QSettings settings("Sebastian Games", "Minesweeper", this);
    settings.beginGroup("userSettings");
    int cellSize = settings.value("cellSize", 50).toInt();
    int gameRows = settings.value("gameRows", 10).toInt();
    int gameCols = settings.value("gameCols", 16).toInt();
    int mineCount = settings.value("mineCount", 20).toInt();
    settings.endGroup();

    //Apply last settings to ui
    ui->cellSizeSlider->setValue(cellSize);
    ui->rowSlider->setValue(gameRows);
    ui->columnSlider->setValue(gameCols);
    ui->minesSlider->setValue(mineCount);
    updateMineCountMax();

    //Set minimums
    ui->cellSizeSlider->setMinimum(20);
    ui->rowSlider->setMinimum(2);
    ui->columnSlider->setMinimum(2);
    ui->minesSlider->setMinimum(1);

    //Set maximums
    ui->cellSizeSlider->setMaximum(80);
    ui->rowSlider->setMaximum(70);
    ui->columnSlider->setMaximum(130);

    //Connect buttons to slots
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    //Set title
    this->setWindowTitle("Settings");
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_cellSizeSlider_valueChanged(int value)
{
    ui->cellSizeLcd->display(value);
}

void SettingsDialog::on_rowSlider_valueChanged(int value)
{
    ui->rowLcd->display(value);
    updateMineCountMax();
}

void SettingsDialog::on_columnSlider_valueChanged(int value)
{
    ui->columnLcd->display(value);
    updateMineCountMax();
}

void SettingsDialog::on_minesSlider_valueChanged(int value)
{
    ui->minesLcd->display(value);
}

void SettingsDialog::updateMineCountMax()
{
    int max = ui->rowSlider->value() * ui->columnSlider->value() - 1;
    ui->minesSlider->setMaximum(max);
}

void SettingsDialog::accept()
{
    //Save settings if accepted
    QSettings settings("Sebastian Games", "Minesweeper", this);
    settings.beginGroup("userSettings");
    settings.setValue("cellSize", ui->cellSizeSlider->value());
    settings.setValue("gameRows", ui->rowSlider->value());
    settings.setValue("gameCols", ui->columnSlider->value());
    settings.setValue("mineCount", ui->minesSlider->value());
    settings.endGroup();

    QDialog::accept();
}

