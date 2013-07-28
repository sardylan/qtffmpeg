#include <QString>
#include <QFileInfo>
#include <QFileDialog>

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

    MainWindow::connect(ui->lineInput, SIGNAL(textChanged(QString)), this, SLOT(cmdConstructor()));
    MainWindow::connect(ui->lineOutput, SIGNAL(textChanged(QString)), this, SLOT(cmdConstructor()));
    MainWindow::connect(ui->lineFFMpeg, SIGNAL(textChanged(QString)), this, SLOT(cmdConstructor()));

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
    QString output_params_common;
    QFileInfo input;
    QFileInfo output;
    QFileInfo ffmpeg;

    input.setFile(ui->lineInput->text());
    output.setFile(ui->lineOutput->text());
    ffmpeg.setFile(ui->lineFFMpeg->text());

    cmd.clear();
    output_params.clear();
    output_params_common.clear();

    output_params_common.append("-ac 2 -ar 44100");

    if(output.suffix() == "mp3")
        output_params.append("-codec:a libmp3lame -b:a 320k");

    if(output.suffix() == "wav")
        output_params.append("-codec:a pcm_s16le");

    if(output_params.isEmpty() == false && ffmpeg.exists() == true) {
        cmd.append("\"");
        cmd.append(ui->lineFFMpeg->text());
        cmd.append("\"");
        cmd.append(" ");

        cmd.append("-i ");
        cmd.append("\"");
        cmd.append(ui->lineInput->text());
        cmd.append("\"");
        cmd.append(" ");

        cmd.append(output_params);
        cmd.append(" ");

        cmd.append(output_params_common);
        cmd.append(" ");

        cmd.append("\"");
        cmd.append(ui->lineOutput->text());
        cmd.append("\"");
    }

    ui->textCmd->setText(cmd);
}
