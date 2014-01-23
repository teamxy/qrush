#include "MainWindow.h"
#include "GLWidget.h"
#include "ui_mainwindow.h"
#include "Highlighter.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GLWidget *canvas = new GLWidget(this);
    setCentralWidget(canvas);

    highlighter = new Highlighter(ui->jsTextEdit->document());

    setWindowTitle(tr("Qrush!"));

    ui->jsConsoleTextEdit->setText("");

    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));

    log("Ready :)");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete highlighter;
}


void MainWindow::log(const QString &message)
{
    QTime time;

    ui->jsConsoleTextEdit->append("["+time.currentTime().toString()+"] "+message);
}

void MainWindow::saveButtonClicked()
{
    log("save");
    log(ui->jsTextEdit->toPlainText());
}
