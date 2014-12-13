/* 
 * File:   SbotThread.h
 * Author: myron
 *
 * Created on Utorok, 2010, september 7, 10:15
 */

#ifndef SBOTTHREAD_H
#define	SBOTTHREAD_H

#include <pthread.h>
#include <ios>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

class SbotData{
public:

    SbotData(){
        lstep = 0;
        rstep = 0;
        lspeed = 0;
        rspeed = 0;
        blocked = 0;
        obstacle = 0;
        distRR = 0;
        distFR = 0;
        distM = 0;
        distFL = 0;
        distRL = 0;
    }

    int lstep;
    int rstep;
    int lspeed;
    int rspeed;

    bool blocked;
    bool obstacle;

    int distRR;
    int distFR;
    int distFL;
    int distRL;
    int distM;
};

class SbotThread{

public:
    SbotThread();
    
    void run();
    void stop();

    //PC->Sbot
    void setDirection(int d);
    void setSpeed(int s);
    void unblock();
    void ignoreObstacle(bool val);

    //Sbot->PC
    //SbotData getData();

    SbotData getData();

    void sendCommand(const char *s);
    int validate(const char* devName);
    void setDeviceName(const char* dev_name);
 /////////
private:
    
    static void* mainLoop(void*);
    

    static char device_name[32];

    pthread_t t;
    
    static FILE* robot;

    static bool end;
    static SbotData data;
    static pthread_mutex_t m_read;
    static pthread_mutex_t m_write;

    static int fdR[2];
    static int fdW[2];
    static int child;

    static int printingDebugging;
    FILE* outlog;

};



#endif	/* SBOTTHREAD_H */

