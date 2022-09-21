#include "geometrydialog.h"
#include "ui_geometrydialog.h"

#include <QMessageBox>
#include <QString>

#include <mvdoc.h>

GeometryDialog::GeometryDialog(QWidget *parent, MvDoc *doc)
    : QDialog(parent, Qt::WindowCloseButtonHint)
    , ui(new Ui::GeometryDialog)
    , doc(doc)
    , _geometry(nullptr)
    , xScale(1.)
    , yScale(1.)
    , zScale(1.)
    , representation(0)
    , axesSize(0)
    , tubeDiameter(1)
    , xPos(0.)
    , yPos(0.)
    , zPos(0.)
{
    ui->setupUi(this);

    // axes
    connect(ui->radioButtonLine, &QAbstractButton::pressed, this, &GeometryDialog::onRadioButtonLine);
    connect(ui->radioButtonTube, &QAbstractButton::pressed, this, &GeometryDialog::onRadioButtonTube);

    // bounding box
    connect(ui->radioButtonBlack, &QAbstractButton::pressed, this, &GeometryDialog::onRadioButtonBlack);
    connect(ui->radioButtonGray, &QAbstractButton::pressed, this, &GeometryDialog::onRadioButtonGray);
    connect(ui->radioButtonWhite, &QAbstractButton::pressed, this, &GeometryDialog::onRadioButtonWhite);

    // tab
    connect(ui->tabWidget, QOverload<int>::of(&QTabWidget::currentChanged), this, QOverload<int>::of(&GeometryDialog::onTabChanged));

    // buttons
    connect(ui->pushButtonApply, &QAbstractButton::clicked, this, &GeometryDialog::onApply);
    connect(ui->pushButtonDone, &QAbstractButton::clicked, this, &GeometryDialog::onDone);
}

GeometryDialog::~GeometryDialog()
{
    delete ui;
    delete _geometry;
}

void GeometryDialog::reinitialize()
{
    // scale
    ui->lineEditXScale->setText("1");
    ui->lineEditYScale->setText("1");
    ui->lineEditZScale->setText("1");
    activateScale(false);

    // axes
    representation = 0;                         // 0 = tube 1 = line
    ui->radioButtonTube->setChecked(true);
    ui->lineEditAxesSize->setText("");
    ui->lineEditTubeDiameter->setText("");
    ui->lineEditPositionX->setText("");
    ui->lineEditPositionY->setText("");
    ui->lineEditPositionZ->setText("");
    activateAxes(false);

    // bounding box
    ui->radioButtonBlack->setChecked(true);
    activateBoundingBox(false);

    // tab
    ui->tabWidget->setCurrentIndex(0);          // 0 = scale, 1 = axes, 2 = bounding box
}

bool GeometryDialog::updateData(bool saveAndValidate)
{
    int n = ui->tabWidget->currentIndex();
    switch (n)
    {
    case 0: // scale
        return updateDataScale(saveAndValidate);
        break;
    case 1: // axes
        return updateDataAxes(saveAndValidate);
        break;
    case 2: // bounding box
        return updateDataBoundingBox(saveAndValidate);
        break;
    default:
        break;
    }
    return true;
}

bool GeometryDialog::updateDataScale(bool saveAndValidate)
{
    if (saveAndValidate)
    {
        QString s;
        double  val;
        bool    ok;

        // xScale
        s   = ui->lineEditXScale->text();
        val = s.toDouble(&ok);
        if (!ok || val <= 0)
        {
            QMessageBox::warning(this, "", tr("Please enter a positive number."));
            ui->lineEditXScale->setFocus();
            return false;
        }
        xScale = val;

        // yScale
        s      = ui->lineEditYScale->text();
        val    = s.toDouble(&ok);
        if (!ok || val <= 0)
        {
            QMessageBox::warning(this, "", tr("Please enter a positive number."));
            ui->lineEditYScale->setFocus();
            return false;
        }
        yScale = val;

        // zScale
        s      = ui->lineEditZScale->text();
        val    = s.toDouble(&ok);
        if (!ok || val <= 0)
        {
            QMessageBox::warning(this, "", tr("Please enter a positive number."));
            ui->lineEditZScale->setFocus();
            return false;
        }
        zScale = val;
    }
    else
    {
        // scale
        ui->lineEditXScale->setText(QString("%1").arg(xScale));
        ui->lineEditYScale->setText(QString("%1").arg(yScale));
        ui->lineEditZScale->setText(QString("%1").arg(zScale));
    }
    return true;
}

