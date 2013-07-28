#include <QString>
#include <QFileInfo>
#include <QFileDialog>
#include <QProcess>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "ffprocess.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    proc = new ffProcess();

    can_run = 0;

    connect(ui->buttonInput, SIGNAL(clicked()), this, SLOT(chooseInput()));
    connect(ui->buttonOutput, SIGNAL(clicked()), this, SLOT(chooseOutput()));
    connect(ui->buttonFFmpeg, SIGNAL(clicked()), this, SLOT(chooseFFmpeg()));

    connect(ui->buttonRun, SIGNAL(clicked()), this, SLOT(runFFmpeg()));

    connect(ui->lineInput, SIGNAL(textChanged(QString)), this, SLOT(cmdConstructor()));
    connect(ui->lineOutput, SIGNAL(textChanged(QString)), this, SLOT(cmdConstructor()));
    connect(ui->lineFFMpeg, SIGNAL(textChanged(QString)), this, SLOT(cmdConstructor()));

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
#ifdef Q_OS_WIN32
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

    filters << "PCM s16le (*.wav)(*.wav)"
            << "MP3 cbr 320k (*.mp3)(*.mp3)";

    diag.setAcceptMode(diag.AcceptSave);
    diag.setFileMode(diag.AnyFile);
    diag.setViewMode(diag.List);
    diag.setFilters(filters);

    diag.exec();

    filenames = diag.selectedFiles();

    if(filenames.count() > 0)
        ui->lineOutput->setText(diag.selectedFiles().at(0));
    else
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
#ifdef Q_OS_WIN32
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

void MainWindow::cmdConstructor()
{
    QString cmd;
    QString output_params;
    int format_defined;
    QFileInfo input;
    QFileInfo output;
    QFileInfo ffmpeg;

    input.setFile(ui->lineInput->text());
    output.setFile(ui->lineOutput->text());
    ffmpeg.setFile(ui->lineFFMpeg->text());

    format_defined = 0;

    cmd.clear();
    output_params.clear();
    arguments.clear();

    if(output.suffix() == "wav" || output.suffix() == "mp3")
        format_defined = 1;

    if(format_defined == 1 && ffmpeg.exists() == true) {
        cmd.clear();
        cmd.append("\"");
        cmd.append(ui->lineFFMpeg->text());
        cmd.append("\"");
        arguments.append(cmd);

        cmd.clear();
        cmd.append("-i ");
        cmd.append("\"");
        cmd.append(ui->lineInput->text());
        cmd.append("\"");
        arguments.append(cmd);

        cmd.clear();
        cmd.append(output_params);

        arguments.append("-codec:a");

        if(output.suffix() == "mp3") {
            arguments.append("libmp3lame");
            arguments.append("-b:a");
            arguments.append("320k");
        }

        if(output.suffix() == "wav") {
            arguments.append("pcm_s16le");
        }

        arguments.append("-ac");
        arguments.append("2");
        arguments.append("-ar");
        arguments.append("44100");

        cmd.clear();
        cmd.append("\"");
        cmd.append(ui->lineOutput->text());
        cmd.append("\"");
        arguments.append(cmd);

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

        proc->start(program, args);
    } else {
        setStatusBarMessage("Please provide all informations!!!");
    }
}

void MainWindow::setStatusBarMessage(QString text)
{
    ui->statusBar->showMessage(text, 10000);
}
