#include "MainWindow.h"
#include "GLWidget.h"
#include "ui_mainwindow.h"
#include "Form.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Form* form = new Form();
    //setCentralWidget(form);

    GLWidget *canvas = new GLWidget(this);
    setCentralWidget(canvas);
    //this->ui->groupBox->addWidget(canvas);

    this->setWindowTitle(tr("Qrush!"));

}

MainWindow::~MainWindow()
{
    delete ui;
}
