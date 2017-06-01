[MA]tlab / [Cp]lus[p]lus [R]eal [T]ime [A]udio library
==============================

Non-blocking, low-latency C++ audio signal processing class with C and MATLAB wrapper.

CppRTA intends to provide a user-friendly C++ API for the most important functionalities of blockwise audio 
reading / writing libraries and real time audio streaming. It uses libmpg123 and libsndfile to read (un-)compressed
audio data from HDD on-the-fly (minimum RAM usage) and portaudio to stream data from / into soundcards. 

MARTAis a MATLAB class which uses the C wrapper for the C++ API, so the functionalities can be used in MATLAB code to 
develop light weight DSP code in an easy way (writing DSP code in MATLAB tends to be much faster than in C / C++). MATLAB calls the C-library functions in MARTA via calllib. You can call the MARTA lib also directly in plain C-Code.

Installation
------------

On a x_86 machine running 32/64 bit Windows Vista or newer, the MATLAB implementation should run without building any
library from source (Use shipped libraries in the MATLAB win32/64 directory). Just navigate to the MATLAB-directory
after downloading the source and run the DEMO scripts. Handling is intend to be easy, so you can immediately start
writing blockoriented DSP code.

CppRTA should be build from source. Check my examples to get an idea of the functionality. You can either compile the C++ example (default), a plain C-Example, or even a QtGUI-based example (You need Qt5 libraries for this). Just (un-)comment the projects you want to build in the CMakeList.txt-file.

Functionality
------------

MARTA: After starting the "playFromFileExample.m" with a valid file name inserted, you should be able to listen to your favourite song via MATLAB! Stop the playback with ``delete(>nameOfYourClassInstance<);``. Connect DSP functions such as the example  filter functions ``loPass``, ``hiPass``, ``bandPass``with ``>nameOfYourClassInstance<.connectDSP(@loPass, 0.01);`` where @x is the function handle to your DSP function (copy one of the examples and start by editing it) and the second input variable is the periodicity MATLAB checks for updated data. If you have high blocklength values, new blocks are coming in less often, so you can lower this value. Nevertheless, 5-10ms seems to be the fastest update rate that can be achieved cross platform, 
so it is no good idea to lower this value behind 0.005.

Note: You can do everything just in time, the playback won't be interrupted because it's managed in the .dll. If your block
length and/or number of buffers is too short for your actual streaming parameters, the DSP functions may not work, because MATLAB
can not get the data buffers fast enough (overwritten by the reading /streaming inside the .dll). The solution to this is simple:
just increase the blocklength and/or number of buffers.

The lowest latency that can be achieved is highly hardware, driver and OS dependent. On Windows, it could be possible to achieve
10-20ms overall latency with small blocksizes (128 or maybe 64) and only one or two ringbuffer elements. On Linux, it seems to be 
easy to go even further (<10ms). Native Linux and OSX support for the MARTA class will be added soon.

CppRTA: Check my examples, they should be self-explaining completely.

Build
-----

Use the CmakeList.txt file to make a build from source. The direct external dependencies are libmpg123, libsndfile and 
portaudio. On Windows, it is recommended to get the pre-built binaries from the mpg123 / sndfile homepage and build 
port audio dll from source. Due to some external dependencies of these libraries, you may ship more than these .dlls 
(libgcc / stdc++ / pthread dlls a.s.o). Just have a look at the win32/64 folders in the MATLAB directory to get an
idea of the external dependencies I found.
