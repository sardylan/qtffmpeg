#include <QString>
#include <QFileInfo>
#include <QFileDialog>
#include <QProcess>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    proc = new QProcess(this);

    can_run = 0;

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

    osProber();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::osProber()

{
#ifdef Q_OS_LINUX
    ui->lineFFMpeg->setText("/usr/bin/ffmpeg");
#endif
#ifdef Q_OS_WIN
    ui->lineFFMpeg->setText("ffmpeg.exe");
#endif
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

#ifdef Q_OS_LINUX
    filters << "FFmpeg Executables(ffmpeg)";
#endif
#ifdef Q_OS_WIN
    filters << "FFmpeg Executables(ffmpeg.exe)";
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
            proc->start(program, args);
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
    setStatusBarMessage("Finished!!");
}

void MainWindow::ffProcessParseLog()
{
    ui->progressBar->setValue(4);
}
