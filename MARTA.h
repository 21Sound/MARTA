/*----------------------------------------------------------------------------*\
Header for MARTA.cpp

Author: (c) Hagen Jaeger    January 2017 - Now
\*----------------------------------------------------------------------------*/

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

//------------------------------------------------------------------------------
