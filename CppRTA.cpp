#include <stdexcept>
#include <cmath>
#include "CppRTA.h"

CppRTA::CppRTA(uint16_t iBlockLen, uint32_t iNumBuffers)
{
    const PaDeviceInfo *sDeviceInfoTmp;
    PaError paErr = paNoError;

    this->paStream = NULL;
    this->sndFileRW = NULL;
    this->mp3Read = NULL;
    this->iNumBuffers = iNumBuffers;
    this->iBufCnt = 0;
    this->bInitFlag = false;
    this->iBlockLen = iBlockLen;
    this->iNumBuffers = iNumBuffers;
    this->iFs = 0;
    this->iNumChans = 0;
    this->iNumSamps = 0;
    this->iPosCnt = 0;
    this->fGain = 1.0;
    this->fLimitGain = 1.0;
    this->bLimitEnabled = true;
    this->bMono = false;
    this->sErrMsg.append("CppRTA initialization: Success. No error messages.");
    this->sDeviceInfoStr.append("No device info requested.");

    if (iBlockLen < 0x40)
        iBlockLen = 0x40;

    if (iNumBuffers > 0xFF)
        iNumBuffers  = 0xFF;

    paErr = Pa_Initialize();
    if(paErr != paNoError)
        throw std::runtime_error(std::string(Pa_GetErrorText(paErr)));

    iNumPortAudioDevices = Pa_GetDeviceCount();

    if (iNumPortAudioDevices < 1)
        throw std::runtime_error("No port audio devices covered.");

    iStdInDev = Pa_GetDefaultInputDevice();

    iStdOutDev = Pa_GetDefaultOutputDevice();

    iStdDuplexDev = 0;
    sDeviceInfoTmp = Pa_GetDeviceInfo(iStdDuplexDev);
    while (!((sDeviceInfoTmp->maxInputChannels > 1)
           && (sDeviceInfoTmp->maxInputChannels == sDeviceInfoTmp->maxOutputChannels))
           && (iStdDuplexDev < (iNumPortAudioDevices-1)))
    {
        iStdDuplexDev++;
        sDeviceInfoTmp = Pa_GetDeviceInfo(iStdDuplexDev);
    }
    if (iStdDuplexDev >= (iNumPortAudioDevices-1))
        iStdDuplexDev = 0xFFFF;
}

void CppRTA::inStream(int16_t iInDevNr, uint32_t iFs, uint32_t iNumChans)
{
    PaStreamParameters inParams;
    PaError paErr = paNoError;

    if(bInitFlag)
        throw std::runtime_error("Object is already in use (multiple stream functions called).");

    if(iStdInDev < 0)
        throw std::runtime_error("No input devices available.");

    if (iInDevNr >= iNumPortAudioDevices)
        throw std::invalid_argument("Input device index is greater than the number of devices.");
    else if (iInDevNr < 0)
        iInDevNr = iStdInDev;

    sDeviceInfo = Pa_GetDeviceInfo(iInDevNr);

    if (iFs < 1);
        iFs = sDeviceInfo->defaultSampleRate;

    if (iNumChans < 1);
        iNumChans = sDeviceInfo->maxInputChannels;

    iBufLen = iBlockLen*iNumChans;

    inParams.device = iInDevNr;
    inParams.channelCount = iNumChans;
    inParams.sampleFormat = paFloat32; //32 bit floating point output
    inParams.suggestedLatency = sDeviceInfo->defaultLowInputLatency;
    inParams.hostApiSpecificStreamInfo = NULL;

    paErr = Pa_OpenStream(&paStream, &inParams, NULL, iFs, iBlockLen, paNoFlag, inStreamCallback, this);
    if(paErr != paNoError)
        throw std::invalid_argument(std::string(Pa_GetErrorText(paErr)));

    mInOutBuf.resize(iNumBuffers);
    for (uint16_t iBufCntInt = 0; iBufCntInt < iNumBuffers; iBufCntInt++)
    {
        mInOutBuf.at(iBufCntInt).resize(iBufLen);
        for (uint16_t iSampCnt = 0; iSampCnt < iBufLen; iSampCnt++)
                mInOutBuf.at(iBufCntInt).at(iSampCnt) = 0.0;
    }

    paErr = Pa_StartStream(paStream);
    if(paErr != paNoError)
        throw std::runtime_error(std::string(Pa_GetErrorText(paErr)));

    this->iNumChans = iNumChans;
    this->iFs = iFs;
    this->bInitFlag = true;
}

