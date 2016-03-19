#include <iostream>
#include <cstdlib>

#ifdef __WIN__
#include <windows.h> //sleep
#define _sleep(x) Sleep(x)
#else
#include <unistd.h>
#define _sleep(x) usleep(1000*x)
#endif

// threaded Stuff
#include <pthread.h>
#include <semaphore.h>

// OpenAL Stuff
#include <AL/al.h>
#include <AL/alc.h>

#include "StringManager.h"

static ALCdevice *g_device = NULL;
static ALCcontext *g_context = NULL;

StringManager bleh;
sem_t sem_block;

// update thread
void* update(void* arg)
{
    for(;;)
    {
        sem_wait(&sem_block);
        bleh.update();
        sem_post(&sem_block);

        _sleep(10);
    }
    return NULL;
}

ALboolean ALinit(ALchar *device)
{
    ALint lMajor, lMinor;

    g_device = alcOpenDevice(device);
    if (g_device == NULL)
    {
        std::cout << "ERROR: Unknown Device specified";
        return AL_FALSE;
    }

    alcGetError(g_device); //clear error state
    g_context = alcCreateContext(g_device,NULL);
    if (g_context == NULL)
    {
        std::cout << "ERROR: Cannot create context";
        return AL_FALSE;
    }
    alcMakeContextCurrent(g_context);

    alcGetIntegerv(g_device, ALC_MAJOR_VERSION, 1, &lMajor);
    alcGetIntegerv(g_device, ALC_MINOR_VERSION, 1, &lMinor);

    std::cout << "OpenAL Version " << lMajor << "." << lMinor << std::endl;
    std::cout << "Renderer: " << alGetString(AL_RENDERER) << std::endl;
    std::cout << "Vendor: " << alGetString(AL_VENDOR) << std::endl;

    return AL_TRUE;
}

void ALclose()
{
    alcDestroyContext(g_context);
    alcCloseDevice(g_device);
}

int main(int argc, char* argv[])
{
    if (ALinit(NULL) == AL_FALSE)
    {
        return EXIT_FAILURE;
    }
    std::cout << "OpenAL Strings: Type MIDI mumber to play note" << std::endl;

    // Update Thread
    sem_init(&sem_block,0,1);
    pthread_t handle;
    if (pthread_create(&handle,NULL,&update,NULL))
    {
        std::cout << "ERROR: Thread creation failed!" << std::endl;
    }
    else {
        sem_wait(&sem_block);
        bleh.addString(35);
        sem_post(&sem_block);

        // Main Loop
        ALuint q;
        do {
                std::cin >> q;

                sem_wait(&sem_block);
                bleh.addString(q);
                sem_post(&sem_block);

        } while (q>0);
    }

    // Cleanup
    pthread_cancel(handle);
    pthread_join(handle, NULL); // second argument??

    sem_close(&sem_block);
    ALclose();

    return EXIT_SUCCESS;
}

