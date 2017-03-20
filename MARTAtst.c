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
