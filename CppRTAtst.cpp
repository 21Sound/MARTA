#include <chrono>
#include <thread>
#include <stdio.h>
#include "CppRTA.h"

#define BLOCKLEN 128
#define NUM_BUFFERS 1

int effect = 1;

void lowPass(double *buffer, int numChannels, int numFrames)
{
    static double State[2];
    int iChanCnt, iSampCnt;

    if (effect == 0)
        return;

    for (iSampCnt = 0; iSampCnt<(numChannels*numFrames); iSampCnt += numChannels)
    {
        for (iChanCnt = 0; iChanCnt<numChannels; iChanCnt++)
        {
            buffer[iSampCnt + iChanCnt] = 0.05*buffer[iSampCnt + iChanCnt] + 0.95*State[iChanCnt];
            State[iChanCnt] = buffer[iSampCnt + iChanCnt];
        }
    }
}

int main()
{
    int count, watchDog;
    int deviceNr = -1;
    const char* sFilePath = "D:/My Music/Cari Lekebusch +TecHouse/Julian Jeweil – Shaka (Original Mix) [ClapCrate.com].mp3";

    CppRTA *cppRTA = new CppRTA(BLOCKLEN, NUM_BUFFERS, lowPass);

    for (count = 0; count<cppRTA->getNumDevices(); count++)
        printf("Device %i: %s \n", count, cppRTA->getDeviceInfoStr(count)->c_str());

    cppRTA->streamFromFile(sFilePath, deviceNr);

    for (watchDog = 0; watchDog < 1000; watchDog++) //1000*0.01 sec play time
    {
        if (watchDog == 500)
            effect = 0;

        std::this_thread::sleep_for(std::chrono::microseconds(10000));
    }

    delete cppRTA;

    printf("\n");
    return 0;
}


//------------------------------------------------------------------------------
