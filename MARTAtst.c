#include <stdio.h>
#include <unistd.h>
#include "MARTA.h"

#define BLOCKLEN 512
#define NUM_BUFFERS 10
#define NUM_DSP_CHANS 2

void DSP(void* myPtrToMARTA)
{
    static int iOldBufPos = 0;
    int iNewBufPos, iBufCnt;
    static double vState[NUM_DSP_CHANS]; //low pass state
    double vData[NUM_DSP_CHANS*BLOCKLEN]; //our processing memory

    iNewBufPos = getCurrentDataPos(myPtrToMARTA);
    iBufCnt = iOldBufPos;

    while (iBufCnt!=iNewBufPos)
    {
        getDataBuf(myPtrToMARTA, vData, iBufCnt);
        for (int iSampCnt=0; iSampCnt<(NUM_DSP_CHANS*BLOCKLEN); iSampCnt+=NUM_DSP_CHANS)
        {
            for (int iChanCnt=0; iChanCnt<NUM_DSP_CHANS; iChanCnt++)
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

    if((myPtrToMARTA =initMARTA(BLOCKLEN, NUM_BUFFERS))  == NULL)
        return -1;

    //if (duplex(myPtrToMARTA, -1, 0, 0) < 0) return -2;

    //if (streamToFile(myPtrToMARTA, "C:/Users/ha1044/Desktop/tmpWrite.wav", -1, 0, 0, 0) < 0) return -2;

    if (streamFromFile(myPtrToMARTA, "D:/Qt/QtProjects/MARTA/MATLAB/Philip George - Wish You Were Mine.mp3", -1) < 0) return -2;

    for (int iWatchdog = 0; iWatchdog < 1000; iWatchdog++) //1000*0.01 sec play time
    {
        DSP(myPtrToMARTA); //example: simple lowpass first order
        usleep(10000);
    }

    releaseMARTA(myPtrToMARTA);

    printf("\n"); return 0;
}


