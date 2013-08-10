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
    QStringList arguments;
    int can_run;
    QProcess* proc;

    int ffTimeParser(QString);

private slots:
    void close();
    void about();
    void chooseInput();
    void chooseOutput();
    void chooseFFmpeg();
    void cmdPreConstructorOutput();
    void cmdPreConstructorCombo();
    void cmdConstructor();
    void runFFmpeg();
    void ffProcessChangeState();
    void ffProcessStarted();
    void ffProcessFinished();
    void ffProcessParseLog();

};

#endif // MAINWINDOW_HPP
