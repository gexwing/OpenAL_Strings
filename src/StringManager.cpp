#include "StringManager.h"
#include <AL/al.h>
#include <map>

double midifreq(ALuint note)
{
    static double factors[12] =
    {
        8.17579891564, /* midifreq(0)               */
        8.66195721803, /* midifreq(0) * 2 ** (1/12) */
        9.17702399742, /* midifreq(0) * 2 ** (2/12) */
        9.72271824132,
        10.3008611535,
        10.9133822323,
        11.5623257097,
        12.2498573744,
        12.9782717994,
        13.7500000000,
        14.5676175474,
        15.4338531643  /* midifreq(0) * 2 ** (11/12) */
    };

    return factors[note % 12] * (1 << (note / 12));
}


StringManager::StringManager()
{

}

StringManager::~StringManager()
{
    std::map<ALuint, String*>::iterator it;
    for (it=StringList.begin(); it!=StringList.end(); it++)
    {
        delete (*it).second;
    }
    StringList.clear();
}

void StringManager::addString(ALuint miditone)
{
    if (miditone<20 or miditone>80) return;

    std::map<ALuint, String*>::iterator it = StringList.find(miditone);
    if (it != StringList.end())
    {
        delete (*it).second;
    }
    StringList[miditone]=new String(midifreq(miditone));
}

void StringManager::update()
{
    std::map<ALuint, String*>::iterator it;
    for (it=StringList.begin(); it!=StringList.end();)
    {
        if (it->second->isFinished() == AL_TRUE)
        {
            delete (*it).second;
            StringList.erase(it++);
        }

        else
        {
            it->second->update();
            ++it;
        }
    }
}
