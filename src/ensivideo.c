#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <pthread.h>

#include "stream_common.h"
#include "oggstream.h"
#include "synchro.h"

pthread_mutex_t videoLock;
pthread_mutex_t mutexSynchro;
pthread_cond_t fenetre;
pthread_cond_t texture;

int main(int argc, char *argv[]) {
    int res;
    pthread_t theora, vorbis;

    if (argc != 2) {
	fprintf(stderr, "Usage: %s FILE", argv[0]);
	exit(EXIT_FAILURE);
    }
    assert(argc == 2);
    
    // Initialisation de la SDL
    res = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS);
    atexit(SDL_Quit);
    assert(res == 0);

    //Initialisation mutex,moniteurs
    initSynchro();
    
    // start the two stream readers
    pthread_create(&theora, NULL, theoraStreamReader, (void*)argv[1]);
    pthread_create(&vorbis, NULL, vorbisStreamReader, (void*)argv[1]);
    
    // wait audio thread
    pthread_join(vorbis, NULL); 

    // 1 seconde de garde pour le son,
    sleep(1);

    // tuer les deux threads videos si ils sont bloqués
    pthread_cancel(theora2sdlthread);
    pthread_cancel(theora);
    
    // attendre les 2 threads videos
    pthread_join(theora2sdlthread,NULL);
    pthread_join(theora,NULL);

    // fin du programme
    destroySynchro();

    exit(EXIT_SUCCESS);    
}
