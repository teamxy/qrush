#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Highlighter.h"
#include "Brush.h"
#include <QTime>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ui->jsConsoleTextEdit->setText("");

  canvas = new GLWidget(this);
  setCentralWidget(canvas);

  // temporary to prevent crashing until fully implemented
  std::shared_ptr<Brush> brush(new Brush(this, "var y0, x0; function onDrag(x,y){line(x0,y0,x, y, 0xFF0000); x0=x; y0=y;};"));
  brushes["bumblebee"] = brush;
  canvas->setBrush(brush);

  highlighter = new Highlighter(ui->jsTextEdit->document());

  setWindowTitle(tr("Qrush!"));

  connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNewBrush()));

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
  log("save");
  log(ui->jsTextEdit->toPlainText());
}

void MainWindow::addNewBrush() {
  log("new Brush");

  // make a new input dialog
  bool ok;
  QString text = QInputDialog::getText(this, tr("New Brush"),
      tr("Brush name:"), QLineEdit::Normal, "", &ok);

  if (ok && !text.isEmpty()) {
    log(text);
    // TODO: get script from input and put into file, then reparse files
    std::shared_ptr<Brush> brush(new Brush(this, "function onDrag(x,y){drawPoint(x, y);};"));
    brushes[text.toStdString()] = brush;
    canvas->setBrush(brush);
    // TODO add brush to combo box
  }
}
