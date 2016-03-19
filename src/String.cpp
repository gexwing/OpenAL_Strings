#include <AL/al.h>
#include "String.h"
#include <cstdlib>
#include <iostream>
#include <cmath>

#define PI 3.1416
const ALuint SAMPLEFREQ = 48000;
const ALushort MINBUFFERNUM = 10;

const double filter_coeff[]={.2, .6, .2};
const int    filter_len=3;

String::String(double freq)
{
    // Rawbuffer, to generate data
    m_samples=(ALuint)(SAMPLEFREQ/freq);
    m_rawbuffer=new ALshort[m_samples];

    f_decay = 1-(0.1*m_samples/SAMPLEFREQ);

    // AL Buffers for Streaming
    m_buffernum=5000/m_samples;
    if (m_buffernum < MINBUFFERNUM) m_buffernum = MINBUFFERNUM;

    m_buffers=new ALuint[m_buffernum];
    alGenBuffers(m_buffernum,&m_buffers[0]);

    // AL Source
    alGenSources(1, &m_source);



    // Resets internal buffer
    initialize();

    // Generate first buffers and bind them to the source
    for(unsigned int i=0; i<m_buffernum; i++)
    {
        fillBuffer(m_buffers[i]);
    }
    alSourceQueueBuffers(m_source,m_buffernum,&m_buffers[0]);

    // Begin Playback
    alSourcePlay(m_source);

    std::cout << "new String: " << m_samples << " | Buffernum: " << m_buffernum << " | decay: " << f_decay << std::endl;
}

String::~String()
{
    // Stop Playback, remove buffers
    alSourceStop(m_source);
    alSourcei(m_source,AL_BUFFER,NULL);

    // Delete AL Stuff
    alDeleteBuffers(m_buffernum, &m_buffers[0]);
    alDeleteSources(1, &m_source);

    // Dealocate dynamic Stuff
    delete[] m_rawbuffer;
    delete[] m_buffers;
}

void String::initialize()
{
    m_count=0;
    std::srand(0);

    for(ALuint i=0; i<m_samples; i++)
    {
        if (i<m_samples/2) m_rawbuffer[i]=32767;
        else m_rawbuffer[i]=-32768;
    }

}

// Unqueue Buffers, fill, queue again, restart stream if broken!
void String::update()
{
    ALint unqueued;
    alGetSourcei(m_source,AL_BUFFERS_PROCESSED,&unqueued);

    if (unqueued>0)
    {
        ALuint *newbuffers = new ALuint[unqueued];
        alSourceUnqueueBuffers(m_source,unqueued,newbuffers);

        for(int bufnr=0; bufnr<unqueued; bufnr++)
        {
            fillBuffer(newbuffers[bufnr]);
            alSourceQueueBuffers(m_source,1,&newbuffers[bufnr]);
        }

        delete[] newbuffers;
    }

    // Make sure it's still playing
    ALint state;
    alGetSourcei(m_source,AL_SOURCE_STATE,&state);
    if (state!=AL_PLAYING)
    {
        std::cout << "warning: buffer queue empty, restarting playback!" << std::endl;
        alSourcePlay(m_source);
    }
}

void String::fillBuffer(ALuint buffer)
{
    // Make sure given ID is a valid buffer
    if (alIsBuffer(buffer) != AL_TRUE)
    {
        return;
    }

    // Calculate next Buffer
    for(ALuint i=0; i<m_samples; i++)
    {
        // basic lowpass filter
        m_rawbuffer[i]*=filter_coeff[0];
        for (int j=1; j<filter_len; j++)
        {
            m_rawbuffer[i]+=filter_coeff[j]*m_rawbuffer[(i-j+m_samples)%m_samples];
        }

        // Decay over time
        //m_rawbuffer[i] *= f_decay;

    }

    // Copy data to buffer
    alBufferData(buffer,AL_FORMAT_MONO16,m_rawbuffer,m_samples*2,SAMPLEFREQ); // 16bit data -> 2 bytes

    m_count++;
}

ALboolean String::isFinished()
{
    if (m_count > 10.0*SAMPLEFREQ/m_samples)
    {
        return AL_TRUE;
    }

    return AL_FALSE;
}
