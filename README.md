[MA]tlab [R]eal [T]ime [A]udio library.
==============================

Non-blocking, low-latency C++ audio signal processing class with C and MATLAB wrapper.

This project intends to provide a user-friendly C++ API for the most important functionalities of blockwise audio 
reading / writing libraries and real time audio streaming. It uses libmpg123 and libsndfile to read (un-)compressed
audio data from HDD on-the-fly (minimum RAM usage) and portaudio to stream data from / into files. MARTA itself
is a MATLAB class which uses the C wrapper for the C++ API, so the functionalities can be used in MATLAB code to 
develop light weight DSP code in an easy way (writing DSP code in MATLAB tends to be much faster than in C / C++). 

Installation
------------

On a x_86 machine running 32/64 bit Windows Vista or newer, the MATLAB implementation should run without building any
library from source (Use shipped libraries in the MATLAB win32/64 directory). Just navigate to the MATLAB-directory
after downloading the source and run the DEMO scripts. Handling is intend to be easy, so you can immediately start
writing blockoriented DSP code. You can use the C / C++ API libMARTA directly if you like to use the interface in
other languages as C/C++/python.

Functionality
------------

After starting the "playFromFileExample.m" with a valid file name inserted, you should be able to listen to your favourite
song via MATLAB! Connect DSP functions such as the example filter functions "loPass, hiPass, bandPass" with 
``cFilePlay.connectDSP(@loPass, 0.01)``; where @x is the function handle to your DSP function (copy one of the examples and
start by editing it) and the second input variable is the periodicity MATLAB checks for updated data. If you have high
blocklength values, mew blocks are coming in less often, so you can lower this value. Nevertheless, 5-10ms seems to
be the fastest update rate that can be achieved cross platform, so it is no good idea to lower this value behind 0.005.

Build
-----

Use the CmakeList.txt file to make a build from source. The direct external dependencies are libmpg123, libsndfile and 
portaudio. On Windows, it is recommended to get the pre-built binaries from the mpg123 / sndfile homepage and build 
port audio dll from source. Due to some external dependencies of these libraries, you may ship more than these .dlls 
(libgcc / stdc++ / pthread dlls a.s.o). Just have a look at the win32/64 folders in the MATLAB directory to get an
idea of possible external dependencies.
