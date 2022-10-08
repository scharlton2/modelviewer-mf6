#ifndef LIGHTINGDIALOG_H
#define LIGHTINGDIALOG_H

#include <QDialog>

namespace Ui {
class LightingDialog;
}

class LightingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LightingDialog(QWidget *parent = nullptr);
    ~LightingDialog();

private:
    Ui::LightingDialog *ui;
};

#endif // LIGHTINGDIALOG_H
