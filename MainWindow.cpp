#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Highlighter.h"
#include "Brush.h"
#include <QTime>
#include <QInputDialog>
#include <QFile>
#include <QDir>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ui->jsConsoleTextEdit->setText("");

  canvas = new GLWidget(this);
  setCentralWidget(canvas);

  // temporary to prevent crashing until fully implemented
  std::shared_ptr<Brush> brush(new Brush(this, "var y0, x0; function onDrag(x,y){line(x0,y0,x, y, 0xFF0000); x0=x; y0=y;};"));
  canvas->setBrush(brush);

  highlighter = new Highlighter(ui->jsTextEdit->document());

  setWindowTitle(tr("Qrush!"));

  connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNewBrush()));
  connect(ui->comboBox, SIGNAL(currentTextChanged(QString)), SLOT(brushChanged(QString)));

  // load all js files in the current working directory

  QDir dir;
  dir.setFilter(QDir::Files);

  QStringList list = dir.entryList();

  for(int i = 0; i < list.size(); ++i) {
      QString name = list.at(i);

      if(!name.endsWith(".js"))
          continue;

      ui->comboBox->addItem(name);
  }

  log("Ready :)");
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

// TODO save into file, then reparse
void MainWindow::saveButtonClicked() {
  QString source = ui->jsTextEdit->toPlainText();

  QFile file(ui->comboBox->currentText());
  file.open(QIODevice::WriteOnly | QIODevice::Text);
  file.write(source.toStdString().data());
  file.close();

  log(ui->comboBox->currentText() + " saved");

  std::shared_ptr<Brush> brush(new Brush(this, source));
  canvas->setBrush(brush);
}

void MainWindow::addNewBrush() {
  log("new Brush");

  // TODO: make separate brush directory

  // make a new input dialog
  bool ok;
  QString brushName = QInputDialog::getText(this, tr("New Brush"), tr("Brush name:"), QLineEdit::Normal, "", &ok);

  if(!brushName.endsWith(".js"))
      brushName.append(".js");

  if (ok && !brushName.isEmpty()) {
    log("New brush "+brushName);

    ui->comboBox->addItem(brushName);

    QFile file(brushName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.close();

    ui->comboBox->setCurrentText(brushName);
  }
}

void MainWindow::brushChanged(QString brushName) {

    ui->jsTextEdit->setEnabled(true);

    QFile file(brushName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString source = file.readAll();
    file.close();

    ui->jsTextEdit->setText(source);

    std::shared_ptr<Brush> brush(new Brush(this, source));
    canvas->setBrush(brush);

    log(brushName + " loaded");
}
