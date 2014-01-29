#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

  private:
    Ui::MainWindow *ui;
    Highlighter* highlighter;
    std::unordered_map<std::string, std::shared_ptr<Brush>> brushes;
    GLWidget* canvas;
};

#endif // MAINWINDOW_H
