/* 
 * File:   GpsThread.h
 * Author: myron
 *
 * Created on Streda, 2010, september 8, 16:26
 */

#ifndef GPSTHREAD_H
#define	GPSTHREAD_H

#include "DataTypes.h"
#include "LocalizationAndPlaning.h"
#include <string>

using namespace std;

class GpsThread {

public:
    GpsThread();

    void init();
    void run();
    void stop();

    Ll getData();

    int validate(char* dev_name);
    void setDeviceName(char* dev_name);

private:

    static int gps;

    char device_name[32];

    static bool end;
    static pthread_mutex_t m_read;
    static Ll data;
    static string buffer;

    pthread_t t;

    static void* mainLoop(void*);
    static void parseGpsLine(char *s);
    static Ll nmea_to_decimal(Ll p);
    

};

#endif	/* GPSTHREAD_H */

