#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButtonTransform->setDisabled(true);
    walsh.setLog(ui->log);

    connect(ui->pushButtonLoad, SIGNAL(clicked()), this, SLOT(load()));
    connect(ui->pushButtonLoadImg, SIGNAL(clicked()), this, SLOT(loadImage()));
    connect(ui->pushButtonTransform, SIGNAL(clicked()), this, SLOT(transform()));
    connect(ui->pushButtonRestore, SIGNAL(clicked()), this, SLOT(restore()));
    connect(ui->pusgButtonSave, SIGNAL(clicked()), this, SLOT(save()));

    img = NULL;
    wold = 0;
    hold = 0;

    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image in Text Format"), NULL, tr("Text Files (*.txt)"));
    QFile file(fileName);
    if (file.exists())
    {
        ui->pushButtonTransform->setEnabled(true);

        int size = walsh.load(fileName);
        ui->spinBoxLevel->setMaximum(size*size);
        ui->spinBoxLevel->setValue(size*size);
        ui->label->setText(QString::number(size*size));

        img = walsh.txtToPixmap(img, WalshTransform::IMAGE_ORIGINAL);
        ui->image->setPixmap(QPixmap::fromImage(*img).scaled(ui->image->size()));
    }
}

void MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image in Text Format"), NULL, tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));
    QFile file(fileName);
    if (file.exists())
    {
        if (img)
            delete img;
        img = new QImage(fileName);
        wold = img->width();
        hold = img->height();

        ui->pushButtonTransform->setEnabled(true);

        int size = walsh.load(img);
        ui->spinBoxLevel->setMaximum(size*size);
        ui->spinBoxLevel->setValue(size*size);
        ui->label->setText(QString::number(size*size));

        img = walsh.txtToPixmap(img, WalshTransform::IMAGE_ORIGINAL);

        ui->image->setPixmap(QPixmap::fromImage(img->copy(QRect(0, 0, wold, hold))).scaled(ui->image->size(), Qt::KeepAspectRatio));
    }
}

void MainWindow::transform()
{
    walsh.transform();
    walsh.filter(ui->spinBoxLevel->value(), ui->comboBox->currentIndex());

    img = walsh.txtToPixmap(img, WalshTransform::IMAGE_SPECTR2D);
    ui->spectr->setPixmap(QPixmap::fromImage(*img).scaled(ui->spectr->size(), Qt::KeepAspectRatio));
}

void MainWindow::restore()
{
    walsh.retransform();
    img = walsh.txtToPixmap(img, WalshTransform::IMAGE_RESTORED);
    ui->restore->setPixmap(QPixmap::fromImage(img->copy(QRect(0, 0, wold, hold))).scaled(ui->restore->size(), Qt::KeepAspectRatio));
}

void MainWindow::save()
{
    img = walsh.txtToPixmap(img, WalshTransform::IMAGE_ORIGINAL);
    if (img)
        img->save("result/Original.png", "png");
    img = walsh.txtToPixmap(img, WalshTransform::IMAGE_SPECTR2D);
    if (img)
        img->save("result/Filter00-Spectr.png", "png");
    img = walsh.txtToPixmap(img, WalshTransform::IMAGE_RESTORED);
    if (img)
        img->save("result/Filter00-Image.png", "png");
}
