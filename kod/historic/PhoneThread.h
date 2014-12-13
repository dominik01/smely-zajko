/* 
 * File:   PhoneThread.h
 * Author: robotour
 *
 * Created on September 10, 2012, 12:37 AM
 */

#ifndef PHONETHREAD_H
#define	PHONETHREAD_H

#include <pthread.h>
#include <ios>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "SbotThread.h"

extern bool autonomy;

class PhoneThread {
public:
    PhoneThread(SbotThread* sbot1);
    PhoneThread(const PhoneThread& orig);
    virtual ~PhoneThread();
    void run();
    void stop();
    static void* mainLoop(void*);
    
private:
    pthread_t t;
    static SbotThread* sbot;
    static bool end;
};

#endif	/* PHONETHREAD_H */

