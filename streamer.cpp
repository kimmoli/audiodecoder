/*
 *
 * (C) Copyright 2014 Kimmo Lindholm
 *
 *
 *
 */

#include "streamer.h"
#include <QQueue>
#include <QThread>
#include <stdio.h>

Streamer::Streamer(int sampleRate)
    : m_cout(stderr, QIODevice::WriteOnly)
{
    m_sampleRate = sampleRate;
    m_playing = false;
    m_canStop = false;

    m_cout << "streamer created"<< endl;
}

void Streamer::queueThis(QAudioBuffer buffer)
{
    m_bufferQueue.enqueue(buffer);

    m_cout << "buffer " << m_bufferQueue.count() << " queued" << endl;

    if (!m_playing)
    {
        m_playing = true;
        emit playingRequested();
    }
}

void Streamer::play()
{
    QAudioBuffer buffer;

    m_cout << "playing" << endl;

    do
    {
        if (!m_bufferQueue.empty())
        {
            buffer = m_bufferQueue.dequeue();
            m_cout << "this buffer " << buffer.byteCount() << " bytes, length " << buffer.duration()/1000 << "ms. buffers left in queue " << m_bufferQueue.count() << endl;

            // TODO: some serious shit here instead of sleep
            // QThread::usleep(buffer.duration());


            if (!m_fileWriter.isOpen() && !m_fileWriter.open("stdout", buffer.format()))
            {
                m_cout << "something broken, fixit. Streamer::play()" << endl;
                return;
            }

            m_fileWriter.write(buffer);
        }
        else
        {
            QThread::msleep(10);
        }
    }
    while (!(m_bufferQueue.empty() && m_canStop));

    m_cout << "playing done" << endl;

    m_playing = false;
    emit finished();
}

void Streamer::decoderDone()
{
    m_canStop = true;
}
