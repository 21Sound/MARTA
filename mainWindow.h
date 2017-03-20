#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define NPLOTDIMS 2 //2d plot

#include <QMainWindow>
#include <QCoreApplication>
#include <QDropEvent>
#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <QPushButton>
#include <QTextEdit>
#include <cmath>
#include "CppRTA.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void playHandle();

private:
    void dragEnterEvent(QDragEnterEvent *dragEnterEventData);
    void dropEvent(QDropEvent *dropEventData);

    QPushButton playButton;

    QTextEdit statusTxt;

    CppRTA *myRTA;

    bool bPlay;

    std::string sAudioFile;

    uint16_t iNumBuffers, iBlockLenIO, iDeviceNr;
};

void lowPass(double *buffer, int numChannels, int numFrames);

#endif // MAINWINDOW_H
