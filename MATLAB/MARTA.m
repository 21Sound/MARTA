%Description:
%--MARTA--: |MA|TLAB |R|eal-|T|ime |A|udio interface for both streaming 
%from and to soundcards and streaming from files (play back / record audio 
%file). This project is based on portaudio, libsndfile, libmpg123, and 
%the C libraries for windows were compiled using minGW.
%Usage: classInstance = MARTA(blockLen, verbose)
%
%Input parameters:
%   blockLen:   Desired audio processing block lengt in range 64 to 16384
%   verbose:    information printing of the c interface (debbuging purpose)
%
% ------------------------------------------------------------------------
% Example: fcnDummy()
%
% Author: Hagen Jaeger (c) TGM @ Jade Hochschule applied licence see EOF
% Version History:
% Ver. 0.01 initial create (empty) 21.02.2017 HJ
% Ver. 0.1 seems to work (quite buggy) 22.07.2016 HJ

classdef MARTA < handle
    
    properties (GetAccess = public)
        MARTAptr;
        iFs;
        iNumDevices;
        iNumChans;
        iBlockLen;
        iBufPosOld;
        iBufPosNew;
        iNumBuffers;
        iNumDSPfcn;
        callbackTimer;
        bVerbose;
        bDSP;
        vData;
        tmpPtr;
        fcnHandleDSP;
    end
    
    methods (Access = public)
        function obj = MARTA(blockLen, numBuffers, verbose)
            if nargin < 1, blockLen = 1024; end
            if nargin < 2, numBuffers = 5; end
            if nargin < 3, verbose = false; end
            
            obj.bVerbose = verbose;
            obj.iBlockLen = 2^round(log2(blockLen));
            obj.iNumBuffers = min(numBuffers, 255);
            
            if obj.bVerbose
                disp(' Verbose (info printing) activated.');
            end
            
            obj.callbackTimer = timer('ExecutionMode', 'fixedRate');
            
            if strcmp(computer, 'PCWIN64')
                addpath('win64');
            elseif strcmp(computer, 'PCWIN')
                addpath('win32');
            end
            
            if ~exist('libMARTA.dll', 'file') ...
                && ~exist('libportaudio.dll', 'file') ...
                && ~exist('libmpg123-0.dll', 'file') ...
                && ~exist('libsndfile-1.dll', 'file')
                error(['not all dependencies were found. You need ' ...
                'libMARTA.dll, libportaudio.dll, libmpg123-0.dll and ' ...
                'libsndfile-1.dll in the system dependent directory.'])
            end
                
            if ~libisloaded('libMARTA')
                loadlibrary('libMARTA.dll', @pMARTA);
            end
            
            obj.MARTAptr = libpointer();
            obj.MARTAptr = calllib('libMARTA', 'initMARTA', blockLen, numBuffers);
            
            if (obj.MARTAptr == 0)
                error('MARTA initialization failed for some reason.');
            end
            
            obj.iNumDevices = calllib('libMARTA', 'getNumDevices', obj.MARTAptr);
            
            if(obj.bVerbose)
                disp(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
            end
            
            obj.bDSP = false;
            obj.iBufPosOld = 0;
            obj.iBufPosNew = 0;
            obj.iNumDSPfcn = 0;
        end
        
        function streamFromFile(obj, sFullPath, iOutDev)
            if nargin < 2, error('give at least a valid file path.'); end
            if nargin < 3, iOutDev = -1; end
            
            iTmp = strfind(sFullPath, '.');
            
            if length(iTmp)>1
                error('Invalid file name (includes more than one dot).');
            end
            
            if isempty(strfind(sFullPath, '/')) ...
                && isempty(strfind(sFullPath, '\'))
                sFullPath = fullfile(pwd, sFullPath);
            end
            
            if ~exist(sFullPath, 'file')
                error([sFullPath ': File does not exist.']);
            end
            
            iTmp = calllib('libMARTA', 'streamFromFile', obj.MARTAptr, sFullPath, iOutDev);
            
            if (iTmp<0)
                error(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
                obj.delete();
                return;
            end
            
            obj.iFs = calllib('libMARTA', 'getSampRate', obj.MARTAptr);
            obj.iNumChans = calllib('libMARTA', 'getNumChans', obj.MARTAptr);
            obj.tmpPtr = libpointer('doublePtr', ...
                zeros(1,obj.iBlockLen*obj.iNumChans));
            
            if(obj.bVerbose)
                disp(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
            end
        end
        
        function streamToFile(obj, sFullPath, iInDev, iFs, iNumChans, iFormat)
            if nargin < 2, error('give at least a valid file path.'); end
            if nargin < 3, iInDev = -1; end
            if nargin < 4, iFs = 0; end
            if nargin < 5, iNumChans = 0; end
            if nargin < 6, iFormat = 0; end
            
            iTmp = strfind(sFullPath, '.');
            if length(iTmp)>1
                error('Invalid file name (includes more than one dot).');
            end
            
            if isempty(strfind(sFullPath, '/')) ...
                && isempty(strfind(sFullPath, '\'))
                sFullPath = fullfile(pwd, sFullPath);
            end
            
            iTmp = calllib('libMARTA', 'streamToFile', obj.MARTAptr, ...
                sFullPath, iInDev, iFs, iNumChans, iFormat);
            
            if (iTmp<0)
                error(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
                obj.delete();
                return;
            end
            
            obj.iFs = calllib('libMARTA', 'getSampRate', obj.MARTAptr);
            obj.iNumChans = calllib('libMARTA', 'getNumChans', obj.MARTAptr);
            obj.tmpPtr = libpointer('doublePtr', ...
                zeros(1,obj.iBlockLen*obj.iNumChans));
            
            if(obj.bVerbose)
                disp(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
            end
        end
        
        function duplexStream(obj, iDuplexDev, iFs, iNumChans)
            if nargin < 2, iDuplexDev = -1; end
            if nargin < 3, iFs = 0; end
            if nargin < 4, iNumChans = 0; end
            
            iTmp = calllib('libMARTA', 'duplex', obj.MARTAptr, ...
                iDuplexDev, iFs, iNumChans);
            
            if (iTmp<0)
                error(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
                obj.delete();
                return;
            end
            
            obj.iFs = calllib('libMARTA', 'getSampRate', obj.MARTAptr);
            obj.iNumChans = calllib('libMARTA', 'getNumChans', obj.MARTAptr);
            obj.tmpPtr = libpointer('doublePtr', ...
                zeros(1,obj.iBlockLen*obj.iNumChans));
            
            if(obj.bVerbose)
                disp(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
            end
        end
        
        function outStream(obj, iOutDev, iFs, iNumChans)
            if nargin < 2
                iOutDev = calllib('libMARTA', 'getStdOutDev', obj.MARTAptr); 
            end
            if nargin < 3, iFs = 0; end
            if nargin < 4, iNumChans = 0; end
            
            iTmp = calllib('libMARTA', 'outStream', obj.MARTAptr, ...
                iOutDev, iFs, iNumChans);
            
            if (iTmp<0)
                error(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
                obj.delete();
                return;
            end
            
            obj.iFs = iFs;
            obj.iNumChans = iNumChans;
            obj.tmpPtr = libpointer('doublePtr', ...
                zeros(1,obj.iBlockLen*obj.iNumChans));
            
            if(obj.bVerbose)
                disp(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
            end
        end
        
        function inStream(obj, iInDev, iFs, iNumChans)
            if nargin < 2, iInDev = -1; end
            if nargin < 3, iFs = 44100; end
            if nargin < 4, iNumChans = 1; end
            
            iTmp = calllib('libMARTA', 'inStream', obj.MARTAptr, ...
                iInDev, iFs, iNumChans);
            
            if (iTmp<0)
                error(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
                obj.delete();
                return;
            end
            
            obj.iFs = iFs;
            obj.iNumChans = iNumChans;
            obj.tmpPtr = libpointer('doublePtr', ...
                zeros(1,obj.iBlockLen*obj.iNumChans));
            
            if(obj.bVerbose)
                disp(calllib('libMARTA', 'getActErrMsg', obj.MARTAptr));
            end
        end
        
        function sDeviceInfo = getDeviceInfo(obj, iDeviceNr)
            if nargin < 2, error('No device number provided.'); end
            sDeviceInfo = calllib('libMARTA', 'getDeviceInfoStr', ...
                obj.MARTAptr, iDeviceNr);
        end
        
        function connectDSP(obj, fcnHandle, fTimerInterval)
            if nargin < 2, error('No function handle provided.'); end
            if nargin < 3, fTimerInterval = 0.01; end
            
            if ~isa(fcnHandle,'function_handle')
                error('First input argument is no function handle.');
            end
            
            obj.iNumDSPfcn = obj.iNumDSPfcn+1;
            
            obj.fcnHandleDSP{obj.iNumDSPfcn} = fcnHandle;
            
            if (obj.bDSP)
                obj.bDSP = false;
                stop(obj.callbackTimer);
            end
            
            set(obj.callbackTimer, 'Period', fTimerInterval, ...
                'TimerFcn', @obj.timerCallback);
            
            start(obj.callbackTimer);
            obj.bDSP = true;
        end
        
        function disconnectDSP(obj)
            if (obj.bDSP)
                obj.bDSP = false;
                stop(obj.callbackTimer);
            end 
            
            obj.iNumDSPfcn = 0;
            
            obj.fcnHandleDSP = [];
        end
        
        function seek(obj, fNormPos)
            fNormPos = max(min(fNormPos,1),0);
            calllib('libMARTA', 'seek', obj.MARTAptr, fNormPos);
        end
        
        function timerCallback(obj, ~, ~)
            obj.iBufPosNew = calllib('libMARTA', 'getCurrentDataPos', obj.MARTAptr);
            
            iBufCnt = obj.iBufPosOld;
            while iBufCnt~=obj.iBufPosNew
                calllib('libMARTA', 'getDataBuf', obj.MARTAptr, ...
                    obj.tmpPtr, iBufCnt);

                vTmp = reshape(get(obj.tmpPtr, 'Value'), obj.iNumChans, obj.iBlockLen)';
                
                for iFcnHandleCnt = 1:obj.iNumDSPfcn
                    vTmp = obj.fcnHandleDSP{iFcnHandleCnt}(vTmp, obj.iFs);
                end
                
                obj.vData = reshape(vTmp', obj.iBlockLen*obj.iNumChans, 1);

                calllib('libMARTA', 'setDataBuf', obj.MARTAptr, ...
                    obj.vData, iBufCnt);
                
                iBufCnt = iBufCnt+1;
                if (iBufCnt>=obj.iNumBuffers), iBufCnt = 0; end
            end
            
            obj.iBufPosOld = obj.iBufPosNew;
        end
        
        function delete(obj)
            if (obj.bDSP)
                stop(obj.callbackTimer);
            end
            
            obj.MARTAptr = calllib('libMARTA', 'releaseMARTA', obj.MARTAptr);
            
            if libisloaded('libMARTA')
                unloadlibrary('libMARTA');
            end
        end
 
    end
        
    methods (Static = true)
        function iFormat = getSndFileFormat(sFileEnding, sNumberFormat, sEndianFormat)
            if strcmp(sFileEnding, '.wav'), iFileFormat = hex2dec('010000');
            elseif strcmp(sFileEnding, '.aiff'), iFileFormat = hex2dec('020000');
            elseif strcmp(sFileEnding, '.au'), iFileFormat = hex2dec('030000');
            elseif strcmp(sFileEnding, '.raw'), iFileFormat = hex2dec('040000');
            elseif strcmp(sFileEnding, '.paf'), iFileFormat = hex2dec('050000');
            elseif strcmp(sFileEnding, '.iff'), iFileFormat = hex2dec('060000');
            elseif strcmp(sFileEnding, '.nist'), iFileFormat = hex2dec('070000');
            elseif strcmp(sFileEnding, '.voc'), iFileFormat = hex2dec('080000');
            elseif strcmp(sFileEnding, '.sf'), iFileFormat = hex2dec('0A0000');
            elseif strcmp(sFileEnding, '.w64'), iFileFormat = hex2dec('0B0000');
            elseif strcmp(sFileEnding, '.mat4'), iFileFormat = hex2dec('0C0000');
            elseif strcmp(sFileEnding, '.mat5'), iFileFormat = hex2dec('0D0000');
            elseif strcmp(sFileEnding, '.pvf'), iFileFormat = hex2dec('0E0000');
            elseif strcmp(sFileEnding, '.xm'), iFileFormat = hex2dec('0F0000');
            elseif strcmp(sFileEnding, '.htk'), iFileFormat = hex2dec('100000');
            elseif strcmp(sFileEnding, '.sds'), iFileFormat = hex2dec('110000');
            elseif strcmp(sFileEnding, '.avr'), iFileFormat = hex2dec('120000');
            elseif strcmp(sFileEnding, '.wavx'), iFileFormat = hex2dec('130000');
            elseif strcmp(sFileEnding, '.sd2f'), iFileFormat = hex2dec('160000');
            elseif strcmp(sFileEnding, '.flac'), iFileFormat = hex2dec('170000');
            elseif strcmp(sFileEnding, '.caf'), iFileFormat = hex2dec('180000');
            elseif strcmp(sFileEnding, '.wve'), iFileFormat = hex2dec('190000');
            elseif strcmp(sFileEnding, '.xiph'), iFileFormat = hex2dec('200000');
            elseif strcmp(sFileEnding, '.mpc'), iFileFormat = hex2dec('210000');
            elseif strcmp(sFileEnding, '.rf64'), iFileFormat = hex2dec('220000');
            else error([sFileEnding ' file writing is not supported.']);
            end

            if strcmp(sNumberFormat, 'signed8'), iNumFormat = hex2dec('0001');
            elseif strcmp(sNumberFormat, 'sint16'), iNumFormat = hex2dec('0002');
            elseif strcmp(sNumberFormat, 'sint24'), iNumFormat = hex2dec('0003');
            elseif strcmp(sNumberFormat, 'sint32'), iNumFormat = hex2dec('0004');
            elseif strcmp(sNumberFormat, 'uint8'), iNumFormat = hex2dec('0005');
            elseif strcmp(sNumberFormat, 'float32'), iNumFormat = hex2dec('0006');
            elseif strcmp(sNumberFormat, 'float64'), iNumFormat = hex2dec('0007');
            elseif strcmp(sNumberFormat, 'ulaw'), iNumFormat = hex2dec('0010');
            elseif strcmp(sNumberFormat, 'alaw'), iNumFormat = hex2dec('0011');
            elseif strcmp(sNumberFormat, 'IMA_ADPCM'), iNumFormat = hex2dec('0012');
            elseif strcmp(sNumberFormat, 'MS_ADPCM'), iNumFormat = hex2dec('0013');
            elseif strcmp(sNumberFormat, 'GSM610'), iNumFormat = hex2dec('0020');
            elseif strcmp(sNumberFormat, 'VOX_ADPCM'), iNumFormat = hex2dec('0021');
            elseif strcmp(sNumberFormat, 'G721_32'), iNumFormat = hex2dec('0030');
            elseif strcmp(sNumberFormat, 'G723_24'), iNumFormat = hex2dec('0031');
            elseif strcmp(sNumberFormat, 'G723_40'), iNumFormat = hex2dec('0032');
            elseif strcmp(sNumberFormat, 'DWVW_12'), iNumFormat = hex2dec('0040');
            elseif strcmp(sNumberFormat, 'DWVW_16'), iNumFormat = hex2dec('0041');
            elseif strcmp(sNumberFormat, 'DWVW_24'), iNumFormat = hex2dec('0042');
            elseif strcmp(sNumberFormat, 'DWVW_N'), iNumFormat = hex2dec('0043');
            elseif strcmp(sNumberFormat, 'DPCM_8'), iNumFormat = hex2dec('0050');
            elseif strcmp(sNumberFormat, 'DPCM_16'), iNumFormat = hex2dec('0051');
            elseif strcmp(sNumberFormat, 'VORBIS'), iNumFormat = hex2dec('0060');
            else error([sNumberFormat ' is an unknown number format.']); 
            end

            if strcmp(sEndianFormat, 'default'), iEndianFormat = 0;
            elseif strcmp(sEndianFormat, 'little'), iEndianFormat = hex2dec('10000000');
            elseif strcmp(sEndianFormat, 'big'), iEndianFormat = hex2dec('20000000');
            elseif strcmp(sEndianFormat, 'CPU'), iEndianFormat = hex2dec('30000000');
            else iEndianFormat = 0; 
            end
            
            iFormat = bitor(iFileFormat, iNumFormat);
            iFormat = bitor(iFormat, iEndianFormat);
        end
    end
end

%--------------------Licence ---------------------------------------------
% Copyright (c) <2011-2017> Hagen Jaeger
% Institute for Hearing Technology and Audiology
% Jade University of Applied Sciences
% All rights reserved.
%
% Redistribution and use in source and binary forms, with or without 
% modification, are permitted provided that the following conditions are met:
% 1. Redistributions of source code must retain the above copyright notice, 
%    this list of conditions and the following disclaimer.
% 2. Redistributions in binary form must reproduce the above copyright 
%    notice, this list of conditions and the following disclaimer in 
%    the documentation and/or other materials provided with the distribution.
% 3. Neither the name of the copyright holder nor the names of its 
%    contributors may be used to endorse or promote products derived from 
%    this software without specific prior written permission.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
% "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
% LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
% FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
% COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
% INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
% BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
% OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
% ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
% TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
% USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.