bool GeometryDialog::updateDataAxes(bool saveAndValidate)
{
    if (saveAndValidate)
    {
        QString s;
        double  val;
        bool    ok;

        // axesSize
        s      = ui->lineEditAxesSize->text();
        val    = s.toDouble(&ok);
        if (!ok || val <= 0)
        {
            QMessageBox::warning(this, "", tr("Please enter a positive number."));
            ui->lineEditAxesSize->setFocus();
            return false;
        }
        axesSize = val;

        // tubeDiameter
        s   = ui->lineEditTubeDiameter->text();
        val = s.toDouble(&ok);
        if (!ok || val <= 0)
        {
            QMessageBox::warning(this, "", tr("Please enter a positive number."));
            ui->lineEditTubeDiameter->setFocus();
            return false;
        }
        tubeDiameter = val;

        // xPos
        s   = ui->lineEditPositionX->text();
        val = s.toDouble(&ok);
        if (!ok)
        {
            QMessageBox::warning(this, "", tr("Please enter a number."));
            ui->lineEditPositionX->setFocus();
            return false;
        }
        xPos = val;

        // yPos
        s    = ui->lineEditPositionY->text();
        val  = s.toDouble(&ok);
        if (!ok)
        {
            QMessageBox::warning(this, "", tr("Please enter a number."));
            ui->lineEditPositionY->setFocus();
            return false;
        }
        yPos = val;


        // zPos
        s    = ui->lineEditPositionZ->text();
        val  = s.toDouble(&ok);
        if (!ok)
        {
            QMessageBox::warning(this, "", tr("Please enter a number."));
            ui->lineEditPositionZ->setFocus();
            return false;
        }
        zPos = val;
    }
    else
    {
        // 0 = tube 1 = line
        if (representation == 0)
        {
            ui->radioButtonTube->setChecked(true);
        }
        else
        {
            ui->radioButtonLine->setChecked(true);
        }

        // axesSize
        ui->lineEditAxesSize->setText(QString("%1").arg(axesSize));

        // tubeDiameter
        ui->lineEditTubeDiameter->setText(QString("%1").arg(tubeDiameter));

        // position
        ui->lineEditPositionX->setText(QString("%1").arg(xPos));
        ui->lineEditPositionY->setText(QString("%1").arg(yPos));
        ui->lineEditPositionZ->setText(QString("%1").arg(zPos));
    }
    return true;
}

bool GeometryDialog::updateDataBoundingBox(bool saveAndValidate)
{
    if (saveAndValidate)
    {
        // no-op
    }
    else
    {
        switch (boundingBoxColor)
        {
        case 0:
            ui->radioButtonBlack->setChecked(true);
            break;
        case 1:
            ui->radioButtonGray->setChecked(true);
            break;
        case 2:
            ui->radioButtonWhite->setChecked(true);
            break;
        default:
            assert(false);
        }
    }
    return true;
}

void GeometryDialog::hideEvent(QHideEvent *e)
{
    delete _geometry;
    _geometry = new QRect(geometry());
}

void GeometryDialog::showEvent(QShowEvent *e)
{
    if (_geometry)
    {
        setGeometry(*_geometry);
    }
}

void GeometryDialog::onApply()
{
    if (updateData(true))
    {
        int n = this->ui->tabWidget->currentIndex();

        switch (n)
        {
        case 0:         // scale
            doc->setScale(xScale, yScale, zScale);
            break;
        case 1:         // axes
            doc->setAxesProperties(xPos, yPos, zPos, axesSize, tubeDiameter);
            break;
        case 2:         // bounding box
            // no-op
            break;
        default:
            assert(false);
        }
    }
}

void GeometryDialog::onDone()
{
    hide();
}

void GeometryDialog::activateScale(bool b)
{
    // scale tab
    ui->lineEditXScale->setEnabled(b);
    ui->lineEditYScale->setEnabled(b);
    ui->lineEditZScale->setEnabled(b);

    if (ui->tabWidget->currentIndex() == 0)
    {
        ui->pushButtonApply->setEnabled(b);
    }
}

void GeometryDialog::activateAxes(bool b)
{
    // axes symbol tab
    ui->radioButtonTube->setEnabled(b);
    ui->radioButtonLine->setEnabled(b);

    ui->lineEditAxesSize->setEnabled(b);
    ui->lineEditTubeDiameter->setEnabled(b && ui->radioButtonTube->isChecked());

    ui->lineEditPositionX->setEnabled(b);
    ui->lineEditPositionY->setEnabled(b);
    ui->lineEditPositionZ->setEnabled(b);

    if (ui->tabWidget->currentIndex() == 1)
    {
        ui->pushButtonApply->setEnabled(b);
    }
}

void GeometryDialog::activateBoundingBox(bool b)
{
    // bounding box tab
    ui->radioButtonBlack->setEnabled(b);
    ui->radioButtonGray->setEnabled(b);
    ui->radioButtonWhite->setEnabled(b);
}

void GeometryDialog::onRadioButtonBlack()
{
    doc->setBoundingBoxColor(0, 0, 0);
}

void GeometryDialog::onRadioButtonGray()
{
    doc->setBoundingBoxColor(0.5, 0.5, 0.5);
}

void GeometryDialog::onRadioButtonWhite()
{
    doc->setBoundingBoxColor(1, 1, 1);
}

void GeometryDialog::onTabChanged(int index)
{
    int n = this->ui->tabWidget->currentIndex();
    switch (n)
    {
    case 0: // scale
        ui->pushButtonApply->setEnabled(ui->lineEditXScale->isEnabled());
        break;
    case 1: // axes
        ui->pushButtonApply->setEnabled(ui->radioButtonTube->isEnabled());
        break;
    case 2: // bounding box
        ui->pushButtonApply->setEnabled(false);
        break;
    default:
        assert(false);
    }
}

void GeometryDialog::onRadioButtonLine()
{
    // axes
    doc->setAxesRepresentationToLine();
    ui->lineEditTubeDiameter->setEnabled(false);
}

void GeometryDialog::onRadioButtonTube()
{
    // axes
    doc->setAxesRepresentationToTube();
    ui->lineEditTubeDiameter->setEnabled(true);
}

void GeometryDialog::enableApplyButton(bool enabled)
{
    ui->pushButtonApply->setEnabled(enabled);
}

void GeometryDialog::setCurrentIndex(int index)
{
    ui->tabWidget->setCurrentIndex(index);
}
