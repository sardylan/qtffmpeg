#include <QObject>
#include <QProcess>

#include "ffprocess.hpp"
#include "mainwindow.hpp"

ffProcess::ffProcess(QObject *parent) :
    QObject(parent)
{
    process = new QProcess(this);

    connect(process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(stateChanged(QProcess::ProcessState)));
}

ffProcess::~ffProcess()
{
    if(process->state() != QProcess::NotRunning)
        process->close();
}

void ffProcess::start(QString program, QStringList args)
{
    process->start(program, args);
}

void ffProcess::stateChanged(QProcess::ProcessState state)
{
    if(state == QProcess::NotRunning)
        MainWindow::setStatusBarMessage("ERROR!!! ffmpeg is not running!!!");
    else if(state == QProcess::Starting)
        MainWindow::setStatusBarMessage("ERROR!!! FFmpeg has not yet been invoked!!!");
    else
        MainWindow::setStatusBarMessage("FFmpeg is running...");
}
