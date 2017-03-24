#include <QApplication>
#include "mainWindow.h"

/*----------------------------------------------------------------------------*\
Test routine for the C++ real time audio class with basic Qt library support.
See mainWindow.cpp/.h for more informations.

Author: (c) Uwe Simmer, Hagen Jaeger    January 2017 - Now
\*----------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWin;
    mainWin.setMaximumSize(400,200);
    mainWin.resize(400,200);
    mainWin.show();

    return app.exec();
}

//--------------------- License ------------------------------------------------

// Copyright (c) 2017 Uwe Simmer, Hagen Jaeger

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
