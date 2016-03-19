#ifndef STRINGMANAGER_H_
#define STRINGMANAGER_H_

#include <AL/al.h>
#include "String.h"
#include <map>

class StringManager
{
public:
  StringManager();
  ~StringManager();

  void addString(ALuint);
  void update();

private:
  std::map<ALuint, String*> StringList;
};

#endif //STRINGMANAGER_H_