void CppRTA::outStream(int16_t iOutDevNr, uint32_t iFs, uint32_t iNumChans)
{
    PaStreamParameters outParams;
    PaError paErr = paNoError;

    if(bInitFlag)
        throw std::runtime_error("Object is already in use (multiple stream functions called).");

    if(iStdOutDev < 0)
        throw std::runtime_error("No output devices available.");

    if (iOutDevNr >= iNumPortAudioDevices)
        throw std::invalid_argument("Input device index is greater than the number of devices.");
    else if (iOutDevNr < 0)
        iOutDevNr = iStdOutDev;

    sDeviceInfo = Pa_GetDeviceInfo(iOutDevNr);

    if (iFs < 1);
        iFs = sDeviceInfo->defaultSampleRate;

    if (iNumChans < 1);
        iNumChans = sDeviceInfo->maxInputChannels;

    iBufLen = iBlockLen*iNumChans;

    outParams.device = iOutDevNr;
    outParams.channelCount = iNumChans;
    outParams.sampleFormat = paFloat32; //32 bit floating point output
    outParams.suggestedLatency = sDeviceInfo->defaultLowOutputLatency;
    outParams.hostApiSpecificStreamInfo = NULL;

    paErr = Pa_OpenStream(&paStream, NULL, &outParams, iFs, iBlockLen, paNoFlag, outStreamCallback, this);
    if(paErr != paNoError)
        throw std::invalid_argument(std::string(Pa_GetErrorText(paErr)));

    mInOutBuf.resize(iNumBuffers);
    for (uint16_t iBufCntInt = 0; iBufCntInt < iNumBuffers; iBufCntInt++)
    {
        mInOutBuf.at(iBufCntInt).resize(iBufLen);
        for (uint16_t iSampCnt = 0; iSampCnt < iBufLen; iSampCnt++)
                mInOutBuf.at(iBufCntInt).at(iSampCnt) = 0.0;
    }

    paErr = Pa_StartStream(paStream);
    if(paErr != paNoError)
        throw std::runtime_error(std::string(Pa_GetErrorText(paErr)));

    this->iNumChans = iNumChans;
    this->iFs = iFs;
    this->bInitFlag = true;
}

void CppRTA::duplex(int16_t iDuplexDeviceNr, uint32_t iFs, uint32_t iNumChans)
{
    PaStreamParameters inParams, outParams;
    PaError paErr = paNoError;

    if(bInitFlag)
        throw std::runtime_error("Object is already in use (multiple stream functions called).");

    if(iStdDuplexDev == 0xFFFF)
        throw std::runtime_error("No duplex devices available.");

    if (iDuplexDeviceNr >= iNumPortAudioDevices)
        throw std::invalid_argument("Input device index is greater than the number of devices.");
    else if (iDuplexDeviceNr < 0)
        iDuplexDeviceNr = iStdDuplexDev;

    sDeviceInfo = Pa_GetDeviceInfo(iDuplexDeviceNr);

    if (iFs < 1);
        iFs = sDeviceInfo->defaultSampleRate;

    if (iNumChans < 1);
        iNumChans = sDeviceInfo->maxInputChannels;

    iBufLen = iBlockLen*iNumChans;

    inParams.device = iDuplexDeviceNr;
    inParams.channelCount = iNumChans;
    inParams.sampleFormat = paFloat32; //32 bit floating point output
    inParams.suggestedLatency = sDeviceInfo->defaultLowInputLatency;
    inParams.hostApiSpecificStreamInfo = NULL;

    outParams.device = iDuplexDeviceNr;
    outParams.channelCount = iNumChans;
    outParams.sampleFormat = paFloat32; //32 bit floating point output
    outParams.suggestedLatency = sDeviceInfo->defaultLowOutputLatency;
    outParams.hostApiSpecificStreamInfo = NULL;

    paErr = Pa_OpenStream(&paStream, &inParams, &outParams, iFs, iBlockLen, paNoFlag, duplexCallback, this);
    if(paErr != paNoError)
        throw std::invalid_argument(std::string(Pa_GetErrorText(paErr)));

    mInOutBuf.resize(iNumBuffers);
    for (uint16_t iBufCntInt = 0; iBufCntInt < iNumBuffers; iBufCntInt++)
    {
        mInOutBuf.at(iBufCntInt).resize(iBufLen);
        for (uint16_t iSampCnt = 0; iSampCnt < iBufLen; iSampCnt++)
                mInOutBuf.at(iBufCntInt).at(iSampCnt) = 0.0;
    }

    paErr = Pa_StartStream(paStream);
    if(paErr != paNoError)
        throw std::runtime_error(std::string(Pa_GetErrorText(paErr)));

    this->iNumChans = iNumChans;
    this->iFs = iFs;
    this->bInitFlag = true;
}

