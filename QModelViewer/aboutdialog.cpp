#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowCloseButtonHint)
    , ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    connect(ui->pushButtonOK, &QAbstractButton::clicked,
            [=]() {
                accept();
            });
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
