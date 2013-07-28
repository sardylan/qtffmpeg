#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    void osProber();

private slots:
    void chooseInput();
    void chooseOutput();
    void chooseFFmpeg();
    void cmdConstructor();

};

#endif // MAINWINDOW_HPP