void CppRTA::streamToFile(std::string sFileName, int16_t iInDeviceNr,
                    uint32_t iFs, uint32_t iNumChans, uint32_t iFormat)
{
    PaStreamParameters inParams;
    PaError paErr = paNoError;
    int sndFileErr = 0;

    if(bInitFlag)
        throw std::runtime_error("Object is already in use (multiple stream functions called).");

    if (iInDeviceNr >= iNumPortAudioDevices)
        throw std::invalid_argument("Input device index is greater than the number of devices.");
    else if (iInDeviceNr < 0)
        iInDeviceNr = iStdInDev;

    if (iInDeviceNr < 0)
        throw std::runtime_error("No input devices available.");

    sDeviceInfo = Pa_GetDeviceInfo(iInDeviceNr);

    if (iFs < 1);
        iFs = sDeviceInfo->defaultSampleRate;

    if (iNumChans < 1);
        iNumChans = sDeviceInfo->maxInputChannels;

    if (iFormat<=0)
        iFormat = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    iBufLen = iBlockLen*iNumChans;

    sndFileInfo.channels = iNumChans;
    sndFileInfo.samplerate = iFs;
    sndFileInfo.format = iFormat;

    sndFileRW = sf_open(sFileName.c_str(), SFM_WRITE, &sndFileInfo);

    if(sndFileRW==NULL)
        throw std::runtime_error(std::string(sf_strerror(sndFileRW)));

    inParams.device = iInDeviceNr;
    inParams.channelCount = iNumChans;
    inParams.sampleFormat = paFloat32; //32 bit floating point output
    inParams.suggestedLatency = sDeviceInfo->defaultLowInputLatency;
    inParams.hostApiSpecificStreamInfo = NULL;

    paErr = Pa_OpenStream(&paStream, &inParams, NULL, iFs, iBlockLen, paNoFlag, streamToFileCallback, this);
    if(paErr != paNoError)
        throw std::invalid_argument(std::string(Pa_GetErrorText(paErr)));

    mInOutBuf.resize(iNumBuffers);
    for (uint16_t iBufCntInt = 0; iBufCntInt < iNumBuffers; iBufCntInt++)
    {
        mInOutBuf.at(iBufCntInt).resize(iBufLen);
        for (uint16_t iSampCnt = 0; iSampCnt < iBufLen; iSampCnt++)
                mInOutBuf.at(iBufCntInt).at(iSampCnt) = 0.0;
    }

    paErr = Pa_StartStream(paStream);
    if(paErr != paNoError)
        throw std::runtime_error(std::string(Pa_GetErrorText(paErr)));

    this->iNumChans = iNumChans;
    this->iFs = iFs;
    this->bInitFlag = true;
}

