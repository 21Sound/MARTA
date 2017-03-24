/*----------------------------------------------------------------------------*\
Test routine for the C-Wrapper of the C++ real time audio class. Lowpassfilters
the stream data.

Author: (c) Uwe Simmer, Hagen Jaeger    January 2017 - Now
\*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <unistd.h>
#include "MARTA.h"

#define BLOCKLEN 512
#define NUM_BUFFERS 10
#define NUM_DSP_CHANS 2

void DSP(void* myPtrToMARTA)
{
    static int iOldBufPos = 0;
    int iNewBufPos, iBufCnt, iChanCnt, iSampCnt;
    static double vState[NUM_DSP_CHANS]; //low pass state
    double vData[NUM_DSP_CHANS*BLOCKLEN]; //our processing memory

    iNewBufPos = getCurrentDataPos(myPtrToMARTA);
    iBufCnt = iOldBufPos;

    while (iBufCnt!=iNewBufPos)
    {
        getDataBuf(myPtrToMARTA, vData, iBufCnt);
        for (iSampCnt=0; iSampCnt<(NUM_DSP_CHANS*BLOCKLEN); iSampCnt+=NUM_DSP_CHANS)
        {
            for (iChanCnt=0; iChanCnt<NUM_DSP_CHANS; iChanCnt++)
            {
                vData[iSampCnt+iChanCnt] = 0.05*vData[iSampCnt+iChanCnt] + 0.95*vState[iChanCnt];
                vState[iChanCnt] = vData[iSampCnt+iChanCnt];
            }
        }
        setDataBuf(myPtrToMARTA, vData, iBufCnt);
        iBufCnt++; iBufCnt%=NUM_BUFFERS;
    }
    iOldBufPos = iNewBufPos;
}


int main()
{
    void* myPtrToMARTA;
    int iCnt, iWatchDog;
    int iDeviceNr = -1; //-1 means std device
    const char* sFilePath = "D:/Qt/QtProjects/VARy/build-VARy-Desktop_Qt_5_7_0_MinGW_32bit-Release/testFiles/HotelCaliforniaSnippet.wav";

    if((myPtrToMARTA = initMARTA(BLOCKLEN, NUM_BUFFERS))  == NULL)
        return -1;

    for (iCnt = 0; iCnt<getNumDevices(myPtrToMARTA); iCnt++)
        printf("Device %i: %s \n", iCnt, getDeviceInfoStr(myPtrToMARTA, iCnt));

    if (streamFromFile(myPtrToMARTA, sFilePath, iDeviceNr) < 0) return -2;

    for (iWatchDog = 0; iWatchDog < 1000; iWatchDog++) //1000*0.01 sec play time
    {
        DSP(myPtrToMARTA); //example: simple lowpass first order
        usleep(10000);
    }

    releaseMARTA(myPtrToMARTA);

    printf("\n"); return 0;
}

//--------------------- License ------------------------------------------------

// Copyright (c) 2017 Uwe Simmer, Hagen Jaeger

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
