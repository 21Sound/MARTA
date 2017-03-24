/*----------------------------------------------------------------------------*\
Header for mainWindow.cpp

Author: (c) Hagen Jaeger    January 2017 - Now
\*----------------------------------------------------------------------------*/

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

//--------------------- License ------------------------------------------------

// Copyright (c) 2017 Hagen Jaeger

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files
// (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge,
// publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
