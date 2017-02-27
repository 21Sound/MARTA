#ifndef CPPRTA_H
#define CPPRTA_H

#include "stdint.h"
#include "portaudio.h"
#include "mpg123.h"
#include "sndfile.h"

#ifdef __cplusplus

#include <string>
#include <vector>

class CppRTA
{

public:

    CppRTA(uint16_t iBlockLen = 512, uint32_t iNumBuffers = 10);

    void inStream(int16_t iInDevNr, uint32_t iFs, uint32_t iNumChans);

    void outStream(int16_t iOutDevNr, uint32_t iFs, uint32_t iNumChans);

    void duplex(int16_t iDuplexDeviceNr = -1, uint32_t iFs = 0, uint32_t iNumChans = 0);

    void streamToFile(std::string sFileName, int16_t iInDeviceNr = -1,
                 uint32_t iFs = 0, uint32_t iNumChans = 0,
                 uint32_t iFormat = (SF_FORMAT_WAV | SF_FORMAT_PCM_16));

    void streamFromFile(std::string sFileName, int16_t iOutDeviceNr = -1);

    void seek(double fNormOffset);

    void setGain(double fGainLog);
    void toggleLimiter(void) {bLimitEnabled = !bLimitEnabled;}
    void toggleMono(void) {bMono = !bMono;}
    inline uint16_t getNumDevices(void) {return iNumPortAudioDevices;}
    inline double getLimitGain(void) {return fLimitGain;}
    inline double getNormPosition(void) {return ((double) iPosCnt/iNumSamps);}
    inline uint16_t getCurrentDataPos(void) {return iBufCnt;}
    inline unsigned int getStdOutDev(void) {return iStdOutDev;}
    inline unsigned int getStdDuplexDev(void) {return iStdDuplexDev;}
    inline uint32_t getSampRate(void) {return iFs;}
    inline uint16_t getNumChans(void) {return iNumChans;}
    inline bool isInitialized(void) {return bInitFlag;}
    std::string* getDeviceInfoStr(uint16_t iDevNr);
    std::string* getActErrMsg(void) {return &this->sErrMsg;}
    void setErrMsg(std::string errMsg) {this->sErrMsg = errMsg;}
    void appendErrMsg(std::string errMsg) {this->sErrMsg.append(errMsg);}

    inline int getDataBuf(double vData[], uint16_t iBufPos)
    {
        if (iBufPos >= iNumBuffers)
            return -1;
        for (uint16_t iSampCnt = 0; iSampCnt < iBufLen; iSampCnt++)
             vData[iSampCnt] = mInOutBuf[iBufPos][iSampCnt];
        return 0;
    }

    inline int setDataBuf(double vData[], uint16_t iBufPos)
    {
        if (iBufPos >= iNumBuffers)
            return -1;
        for (uint16_t iSampCnt = 0; iSampCnt < iBufLen; iSampCnt++)
            mInOutBuf[iBufPos][iSampCnt] = vData[iSampCnt];
        return 0;
    }

    ~CppRTA(void);

protected:

    static int inStreamCallback(const void *vInputBuffer, void *vOutputBuffer,
                                    unsigned long iFramesPerBuffer,
                                    const PaStreamCallbackTimeInfo* sTimeInfo,
                                    PaStreamCallbackFlags iStatusFlags,
                                    void *userData);

    static int outStreamCallback(const void *vInputBuffer, void *vOutputBuffer,
                                    unsigned long iFramesPerBuffer,
                                    const PaStreamCallbackTimeInfo* sTimeInfo,
                                    PaStreamCallbackFlags iStatusFlags,
                                    void *userData);

    static int duplexCallback(const void *vInputBuffer, void *vOutputBuffer,
                              unsigned long iFramesPerBuffer,
                              const PaStreamCallbackTimeInfo* sTimeInfo,
                              PaStreamCallbackFlags iStatusFlags,
                              void *userData);

    static int streamToFileCallback(const void *vInputBuffer, void *vOutputBuffer,
                                    unsigned long iFramesPerBuffer,
                                    const PaStreamCallbackTimeInfo* sTimeInfo,
                                    PaStreamCallbackFlags iStatusFlags,
                                    void *userData);

    static int streamFromFileCallback(const void *vInputBuffer, void *vOutputBuffer,
                                      unsigned long iFramesPerBuffer,
                                      const PaStreamCallbackTimeInfo* sTimeInfo,
                                      PaStreamCallbackFlags iStatusFlags,
                                      void *userData);

    void fastLimiter(double *vData);

    inline void increaseBufCnt(void) {iBufCnt++;}
    inline uint32_t getBufCnt(void) {return iBufCnt;}
    inline uint32_t getBufLen(void) {return iBufLen;}
    inline uint32_t getNumBuffers(void) {return iNumBuffers;}
    inline void resetBufCnt(void) {iBufCnt = 0;}

private:

    PaStream *paStream;
    const PaDeviceInfo *sDeviceInfo;
    SNDFILE *sndFileRW;
    SF_INFO sndFileInfo;
    mpg123_handle *mp3Read;
    std::vector< std::vector<double> > mInOutBuf;
    std::vector<int16_t> vInt16Buf;
    double fGain, fLimitGain;
    uint32_t iFs, iBlockLen, iNumSamps, iPosCnt;
    uint16_t iNumChans, iInDevNr, iBufCnt, iBufLen, iNumBuffers;
    uint16_t iNumPortAudioDevices, iStdInDev, iStdOutDev, iStdDuplexDev;
    bool bInitFlag, bLimitEnabled, bMono;
    std::string sErrMsg, sDeviceInfoStr;
};

#endif

#endif