void CppRTA::streamFromFile(std::string sFileName, int16_t iOutDeviceNr)
{
    PaStreamParameters outParams;
    PaError paErr = paNoError;
    long iFsTmp;
    int iNumChansTmp, mp3Err = MPG123_OK, encodingID, sndFileErr = 0;
    std::string initErrStr;

    if(bInitFlag)
        throw std::runtime_error("Object is already in use (multiple stream functions called).");

    iNumPortAudioDevices = Pa_GetDeviceCount();

    if (iOutDeviceNr >= iNumPortAudioDevices)
        throw std::invalid_argument("Output device index is greater than the number of devices.");
    else if (iOutDeviceNr < 0)
        iOutDeviceNr = iStdOutDev;

    if (iOutDeviceNr < 0)
        throw std::runtime_error("No output devices available.");

    sDeviceInfo = Pa_GetDeviceInfo(iOutDeviceNr);

    sndFileInfo.format = 0x0;

    sndFileRW = sf_open(sFileName.c_str(), SFM_READ, &sndFileInfo);

    if(sndFileRW==NULL)
    {
        initErrStr.append("sndFile initialization failed. Wrong file format or empty file?");

        mp3Err = mpg123_init();
        if(mp3Err!=MPG123_OK)
        {
            initErrStr.append("mpg123 read initialization: Couldn't initialize mpg123 instance.");
            throw std::runtime_error(initErrStr);
        }

        mp3Read = mpg123_new(NULL, &mp3Err);
        if(mp3Err!=MPG123_OK)
        {
            initErrStr.append("mpg123 read initialization: Couldn't create new mpg123 handle.");
            throw std::runtime_error(initErrStr);
        }

        mp3Err = mpg123_open(mp3Read, sFileName.c_str());
        if(mp3Err!=MPG123_OK)
        {
            initErrStr.append("mpg123 read initialization: Error when try to open the data stream. No mp3 file?");
            throw std::runtime_error(initErrStr);
        }

        mp3Err = mpg123_getformat(mp3Read, &iFsTmp, &iNumChansTmp, &encodingID);
        if(mp3Err!=MPG123_OK)
            throw std::runtime_error("mpg123 read initialization: Error when try to get the standart decode format.");

        iFs = iFsTmp;
        iNumChans = iNumChansTmp;

        if(encodingID!=MPG123_ENC_SIGNED_16)
        {
            encodingID = MPG123_ENC_SIGNED_16;
            mp3Err = mpg123_format_support(mp3Read, iFsTmp, encodingID);
            if (mp3Err>0)
            {
                mp3Err = mpg123_format(mp3Read, iFsTmp, iNumChansTmp, encodingID);
                if(mp3Err!=MPG123_OK)
                    throw std::runtime_error("mpg123 read initialization: Error when try to set decoder output to 16Bit integer format.");
            }
            else
                throw std::runtime_error("mpg123 read initialization: This mpg123 library does not support 16Bit integer decoding.");
        }

        iNumSamps = mpg123_length(mp3Read);
    }
    else
    {
        iFs = sndFileInfo.samplerate;
        iNumChans = sndFileInfo.channels;
        iNumSamps = sndFileInfo.frames;
    }

    iBufLen = iBlockLen*iNumChans;

    outParams.device = iOutDeviceNr;
    outParams.channelCount = iNumChans;
    outParams.sampleFormat = paFloat32; //32 bit floating point output
    outParams.suggestedLatency = sDeviceInfo->defaultLowInputLatency;
    outParams.hostApiSpecificStreamInfo = NULL;

    paErr = Pa_OpenStream(&paStream, NULL, &outParams, iFs, iBlockLen, paNoFlag, streamFromFileCallback, this);
    if(paErr != paNoError)
        throw std::invalid_argument(std::string(Pa_GetErrorText(paErr)));

    mInOutBuf.resize(iNumBuffers);
    for (uint16_t iBufCntInt = 0; iBufCntInt < iNumBuffers; iBufCntInt++)
    {
        mInOutBuf.at(iBufCntInt).resize(iBufLen);
        vInt16Buf.resize(iBufLen);
        for (uint16_t iSampCnt = 0; iSampCnt < iBufLen; iSampCnt++)
                mInOutBuf.at(iBufCntInt).at(iSampCnt) = 0.0;
    }

    paErr = Pa_StartStream(paStream);
    if(paErr != paNoError)
        throw std::runtime_error(std::string(Pa_GetErrorText(paErr)));

    this->bInitFlag = true;
}

void CppRTA::seek(double fNormOffset)
{
    if (fNormOffset < 0.0)
        fNormOffset = 0.0;
    else if (fNormOffset > 0.999)
        fNormOffset = 0.999;

    uint32_t iOffset = fNormOffset*iNumSamps;

    if (sndFileRW != NULL)
        sf_seek(sndFileRW, iOffset, SEEK_SET);
    else if (mp3Read != NULL)
        mpg123_seek(mp3Read, iOffset, SEEK_SET);

    iPosCnt = iOffset;
}

