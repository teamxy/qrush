#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Highlighter.h"
#include "Brush.h"
#include <QTime>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  canvas = new GLWidget(this);
  setCentralWidget(canvas);

  // temporary to prevent crashing until fully implemented
  std::shared_ptr<Brush> brush(new Brush("function onDrag(x,y){drawPoint(x, y, 0x00FFFF);};"));
  brushes["bumblebee"] = brush;
  canvas->setBrush(brush);

  highlighter = new Highlighter(ui->jsTextEdit->document());

  setWindowTitle(tr("Qrush!"));

  ui->jsConsoleTextEdit->setText("");

  connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()));
  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNewBrush()));

  log("Ready :)");
}

MainWindow::~MainWindow()
{
  delete ui;
  delete highlighter;
}

// Todo make this available to V8/Brush
void MainWindow::log(const QString &message)
{
  QTime time;
  ui->jsConsoleTextEdit->append("["+time.currentTime().toString()+"] "+message);
}

// TODO save into file, then reparse
void MainWindow::saveButtonClicked()
{
  log("save");
  log(ui->jsTextEdit->toPlainText());
}

void MainWindow::addNewBrush()
{
  log("new Brush");

  // make a new input dialog
  bool ok;
  QString text = QInputDialog::getText(this, tr("New Brush"),
      tr("Brush name:"), QLineEdit::Normal, "", &ok);

  if (ok && !text.isEmpty()){
    log(text);
    // TODO: get script from input and put into file, then reparse files
    std::shared_ptr<Brush> brush(new Brush("function onDrag(x,y){drawPoint(x, y);};"));
    brushes[text.toStdString()] = brush;
    canvas->setBrush(brush);
    // TODO add brush to combo box
  }


}
