/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "audiodecoder.h"
#include "streamer.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QThread>

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream cout(stderr, QIODevice::WriteOnly);

    if ((app.arguments().size() < 2) || (isatty(fileno(stdout))))
    {
        cout << "Usage: audiodecoder SOURCEFILE [sample rate] |> ..." << endl;
        cout << "default sample rate is 48000" << endl << endl;
        cout << "Note that wav is output to stdout! redirect to file or pipe to gst-launch" << endl;
        cout << " e.g. gst-launch-0.10 -v fdsrc ! wavparse ! audioconvert ! alsasink" << endl;
        return 0;
    }

    QFileInfo sourceFile;

    sourceFile.setFile(app.arguments().at(1));

    int sampleRate = 48000;

    if (app.arguments().size() > 2)
    {
        sampleRate = app.arguments().at(2).toInt();
    }

    cout << "Samplerate is " << sampleRate << endl;

    AudioDecoder* decoder = new AudioDecoder(sampleRate);
    QThread* worker = new QThread();
    Streamer* streamer = new Streamer(sampleRate);

    streamer->moveToThread(worker);

    QObject::connect(streamer, SIGNAL(playingRequested()), worker, SLOT(start()));
    QObject::connect(worker, SIGNAL(started()), streamer, SLOT(play()));
    QObject::connect(streamer, SIGNAL(finished()), worker, SLOT(quit()), Qt::DirectConnection);

    QObject::connect(streamer, SIGNAL(finished()), &app, SLOT(quit()));
    QObject::connect(decoder, SIGNAL(enqueue(QAudioBuffer)), streamer, SLOT(queueThis(QAudioBuffer)), Qt::DirectConnection);
    QObject::connect(decoder, SIGNAL(done()), streamer, SLOT(decoderDone()), Qt::DirectConnection);

    decoder->setSourceFilename(sourceFile.absoluteFilePath());
    decoder->start();

    int i = app.exec();

    cout << "app exiting with status " << i << endl;

    return i;
}
