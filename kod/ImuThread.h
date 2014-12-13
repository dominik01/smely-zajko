/* 
 * File:   ImuThread.h
 * Author: myron
 *
 * Created on Streda, 2010, september 8, 14:56
 */

#ifndef IMUTHREAD_H
#define	IMUTHREAD_H

#include <pthread.h>
#include <ios>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

class ImuData{
public:
    double xAngle;
    double yAngle;
    double zAngle;
};

class ImuThread {
    
public:
    ImuThread();

    void init();
    void run();
    void stop();

    ImuData getData();

    int validate(char* devName);
    void setDeviceName(char* dev_name);

private:
    static char IMUPORT[32];

    static bool end;
    static pthread_mutex_t m_read;
    static int imu;
    static ImuData data;
    
    pthread_t t;
   
    static void* mainLoop(void*);
};

#endif	/* IMUTHREAD_H */

