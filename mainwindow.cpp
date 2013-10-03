#include <QString>
#include <QFileInfo>
#include <QFileDialog>
#include <QProcess>
#include <QTextStream>
#include <QMessageBox>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::SeparateChannels);

    can_run = 0;

    connect(ui->buttonClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->buttonAbout, SIGNAL(clicked()), this, SLOT(about()));
    connect(ui->buttonInput, SIGNAL(clicked()), this, SLOT(chooseInput()));
    connect(ui->buttonOutput, SIGNAL(clicked()), this, SLOT(chooseOutput()));
    connect(ui->buttonFFmpeg, SIGNAL(clicked()), this, SLOT(chooseFFmpeg()));

    connect(ui->buttonRun, SIGNAL(clicked()), this, SLOT(runFFmpeg()));

    connect(ui->lineInput, SIGNAL(textChanged(QString)), this, SLOT(cmdConstructor()));
    connect(ui->lineFFMpeg, SIGNAL(textChanged(QString)), this, SLOT(cmdConstructor()));
    connect(ui->lineOutput, SIGNAL(textChanged(QString)), this, SLOT(cmdPreConstructorOutput()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cmdPreConstructorCombo()));

    connect(proc, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(ffProcessChangeState()));
    connect(proc, SIGNAL(started()), this, SLOT(ffProcessStarted()));
    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ffProcessFinished()));
    connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(ffProcessParseLog()));

#ifdef Q_OS_WIN
    ui->lineFFMpeg->setText("ffmpeg.exe");
#else
    ui->lineFFMpeg->setText("/usr/bin/ffmpeg");
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::close()
{
    qApp->exit(0);
}

void MainWindow::about()
{
    QMessageBox::about(this,
                       "QtFFmpeg",
                       "<h1>QtFFmpeg 1.1.1</h1>"
                       "<h2>Simple QT GUI for FFmpeg</h2>"
                       "<h3>Copyright (C) 2013  Luca Cireddu<br />"
                       "sardylan@gmail.com<br />"
                       "<a href=\"http://qtffmpeg.thehellnet.org/\">http://qtffmpeg.thehellnet.org/</a></h3>"
                       "<p>This program is free software; you can redistribute it and/or<br />"
                       "modify it under the terms of the GNU General Public License<br />"
                       "as published by the Free Software Foundation; either version 2<br />"
                       "of the License, or (at your option) any later version.</p>"
                       "<p>This program is distributed in the hope that it will be useful,<br />"
                       "but WITHOUT ANY WARRANTY; without even the implied warranty of<br />"
                       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br />"
                       "GNU General Public License for more details.</p>"
                       "<p>You should have received a copy of the GNU General Public License<br />"
                       "along with this program; if not, write to the Free Software<br />"
                       "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.</p>"
                       "<p><a href=\"http://www.gnu.org/licenses/gpl-2.0.html\">http://www.gnu.org/licenses/gpl-2.0.html</a></p>"
                       "<hr />"
                       "<p><a href=\"http://qt-project.org/\">Qt Libraries</a> are licensed under the "
                       "<a href=\"http://www.gnu.org/licenses/gpl.html\">GNU GPLv3</a> "
                       "and the <a href=\"http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html\">GNU LGPLv2.1</a>.</p>"
                       "<p><a href=\"http://www.ffmpeg.org/\"FFmpeg</a> static build includes code from <a href=\"http://lame.sourceforge.net/\">LAME</a>.</p>"
                       "<p>Both are released under the <a href=\"http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html\">GNU LGPLv2.1</a>.</p>");
}

void MainWindow::chooseInput()
{
    QFileDialog diag;
    QStringList filters;
    QStringList filenames;

    filters << "All files (*.*)";

    diag.setAcceptMode(diag.AcceptOpen);
    diag.setFileMode(diag.ExistingFile);
    diag.setViewMode(diag.List);
    diag.setFilters(filters);

    diag.exec();

    filenames = diag.selectedFiles();

    if(filenames.count() > 0)
        ui->lineInput->setText(diag.selectedFiles().at(0));
    else
        ui->lineInput->setText("");
}

