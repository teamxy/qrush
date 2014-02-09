#include "../include/MainWindow.h"
#include "ui_mainwindow.h"
#include "../include/Highlighter.h"
#include "../include/Brush.h"
#include <QTime>
#include <QInputDialog>
#include <QFile>
#include <QDir>
#include <QFuture>
#include <QtConcurrent>

#include <iostream>

// TODO put in some file
const static QString BOILERPLATE_JS =
"var x0, y0;\n\
\n\
function onRelease(x,y){\n\
  x0 = undefined;\n\
  y0 = undefined;\n\
}\n\
\n\
function onDrag(x,y){\n\
if(x0 && y0){\n\
  line(x0,y0,x, y, 0x00FF00);\n\
}\n\
  x0 = x;\n\
  y0 = y;\n\
}";

static QString JS_PATH = "js/";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // init the .qrush directory

  JS_PATH = QDir::homePath() + "/.qrush/qrush-brush";
  QDir jsDir;
  jsDir.mkpath(JS_PATH);

  // init ui

  ui->jsConsoleTextEdit->setText("");

  canvas = new GLWidget(this);
  ui->horizontalLayout_2->addWidget(canvas);
  //setCentralWidget(canvas);


  highlighter = new Highlighter(ui->jsTextEdit->document());

  setWindowTitle(tr("Qrush!"));

  connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNewBrush()));
  connect(ui->comboBox, SIGNAL(currentTextChanged(QString)), SLOT(brushChanged(QString)));
  connect(this, SIGNAL(signalFileLoaded(QString,QString)), this, SLOT(fileLoaded(QString,QString)));
  connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
  connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(redo()));

  // load all js files in the current working directory

  QDir dir(JS_PATH);
  dir.setFilter(QDir::Files);

  QStringList list = dir.entryList();

  for(int i = 0; i < list.size(); ++i) {
      QString name = list.at(i);

      if(!name.endsWith(".js"))
          continue;

      ui->comboBox->addItem(name);
  }

  log("Ready :)");
  log("Your scripts path is: " + JS_PATH);
}

MainWindow::~MainWindow() {
  delete ui;
  delete highlighter;
}

// Todo make this available to V8/Brush
void MainWindow::log(const QString &message) {
  QTime time;
  ui->jsConsoleTextEdit->append("["+time.currentTime().toString()+"] "+message);
}

void MainWindow::logError(const QString &message) {
    QString formattedMessage;

    formattedMessage.append("<font color='red' style='white-space: pre'>");
    formattedMessage.append(message);
    formattedMessage.append("</font>");

    log(formattedMessage);
}

void MainWindow::saveButtonClicked() {
  QString source = ui->jsTextEdit->toPlainText();

  QFile file(JS_PATH + "/" + ui->comboBox->currentText());
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  file.write(source.toStdString().data());
  file.close();

  log(ui->comboBox->currentText() + " saved");

  std::shared_ptr<Brush> brush(new Brush(source));
  canvas->setBrush(brush);
}

void MainWindow::addNewBrush() {
  log("new Brush");

  // make a new input dialog
  bool ok;
  QString brushName = QInputDialog::getText(this, tr("New Brush"), tr("Brush name:"), QLineEdit::Normal, "", &ok);

  // append js if needed
  if(!brushName.endsWith(".js")) brushName.append(".js");

  if (ok && !brushName.isEmpty()) {
    log("New brush "+brushName);

    // add to combobox
    ui->comboBox->addItem(brushName);
    ui->comboBox->setCurrentText(brushName);

    // add some boilerplate code
    ui->jsTextEdit->setText(BOILERPLATE_JS);
  }
}

void MainWindow::fileLoaded(QString filename, QString content) {

    // somehow we can not create a new Brush object in a thread

    std::shared_ptr<Brush> brush(new Brush(content));

    canvas->setBrush(brush);

    ui->jsTextEdit->setText(content);
    ui->jsTextEdit->setEnabled(true);

    log(filename + " loaded");

}

void MainWindow::brushChanged(QString brushName) {

    ui->jsTextEdit->setEnabled(false);

    QtConcurrent::run(
      [brushName, this] () {
        QFile file(JS_PATH + "/" + brushName);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString source = file.readAll();
        file.close();

        emit signalFileLoaded(brushName, source);
      }
    );

}

void MainWindow::undo() {
  log("undo");
  canvas->undo();
}

void MainWindow::redo() {
  log("redo");
  canvas->redo();
}
