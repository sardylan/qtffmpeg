#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QString>
#include <QProcess>

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
    QProcess* proc;

private slots:
    void chooseInput();
    void chooseOutput();
    void chooseFFmpeg();
    void cmdConstructor();
    void runFFmpeg();
    void ffProcessChangeState();
    void ffProcessStarted();
    void ffProcessFinished();

};

#endif // MAINWINDOW_HPP
