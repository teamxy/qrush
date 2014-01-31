#ifndef QRUSH_MAINWINDOW_H_
#define QRUSH_MAINWINDOW_H_

#include <QMainWindow>
#include "GLWidget.h"
#include <unordered_map>

#include "Highlighter.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void log(const QString &message);
    void logError(const QString &message);

  public slots:
    void saveButtonClicked();
    void addNewBrush();
    void brushChanged(QString brushName);

  private:
    Ui::MainWindow *ui;
    Highlighter* highlighter;
    GLWidget* canvas;
};

#endif // QRUSH_MAINWINDOW_H_
