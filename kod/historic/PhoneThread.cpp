/* 
 * File:   PhoneThread.cpp
 * Author: robotour
 * 
 * Created on September 10, 2012, 12:37 AM
 */

#include <string.h>
//#include "PhoneThread.h"

bool PhoneThread::end;
SbotThread* PhoneThread::sbot;

PhoneThread::PhoneThread(SbotThread* sbot1)
{
    sbot = sbot1;
    end = false;
}

PhoneThread::~PhoneThread() {
}

void PhoneThread::run()
{
    pthread_create( &t, NULL, mainLoop, NULL );
    pthread_detach( t );
}

void PhoneThread::stop()
{
    if(!end){
        end = true;
        if( pthread_join( t, NULL )!=0){
            printf("phone thread join error");
        }
    }
}

void* PhoneThread::mainLoop(void*)
{
    int speed = 0;
    int direction = 0;

    char *command = (char *)"/home/robotour/btserver/server";

    FILE* f;

    while (!end)
    {
        // launch the program that will wait for a connection from the phone
        if ( !(f = (FILE*)popen(command,"r")) ){
            // If fpipe is NULL
            perror("PhoneThread::mainLoop() - popen()");
            exit(-1);
        }
        if (f < 0) return 0;
        printf("waiting for user connecting on BT from a phone...\n");

        char b[80];

        do {
            if (0 == fgets( b, 70, f)) break;
            // user with phone has specified some command
            printf("phone: %s", b);

            if (strncmp(b, "fwd", 3) == 0)
            {
                speed++;
                if( speed>10 )  speed = 10;
                sbot->setDirection(direction);
                sbot->setSpeed(speed);
            }
            else if (strncmp(b, "back", 4) == 0)
            {
                speed--;
                if( speed<-10) speed = -10;
                sbot->setDirection(direction);
                sbot->setSpeed(speed);
            }
            else if (strncmp(b, "left", 4) == 0)
            {
                direction--;
                if(direction< -30) direction = -30;
                sbot->setDirection(direction);
                sbot->setSpeed(speed);
            }
            else if (strncmp(b, "right", 5) == 0)
            {
                direction++;
                if(direction> 30) direction = 30;
                sbot->setDirection(direction);
                sbot->setSpeed(speed);
            }
            else if (strncmp(b, "stop", 4) == 0)
            {
                autonomy = false;
                speed = 0;
                direction = 0;
                sbot->setSpeed(speed);
                sbot->setDirection(direction);
            }
            else if (strncmp(b, "go", 2) == 0)
            {
                autonomy = true;
            }
         } while (strncmp(b, "bye", 3) != 0);

         printf("phone has disconnected.");
         // user with the phone has disconnected
         pclose(f);

    } // while (!end)
}

