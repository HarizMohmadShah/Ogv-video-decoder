#include <pthread.h>
#include <assert.h>
#include <stdio.h>

#include "synchro.h"
#include "ensitheora.h"

bool fini;

/* les variables pour la synchro, ici */

/* les variables pour Fenetre et Texture */
pthread_mutex_t mutexSynchro1;
pthread_cond_t fenetre, texture;

/* les variables pour prod et cons */
pthread_mutex_t mutexSynchro2;
pthread_cond_t cons, prod;

int nbTexturePrete = 0;

/* l'implantation des fonctions de synchro ici */
void initSynchro(){
  pthread_mutex_init(&hashLock, NULL);
  pthread_mutex_init(&mutexSynchro1, NULL);
  pthread_mutex_init(&mutexSynchro2, NULL);
  pthread_cond_init(&fenetre, NULL);
  pthread_cond_init(&texture, NULL);
  pthread_cond_init(&cons, NULL);
  pthread_cond_init(&prod, NULL);
}

void destroySynchro(){
  pthread_mutex_destroy(&hashLock);
  pthread_mutex_destroy(&mutexSynchro1);
  pthread_mutex_destroy(&mutexSynchro2);
  pthread_cond_destroy(&fenetre);
  pthread_cond_destroy(&texture);
  pthread_cond_destroy(&cons);
  pthread_cond_destroy(&prod);
}

void envoiTailleFenetre(th_ycbcr_buffer buffer) {
  pthread_mutex_lock(&mutexSynchro1);
  windowsx = buffer[0].width;
  windowsy = buffer[0].height;
  pthread_cond_signal(&fenetre);
  pthread_mutex_unlock(&mutexSynchro1);
}

void attendreTailleFenetre() {
  pthread_mutex_lock(&mutexSynchro1);
  pthread_cond_wait(&fenetre, &mutexSynchro1);
  pthread_mutex_unlock(&mutexSynchro1);
}

void signalerFenetreEtTexturePrete() {
  pthread_mutex_lock(&mutexSynchro1);
  pthread_cond_signal(&texture);
  pthread_mutex_unlock(&mutexSynchro1);
}

void attendreFenetreTexture() {
  pthread_mutex_lock(&mutexSynchro1);
  pthread_cond_wait(&texture, &mutexSynchro1);
  pthread_mutex_unlock(&mutexSynchro1);
}

void debutConsommerTexture() {
  pthread_mutex_lock(&mutexSynchro2);
  while(nbTexturePrete == 0){
    pthread_cond_wait(&cons, &mutexSynchro2);
  }
}

void finConsommerTexture() {
  nbTexturePrete--;
  pthread_cond_signal(&prod);
  pthread_mutex_unlock(&mutexSynchro2);
}

void debutDeposerTexture() {
  pthread_mutex_lock(&mutexSynchro2);
  while(nbTexturePrete == NBTEX){
    pthread_cond_wait(&prod, &mutexSynchro2);
  }
}

void finDeposerTexture() {
  nbTexturePrete++;
  pthread_cond_signal(&cons);
  pthread_mutex_unlock(&mutexSynchro2);
}
