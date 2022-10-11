#include "lightingdialog.h"
#include "ui_lightingdialog.h"

LightingDialog::LightingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LightingDialog)
{
    ui->setupUi(this);
}

LightingDialog::~LightingDialog()
{
    delete ui;
}
