#ifndef STRING_H_
#define STRING_H_

#include <AL/al.h>

/* String class, delivers OpenAL Buffer ready to play */
class String
{
public:
  String(double freq);
  ~String();

  void update();
  ALboolean isFinished();

private:
  void initialize();
  void fillBuffer(ALuint buffer);

  float f_decay;

  ALuint m_count;
  ALuint m_samples;

  ALuint m_source;

  ALuint m_buffernum;
  ALuint *m_buffers;

  ALshort *m_rawbuffer; //16bit unsigned PCM data
};

#endif
