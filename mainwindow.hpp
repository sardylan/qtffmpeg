#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QString>
#include <QProcess>

#include "ffprocess.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setStatusBarMessage(QString);
    
private:
    Ui::MainWindow *ui;
    void osProber();
    QStringList arguments;
    int can_run;
    ffProcess* proc;

private slots:
    void chooseInput();
    void chooseOutput();
    void chooseFFmpeg();
    void cmdConstructor();
    void runFFmpeg();

};

#endif // MAINWINDOW_HPP
