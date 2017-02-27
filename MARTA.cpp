#include "MARTA.h"
#include "CppRTA.h"

void* initMARTA(uint16_t iBlockLen, uint16_t iNumBuffers)
{
    CppRTA *myCppRTA;

    try {myCppRTA = new CppRTA(iBlockLen, iNumBuffers);}
    catch(std::exception &fault)
    {
        myCppRTA->setErrMsg("\nReal time audio class initialization: ");
        myCppRTA->appendErrMsg(fault.what());
        return NULL;
    }
    return myCppRTA;
}

int inStream(void *myPtrToMARTA, int16_t iInDeviceNr, uint32_t iFs, uint32_t iNumChans)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;

    myCppRTA->setErrMsg("\nReal time audio class input stream init: ");

    try {myCppRTA->inStream(iInDeviceNr, iFs, iNumChans);}
    catch(std::exception &fault)
    {
        myCppRTA->appendErrMsg(std::string(fault.what()));
        return -1;
    }
    myCppRTA->appendErrMsg("No error.");
    return 0;
}

int outStream(void *myPtrToMARTA, int16_t iOutDeviceNr, uint32_t iFs, uint32_t iNumChans)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;

    myCppRTA->setErrMsg("\nReal time audio class output stream init: ");

    try {myCppRTA->outStream(iOutDeviceNr, iFs, iNumChans);}
    catch(std::exception &fault)
    {
        myCppRTA->appendErrMsg(std::string(fault.what()));
        return -1;
    }
    myCppRTA->appendErrMsg("No error.");
    return 0;
}

int duplex(void *myPtrToMARTA, int16_t iDuplexDeviceNr, uint32_t iFs, uint32_t iNumChans)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;

    myCppRTA->setErrMsg("\nReal time audio class duplex stream init: ");

    try {myCppRTA->duplex(iDuplexDeviceNr, iFs, iNumChans);}
    catch(std::exception &fault)
    {
        myCppRTA->appendErrMsg(std::string(fault.what()));
        return -1;
    }
    myCppRTA->appendErrMsg("No error.");
    return 0;
}

int streamToFile(void *myPtrToMARTA, const char* sFileName, int16_t iInDeviceNr,
             uint32_t iFs, uint32_t iNumChans, uint32_t iFormat)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;

    try {myCppRTA->streamToFile(std::string(sFileName), iInDeviceNr, iFs, iNumChans, iFormat);}
    catch(std::exception &fault)
    {
        myCppRTA->setErrMsg("\nReal time audio class stream to file init: ");
        myCppRTA->appendErrMsg(fault.what());
        return -1;
    }
    myCppRTA->appendErrMsg("No error.");
    return 0;
}

int streamFromFile(void *myPtrToMARTA, const char* sFileName, int16_t iOutDeviceNr)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;

    try {myCppRTA->streamFromFile(std::string(sFileName), iOutDeviceNr);}
    catch(std::exception &fault)
    {
        myCppRTA->setErrMsg("\nReal time audio class stream from file init: ");
        myCppRTA->appendErrMsg(fault.what());
        return -1;
    }
    myCppRTA->appendErrMsg("No error.");
    return 0;
}

int seek(void *myPtrToMARTA, double fNormOffset)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        myCppRTA->seek(fNormOffset);
    return 0;
}

int setGain(void *myPtrToMARTA, double fGainLog)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        myCppRTA->setGain(fGainLog);
    return 0;
}

int toggleLimiter(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        myCppRTA->toggleLimiter();
    return 0;
}

int toggleMono(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        myCppRTA->toggleMono();
    return 0;
}

uint16_t getNumDevices(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return 0xFFFF;
    else
        return myCppRTA->getNumDevices();
}

double getLimitGain(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        return myCppRTA->getLimitGain();
}

double getNormPosition(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        return myCppRTA->getNormPosition();
}

int getCurrentDataPos(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        return myCppRTA->getCurrentDataPos();
}

int getDataBuf(void *myPtrToMARTA, double vData[], uint16_t iBufPos)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        return myCppRTA->getDataBuf(vData, iBufPos);
}

int setDataBuf(void *myPtrToMARTA, double vData[], uint16_t iBufPos)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        return myCppRTA->setDataBuf(vData, iBufPos);
}

unsigned int getStdOutDev(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return 0xFFFF;
    else
        return myCppRTA->getStdOutDev();
}

unsigned int getStdDuplexDev(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return 0xFFFF;
    else
        return myCppRTA->getStdDuplexDev();
}

uint32_t getSampRate(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return 0;
    else
         return myCppRTA->getSampRate();
}

uint16_t getNumChans(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return 0;
    else
        return myCppRTA->getNumChans();
}

int isInitialized(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;
    else
        return ((int)myCppRTA->isInitialized())*2-1;
}

const char* getDeviceInfoStr(void *myPtrToMARTA, uint16_t iDevNr)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return NULL;
    else
        return myCppRTA->getDeviceInfoStr(iDevNr)->c_str();
}

const char* getActErrMsg(void *myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return NULL;
    else
        return myCppRTA->getActErrMsg()->c_str();
}

int releaseMARTA(void* myPtrToMARTA)
{
    CppRTA *myCppRTA = (CppRTA*) myPtrToMARTA;

    if (myCppRTA == NULL)
        return -0x7FFF;

    delete myCppRTA;
    myCppRTA = NULL;
}