void MainWindow::chooseOutput()
{
    QFileDialog diag;
    QStringList filters;
    QStringList filenames;
    QFileInfo output;
    QString outputname;

    filters << "PCM Wave(*.wav)"
            << "MP3(*.mp3)";

    diag.setAcceptMode(diag.AcceptSave);
    diag.setFileMode(diag.AnyFile);
    diag.setViewMode(diag.List);
    diag.setFilters(filters);

    diag.exec();

    filenames = diag.selectedFiles();

    if(filenames.count() > 0) {
        outputname = diag.selectedFiles().at(0);
        output.setFile(outputname);

        if(output.suffix() == "wav" || output.suffix() == "mp3")
            ui->lineOutput->setText(outputname);
        else {
            outputname.append(".wav");
            ui->lineOutput->setText(outputname);
        }
    } else
        ui->lineOutput->setText("");
}

void MainWindow::chooseFFmpeg()
{
    QFileDialog diag;
    QStringList filters;
    QStringList filenames;

#ifdef Q_OS_WIN
    filters << "FFmpeg Executables(ffmpeg.exe)";
#else
    filters << "FFmpeg Executables(ffmpeg)";
#endif

    diag.setAcceptMode(diag.AcceptOpen);
    diag.setFileMode(diag.ExistingFile);
    diag.setViewMode(diag.List);
    diag.setFilters(filters);

    diag.exec();

    filenames = diag.selectedFiles();

    if(filenames.count() > 0)
        ui->lineFFMpeg->setText(diag.selectedFiles().at(0));
    else
        ui->lineFFMpeg->setText("");
}

void MainWindow::cmdPreConstructorOutput()
{
    QFileInfo output;
    output.setFile(ui->lineOutput->text());

    if(output.suffix() == "wav") {
        if(ui->comboBox->currentIndex() != 0)
            disconnect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cmdPreConstructorCombo()));
            ui->comboBox->setCurrentIndex(0);
            connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cmdPreConstructorCombo()));
    }

    if(output.suffix() == "mp3") {
        if(ui->comboBox->currentIndex() != 1 || ui->comboBox->currentIndex() != 2 || ui->comboBox->currentIndex() != 3)
            disconnect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cmdPreConstructorCombo()));
            ui->comboBox->setCurrentIndex(1);
            connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(cmdPreConstructorCombo()));
    }

    cmdConstructor();
}

void MainWindow::cmdPreConstructorCombo()
{
    QFileInfo output;
    QString outputname;
    QString outputnamecropped;

    outputname = ui->lineOutput->text();
    output.setFile(outputname);

    if(ui->comboBox->currentIndex() == 0) {
        if(output.suffix() != "wav") {
            outputnamecropped = outputname.left(outputname.lastIndexOf("."));
            outputnamecropped.append(".wav");

            disconnect(ui->lineOutput, SIGNAL(textChanged(QString)), this, SLOT(cmdPreConstructorOutput()));
            ui->lineOutput->setText(outputnamecropped);
            connect(ui->lineOutput, SIGNAL(textChanged(QString)), this, SLOT(cmdPreConstructorOutput()));
        }
    }

    if(ui->comboBox->currentIndex() == 1 || ui->comboBox->currentIndex() == 2 || ui->comboBox->currentIndex() == 3) {
        if(output.suffix() != "mp3") {
            outputnamecropped = outputname.left(outputname.lastIndexOf("."));
            outputnamecropped.append(".mp3");

            disconnect(ui->lineOutput, SIGNAL(textChanged(QString)), this, SLOT(cmdPreConstructorOutput()));
            ui->lineOutput->setText(outputnamecropped);
            connect(ui->lineOutput, SIGNAL(textChanged(QString)), this, SLOT(cmdPreConstructorOutput()));
        }
    }

    cmdConstructor();
}