void CppRTA::setGain(double fGainLog)
{
    if (fGainLog>100.0)
        fGainLog = 100.0;
    else if (fGainLog<-100.0)
        fGainLog = -100.0;

    fGain = pow(10, fGainLog*0.05);
}

void CppRTA::fastLimiter(double *vData)
 {
     uint16_t iFrameCnt, iChanCnt;
     const double fB = 0.5/((double)iFs);
     const double fA = 1-fB;
     double fTmpVal, fMaxVal;

     for (iFrameCnt=0; iFrameCnt<iBufLen; iFrameCnt+=iNumChans)
     {
         fMaxVal = 0;

         for (iChanCnt=0; iChanCnt<iNumChans; iChanCnt++)
         {
             fTmpVal = vData[iFrameCnt+iChanCnt];

             if (fTmpVal<0) fTmpVal = -fTmpVal;
             if (fTmpVal>fMaxVal) fMaxVal=fTmpVal;
         }

         if (fMaxVal > 0.999)
         {
             fMaxVal = 0.99/fMaxVal;
             if (fMaxVal < fLimitGain) fLimitGain = fMaxVal;
         }
         else if (fLimitGain < 0.999)
             fLimitGain = fB + fA*fLimitGain;

         for (iChanCnt=0; iChanCnt<iNumChans; iChanCnt++)
             vData[iFrameCnt+iChanCnt] *= fLimitGain;
     }
}

int CppRTA::inStreamCallback(const void *vInputBuffer, void *vOutputBuffer,
                             unsigned long iFramesPerBuffer,
                             const PaStreamCallbackTimeInfo* sTimeInfo,
                             PaStreamCallbackFlags iStatusFlags,
                             void *userData)
{
    (void) sTimeInfo; (void) iStatusFlags; (void) vOutputBuffer;

    CppRTA* obj = (CppRTA*) userData;
    float *vRecData = (float*) vInputBuffer;

    for (uint32_t iSampCnt = 0; iSampCnt<obj->iBufLen; iSampCnt++)
        obj->mInOutBuf[obj->iBufCnt][iSampCnt] = (double) vRecData[iSampCnt]*obj->fGain;

    if (obj->bLimitEnabled)
        obj->fastLimiter((obj->mInOutBuf[obj->iBufCnt]).data());

    if(obj->bMono)
    {
        for (uint32_t iSampCnt = 0; iSampCnt < obj->iBufLen; iSampCnt+=obj->iNumChans)
        {
            for (uint32_t iIntSampCnt = 1; iIntSampCnt < obj->iNumChans; iIntSampCnt++)
                obj->mInOutBuf[obj->iBufCnt][iSampCnt] += obj->mInOutBuf[obj->iBufCnt][iSampCnt+iIntSampCnt];
            obj->mInOutBuf[obj->iBufCnt][iSampCnt] /= obj->iNumChans;
        }
    }

    obj->iBufCnt++;

    if (obj->iBufCnt >= (obj->iNumBuffers))
        obj->iBufCnt = 0;

    return paContinue;
}

int CppRTA::outStreamCallback(const void *vInputBuffer, void *vOutputBuffer,
                              unsigned long iFramesPerBuffer,
                              const PaStreamCallbackTimeInfo* sTimeInfo,
                              PaStreamCallbackFlags iStatusFlags,
                              void *userData)
{
    (void) sTimeInfo; (void) iStatusFlags; (void) vInputBuffer;

    CppRTA* obj = (CppRTA*) userData;
    float *vPlayData = (float*) vOutputBuffer;

    for (uint32_t iSampCnt = 0; iSampCnt<obj->iBufLen; iSampCnt++)
        obj->mInOutBuf[obj->iBufCnt][iSampCnt] *= obj->fGain;

    if (obj->bLimitEnabled)
        obj->fastLimiter((obj->mInOutBuf[obj->iBufCnt]).data());

    for (uint32_t iSampCnt = 0; iSampCnt<obj->iBufLen; iSampCnt++)
        vPlayData[iSampCnt] = (float) obj->mInOutBuf[obj->iBufCnt][iSampCnt];

    if(obj->bMono)
    {
        for (uint32_t iSampCnt = 0; iSampCnt < obj->iBufLen; iSampCnt+=obj->iNumChans)
        {
            for (uint32_t iIntSampCnt = 1; iIntSampCnt < obj->iNumChans; iIntSampCnt++)
                obj->mInOutBuf[obj->iBufCnt][iSampCnt] += obj->mInOutBuf[obj->iBufCnt][iSampCnt+iIntSampCnt];
            obj->mInOutBuf[obj->iBufCnt][iSampCnt] /= obj->iNumChans;
        }
    }

    obj->iBufCnt++;

    if (obj->iBufCnt >= (obj->iNumBuffers))
        obj->iBufCnt = 0;

    return paContinue;
}

