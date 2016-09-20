#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "walshtranform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    WalshTransform walsh;
    QImage *img;
    int wold, hold;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void load();
    void loadImage();
    void transform();
    void restore();
    void save();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