void MainWindow::cmdConstructor()
{
    QFileInfo input;
    QFileInfo output;
    QFileInfo ffmpeg;

    input.setFile(ui->lineInput->text());
    output.setFile(ui->lineOutput->text());
    ffmpeg.setFile(ui->lineFFMpeg->text());

    arguments.clear();

    if(input.exists() == true && ffmpeg.exists() == true && (output.suffix() == "wav" || output.suffix() == "mp3")) {
        arguments.append(ui->lineFFMpeg->text());
        arguments.append("-i");
        arguments.append(ui->lineInput->text());
        arguments.append("-codec:a");

        if(ui->comboBox->currentIndex() == 0) {
            arguments.append("pcm_s16le");
            arguments.append("-ac");
            arguments.append("2");
            arguments.append("-ar");
            arguments.append("44100");
        }

        if(ui->comboBox->currentIndex() == 1) {
            arguments.append("libmp3lame");
            arguments.append("-b:a");
            arguments.append("320k");
            arguments.append("-ac");
            arguments.append("2");
            arguments.append("-ar");
            arguments.append("44100");
        }

        if(ui->comboBox->currentIndex() == 2) {
            arguments.append("libmp3lame");
            arguments.append("-b:a");
            arguments.append("192k");
            arguments.append("-ac");
            arguments.append("2");
            arguments.append("-ar");
            arguments.append("44100");
        }

        if(ui->comboBox->currentIndex() == 3) {
            arguments.append("libmp3lame");
            arguments.append("-b:a");
            arguments.append("96k");
            arguments.append("-ac");
            arguments.append("1");
            arguments.append("-ar");
            arguments.append("44100");
        }

        arguments.append("-y");
        arguments.append(ui->lineOutput->text());

        can_run = 1;
    } else {
        can_run = 0;
    }

    ui->textCmd->setText(arguments.join(" "));
}

void MainWindow::runFFmpeg()
{
    QString program;
    QStringList args;
    int i;

    if(can_run == 1) {
        program = arguments.at(0);

        for(i=0; i < arguments.count()-1; i++)
            args.append(arguments.at(i+1));

        if(proc->state() == QProcess::NotRunning) {
            proc->start(program, args, QIODevice::ReadOnly);
            //proc->deleteLater();
            //proc->waitForStarted();
        } else {
            setStatusBarMessage("FFmpeg already running!!!");
        }
    } else {
        setStatusBarMessage("Please provide all informations!!!");
    }
}

void MainWindow::setStatusBarMessage(QString text)
{
    ui->statusBar->showMessage(text, 10000);
}

void MainWindow::ffProcessChangeState()
{
    QProcess::ProcessState state;
    QString message;

    message.clear();

    state = proc->state();

    if(state == QProcess::NotRunning) {
        message = "ERROR!!! FFmpeg is not running!!!";
    } else if(state == QProcess::Starting) {
        message = "ERROR!!! FFmpeg has not yet been invoked!!!";
    } else {
        message = "FFmpeg is running...";
    }

    setStatusBarMessage(message);
}

void MainWindow::ffProcessStarted()
{
    ui->buttonRun->setEnabled(false);
    setStatusBarMessage("Running FFmpeg... Please wait...");
}

void MainWindow::ffProcessFinished()
{
    ui->buttonRun->setEnabled(true);
    ui->progressBar->setValue(ui->progressBar->maximum());
    setStatusBarMessage("Finished!!");
}

void MainWindow::ffProcessParseLog()
{
    QByteArray raw;
    QStringList lines;
    QString line;

    QStringList elems;
    QString duration;

    QString temp;

    raw = proc->readAllStandardError();
    lines = QString(raw).split("\n");

    foreach(line, lines) {
        if(line.contains("Duration")) {
            while(line.startsWith(" "))
                line.remove(0, 1);

            duration = line.split(" ").at(1);
            duration.chop(4);
            ui->progressBar->setMaximum(ffTimeParser(duration));
        }

        if(line.contains("time")) {
            while(line.startsWith(" "))
                line.remove(0, 1);

            elems = line.split(' ');

            foreach(temp, elems){
                if(temp.startsWith("time=")) {
                    duration = temp.split("=").at(1);
                    duration.chop(3);
                    ui->progressBar->setValue(ffTimeParser(duration));
                }
            }
        }
    }
}

int MainWindow::ffTimeParser(QString input)
{
    int ret;
    QStringList split;

    ret = 0;

    if(input.length() > 0) {
        split = input.split(":");
        ret = (split.at(0).toInt() * 3600) + (split.at(1).toInt() * 60) + split.at(2).toInt();
    }

    return ret;
}
