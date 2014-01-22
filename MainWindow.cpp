#include "MainWindow.h"
#include "GLWidget.h"
#include "ui_mainwindow.h"
#include "Highlighter.h"

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

    highlighter = new Highlighter(ui->textEdit->document());

    setWindowTitle(tr("Qrush!"));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete highlighter;
}
