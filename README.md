OpenAL_Strings
==============

Overview
--------

This project implements the Karplus-Strong algorithm to synthesize String-like sounding instrumental sounds in C++. Should be able to be built crossplatform.

src/ - All sourcecode for 


Dependencies
------------

[RtMidi](https://github.com/thestk/rtmidi/): included in repository  
[OpenAL](https://www.openal.org/): Install as system library

Future Stuff
------------

* Update RtMidi, investigate proper way to include the lib from github.
* Change text-based midi note input to RtMidi methods.
* Add proper Makefile support instead of using Codeblocks Project to compile project.
