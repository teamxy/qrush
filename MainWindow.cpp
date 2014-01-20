#include "MainWindow.h"
#include "GLWidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GLWidget *canvas = new GLWidget(this);
    this->ui->gridLayout->addWidget(canvas, 0, 0);




    this->setWindowTitle(tr("Qrush!"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