int CppRTA::duplexCallback(const void *vInputBuffer, void *vOutputBuffer,
                           unsigned long iFramesPerBuffer,
                           const PaStreamCallbackTimeInfo* sTimeInfo,
                           PaStreamCallbackFlags iStatusFlags,
                           void *userData)
{
    (void) sTimeInfo; (void) iStatusFlags;

    CppRTA* obj = (CppRTA*) userData;
    float *vPlayData = (float*) vOutputBuffer;
    float *vRecData = (float*) vInputBuffer;

    if (obj->bLimitEnabled)
        obj->fastLimiter((obj->mInOutBuf[obj->iBufCnt]).data());

    for (uint32_t iSampCnt = 0; iSampCnt<obj->iBufLen; iSampCnt++)
        vPlayData[iSampCnt] = (float) obj->mInOutBuf[obj->iBufCnt][iSampCnt];

    for (uint32_t iSampCnt = 0; iSampCnt<obj->iBufLen; iSampCnt++)
        obj->mInOutBuf[obj->iBufCnt][iSampCnt] = (double) vRecData[iSampCnt]*obj->fGain;

    if(obj->bMono)
    {
        for (uint32_t iSampCnt = 0; iSampCnt < obj->iBufLen; iSampCnt+=obj->iNumChans)
        {
            for (uint32_t iIntSampCnt = 1; iIntSampCnt < obj->iNumChans; iIntSampCnt++)
                obj->mInOutBuf[obj->iBufCnt][iSampCnt] += obj->mInOutBuf[obj->iBufCnt][iSampCnt+iIntSampCnt];
            obj->mInOutBuf[obj->iBufCnt][iSampCnt] /= obj->iNumChans;
        }
    }

    obj->iBufCnt++;

    if (obj->iBufCnt >= (obj->iNumBuffers))
        obj->iBufCnt = 0;

    return paContinue;
}

int CppRTA::streamToFileCallback(const void *vInputBuffer, void *vOutputBuffer,
                                 unsigned long iFramesPerBuffer,
                                 const PaStreamCallbackTimeInfo* sTimeInfo,
                                 PaStreamCallbackFlags iStatusFlags,
                                 void *userData)
{
    (void) sTimeInfo; (void) iStatusFlags; (void) vOutputBuffer;

    CppRTA *obj = (CppRTA*) userData;
    float *vRecData = (float*) vInputBuffer;

    if (obj->bLimitEnabled)
        obj->fastLimiter((obj->mInOutBuf[obj->iBufCnt]).data());

    sf_write_double(obj->sndFileRW, obj->mInOutBuf[obj->iBufCnt].data(), obj->iBufLen);

    for (uint32_t iSampCnt = 0; iSampCnt<obj->iBufLen; iSampCnt++)
        obj->mInOutBuf[obj->iBufCnt][iSampCnt] = (double) vRecData[iSampCnt]*obj->fGain;

    if(obj->bMono)
    {
        for (uint32_t iSampCnt = 0; iSampCnt < obj->iBufLen; iSampCnt+=obj->iNumChans)
        {
            for (uint32_t iIntSampCnt = 1; iIntSampCnt < obj->iNumChans; iIntSampCnt++)
                obj->mInOutBuf[obj->iBufCnt][iSampCnt] += obj->mInOutBuf[obj->iBufCnt][iSampCnt+iIntSampCnt];
            obj->mInOutBuf[obj->iBufCnt][iSampCnt] /= obj->iNumChans;
        }
    }

    obj->iBufCnt++;

    if (obj->iBufCnt >= obj->iNumBuffers)
        obj->iBufCnt = 0;

    return paContinue;
}

