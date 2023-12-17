#include <stdbool.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "RoboticSystem.h"

char* directionToString(enum Direction direction) {
    switch(direction) {
        case UP:
            return "UP";
        case DOWN:
            return "DOWN";
        case RIGHT:
            return "RIGHT";
        case LEFT:
            return "LEFT";
        default:
            return "UNKNOWN";
    }
}

char* boolToString(bool boolean) {
    return boolean ? "true" : "false";
}

int createMessageQueue(){
    int msg_queue_id = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msg_queue_id == -1) {
        perror("[C] Error while creating the message queue.");
        exit(EXIT_FAILURE);
    }
    return msg_queue_id;
}

int getMessageQueue(){
    int msg_queue_id = msgget(MSGKEY, 0);
    if (msg_queue_id == -1) {
        perror("Error while adding to the message queue.");
        exit(EXIT_FAILURE);
    }
    return msg_queue_id;
}

struct SharedData* createShm(){
    int shmID = shmget(SHMKEY, sizeof(struct SharedData), IPC_CREAT | 0644);
    if (shmID == -1) {
        perror("[C] Error while creating shared memory - shmget");
        exit(EXIT_FAILURE);
    }
    struct SharedData *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("[C] Error while creating shared memory - shmat");
        exit(EXIT_FAILURE);
    }
    return sharedData;
}

struct SharedData* getShm(){
    int shmID = shmget(SHMKEY, sizeof(struct SharedData), 0);
    if (shmID == -1) {
        perror("Error while getting shared memory - shmget");
        exit(EXIT_FAILURE);
    }
    struct SharedData *sharedData = shmat(shmID, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("Error while getting shared memory - shmat");
        exit(EXIT_FAILURE);
    }
    return sharedData;
}