/*
 *
 * (C) Copyright 2014 Kimmo Lindholm
 *
 *
 *
 */

#ifndef STREAMER_H
#define STREAMER_H

#include <QAudioDecoder>
#include <QTextStream>
#include <QQueue>

#include "wavefilewriter.h"

class Streamer : public QObject
{
    Q_OBJECT

public:
    Streamer(int sampleRate);
    ~Streamer() { }

signals:
    void playingRequested();
    void finished();

public slots:
    void queueThis(QAudioBuffer buffer);
    void play();
    void decoderDone();

private:

    bool m_playing;
    bool m_canStop;

    QTextStream m_cout;

    int m_sampleRate;
    QQueue<QAudioBuffer> m_bufferQueue;


    WaveFileWriter m_fileWriter;

};


#endif // STREAMER_H