int CppRTA::streamFromFileCallback(const void *vInputBuffer, void *vOutputBuffer,
                                   unsigned long iFramesPerBuffer,
                                   const PaStreamCallbackTimeInfo* sTimeInfo,
                                   PaStreamCallbackFlags iStatusFlags,
                                   void *userData)
{
    (void) sTimeInfo; (void) iStatusFlags; (void) vInputBuffer;

    CppRTA* obj = (CppRTA*) userData;
    float *vPlayData = (float*) vOutputBuffer;
    size_t iNumSampsRead;

    if (obj->bLimitEnabled)
        obj->fastLimiter((obj->mInOutBuf[obj->iBufCnt]).data());

    for (uint32_t iSampCnt = 0; iSampCnt<obj->iBufLen; iSampCnt++)
        vPlayData[iSampCnt] = (float) obj->mInOutBuf[obj->iBufCnt][iSampCnt];

    if (obj->sndFileRW != NULL)
    {
        iNumSampsRead = sf_read_double(obj->sndFileRW, obj->mInOutBuf[obj->iBufCnt].data(), obj->iBufLen);
        for (uint32_t iSampCnt = 0; iSampCnt < obj->iBufLen; iSampCnt++)
            obj->mInOutBuf[obj->iBufCnt][iSampCnt] *= obj->fGain;
    }
    else if (obj->mp3Read != NULL)
    {
        mpg123_read(obj->mp3Read, (uint8_t*) obj->vInt16Buf.data(), obj->iBufLen*sizeof(int16_t), &iNumSampsRead);
        for (uint32_t iSampCnt = 0; iSampCnt < obj->iBufLen; iSampCnt++)
            obj->mInOutBuf[obj->iBufCnt][iSampCnt] = (double) obj->vInt16Buf[iSampCnt]*obj->fGain/0x8000;
    }

    if(obj->bMono)
    {
        for (uint32_t iSampCnt = 0; iSampCnt < obj->iBufLen; iSampCnt+=obj->iNumChans)
        {
            for (uint32_t iIntSampCnt = 1; iIntSampCnt < obj->iNumChans; iIntSampCnt++)
                obj->mInOutBuf[obj->iBufCnt][iSampCnt] += obj->mInOutBuf[obj->iBufCnt][iSampCnt+iIntSampCnt];
            obj->mInOutBuf[obj->iBufCnt][iSampCnt] /= obj->iNumChans;
        }
    }

    obj->iBufCnt++;
    obj->iPosCnt+=obj->iBlockLen;

    if (obj->iBufCnt >= obj->iNumBuffers)
        obj->iBufCnt = 0;

    return paContinue;
}

std::string* CppRTA::getDeviceInfoStr(uint16_t iDevNr)
{
    const PaDeviceInfo *sDeviceInfoTmp;

    sDeviceInfoTmp = Pa_GetDeviceInfo(iDevNr);

    if (iDevNr>=iNumPortAudioDevices)
    {
        sDeviceInfoStr = std::string("Requested device idx was out of range, have " );
        sDeviceInfoStr.append(std::to_string(iNumPortAudioDevices));
        sDeviceInfoStr.append(" devices");
    }
    else
    {
        sDeviceInfoStr = std::string(Pa_GetHostApiInfo(sDeviceInfoTmp->hostApi)->name);
        sDeviceInfoStr.append(": ");
        sDeviceInfoStr.append(sDeviceInfoTmp->name);
    }
    return &sDeviceInfoStr;
}

CppRTA::~CppRTA(void)
{
    if (paStream != NULL)
    {
        if (Pa_IsStreamActive(paStream)>0)
            Pa_AbortStream(paStream);

        Pa_CloseStream(paStream);
    }


    if (sndFileRW != NULL)
        sf_close(sndFileRW);


    if (mp3Read != NULL)
        mpg123_close(mp3Read);

    Pa_Terminate();
}
