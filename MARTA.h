#ifndef MARTA_H
#define MARTA_H

#include <stdint.h>

#ifdef __cplusplus
	extern "C" {
#endif

void* initMARTA(uint16_t iBlockLen, uint16_t iNumBuffers);

int inStream(void *myPtrToMARTA, int16_t iInDeviceNr, uint32_t iFs, uint32_t iNumChans);

int outStream(void *myPtrToMARTA, int16_t iOutDeviceNr, uint32_t iFs, uint32_t iNumChans);

int duplex(void *myPtrToMARTA, int16_t iDuplexDeviceNr, uint32_t iFs, uint32_t iNumChans);

int streamToFile(void *myPtrToMARTA, const char* sFileName, int16_t iInDeviceNr,
             uint32_t iFs, uint32_t iNumChans, uint32_t iFormat);

int streamFromFile(void *myPtrToMARTA, const char* sFileName, int16_t iOutDeviceNr);

int seek(void *myPtrToMARTA, double fNormOffset);

int setGain(void *myPtrToMARTA, double fGainLog);

int toggleLimiter(void *myPtrToMARTA);

int toggleMono(void *myPtrToMARTA);

uint16_t getNumDevices(void *myPtrToMARTA);

double getLimitGain(void *myPtrToMARTA);

double getNormPosition(void *myPtrToMARTA);

int getCurrentDataPos(void *myPtrToCppRTA);

int getDataBuf(void *myPtrToCppRTA, double vData[], uint16_t iBufPos);

int setDataBuf(void *myPtrToCppRTA, double vData[], uint16_t iBufPos);

unsigned int getStdOutDev(void *myPtrToMARTA);

unsigned int getStdDuplexDev(void *myPtrToMARTA);

uint32_t getSampRate(void *myPtrToMARTA);

uint16_t getNumChans(void *myPtrToMARTA);

int isInitialized(void *myPtrToMARTA);

const char* getDeviceInfoStr(void *myPtrToMARTA, uint16_t iDevNr);

const char* getActErrMsg(void *myPtrToMARTA);

int releaseMARTA(void *myPtrToMARTA);

#ifdef __cplusplus
	}
#endif

#endif
