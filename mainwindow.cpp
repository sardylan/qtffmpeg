#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MainWindow::connect(ui->buttonInput, SIGNAL(clicked()), this, SLOT(chooseInput()));
    MainWindow::connect(ui->buttonOutput, SIGNAL(clicked()), this, SLOT(chooseOutput()));
    MainWindow::connect(ui->buttonFFmpeg, SIGNAL(clicked()), this, SLOT(chooseFFmpeg()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::chooseInput()
{
    ui->lineInput->setText("Ciao");
}

void MainWindow::chooseOutput()
{
    ui->lineOutput->setText("Ciao");
}

void MainWindow::chooseFFmpeg()
{
    ui->lineFFMpeg->setText("Ciao");
}
