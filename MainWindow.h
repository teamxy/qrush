#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

public slots:
    void saveButtonClicked();

private:
    Ui::MainWindow *ui;
    Highlighter* highlighter;
};

#endif // MAINWINDOW_H
