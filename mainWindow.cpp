/*----------------------------------------------------------------------------*\
Small GUI example to use CppRTA with Qt to build a media player a.s.o.

Author: (c) Hagen Jaeger    January 2017 - Now
\*----------------------------------------------------------------------------*/

#include "mainWindow.h"
#include "CppRTA.h"

#define BLOCKLEN 128
#define NUM_BUFFERS 1

void lowPass(double *buffer, int numChannels, int numFrames)
{
    static double State[2];
    int iChanCnt, iSampCnt;

    for (iSampCnt = 0; iSampCnt<(numChannels*numFrames); iSampCnt += numChannels)
    {
        for (iChanCnt = 0; iChanCnt<numChannels; iChanCnt++)
        {
            buffer[iSampCnt + iChanCnt] = 0.05*buffer[iSampCnt + iChanCnt] + 0.95*State[iChanCnt];
            State[iChanCnt] = buffer[iSampCnt + iChanCnt];
        }
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setAcceptDrops(true);
    QPalette tmpPal = this->palette();

    this->myRTA = NULL;
    this->sAudioFile = "tstAudio.wav";
    this->iNumBuffers = 1;
    this->iBlockLenIO = 1024;
    this->iDeviceNr = -1;
    this->bPlay = false;

    playButton.setParent(this);
    playButton.setText("Paused");
    playButton.move(QPoint(10,15));
    playButton.resize(QSize(100,50));
    playButton.setPalette(tmpPal);

    statusTxt.setParent(this);
    statusTxt.setText("No Messages");
    statusTxt.move(QPoint(10,110));
    statusTxt.resize(QSize(380,80));
    statusTxt.setPalette(this->palette());
    statusTxt.viewport()->setAutoFillBackground(false);
    statusTxt.setReadOnly(true);

    connect(&playButton, SIGNAL(clicked()), this, SLOT(playHandle()));
}

void MainWindow::playHandle()
{
    bPlay = !bPlay;

    if (bPlay)
    {
        playButton.setText("Playing...");
        statusTxt.setText("\nAudio Playback started: Playing ");
        statusTxt.append(QString::fromStdString(sAudioFile));

        if (myRTA != NULL)
        {
            delete myRTA;
            myRTA = NULL;
        }

        myRTA = new CppRTA(iBlockLenIO, iNumBuffers, lowPass);

        try {myRTA->streamFromFile(sAudioFile, iDeviceNr);}
        catch(std::exception &fault)
        {
            statusTxt.append("\nReal time audio class: ");
            statusTxt.append(fault.what());
            playHandle();
            return;
        }
    }
    else
    {
        playButton.setText("Paused");
        statusTxt.append("\nAudio Playback paused.");

        if (myRTA != NULL)
        {
            delete myRTA;
            myRTA = NULL;
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *dragEnterEventData)
{
    bool acceptDrop = true;
    QList<QUrl> fileNames = dragEnterEventData->mimeData()->urls();

    if(fileNames.size() > 1)  acceptDrop = false;
    if(acceptDrop) dragEnterEventData->accept();
}

void MainWindow::dropEvent(QDropEvent *dropEventData)
{
    QString tmpStr, combinedStr;
    QFileInfo fileNameExtractor;

    QList<QUrl> fileNames = dropEventData->mimeData()->urls();

    tmpStr = fileNames[0].toLocalFile();

    sAudioFile = tmpStr.toStdString();
    if (myRTA != NULL)
    {
        bPlay = true;
        playHandle();
        try {myRTA->streamFromFile(sAudioFile);}
        catch(std::exception &fault)
        {
            statusTxt.setText(fault.what());
            myRTA = NULL;
            return;
        }
        delete myRTA;
        myRTA = NULL;
    }
}

MainWindow::~MainWindow()
{
    if (bPlay)
        playHandle();

    if (myRTA != NULL)
    {
        delete myRTA;
        myRTA = NULL;
    }
}

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
