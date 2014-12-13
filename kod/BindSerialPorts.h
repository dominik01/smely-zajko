/* 
 * File:   BindSerialPorts.h
 * Author: myron
 *
 * Created on Streda, 2010, november 24, 23:03
 */

#ifndef BINDSERIALPORTS_H
#define	BINDSERIALPORTS_H

//#include "GpsThread.h"
#include "ImuThread.h"
#include "SbotThread.h"


class BindSerialPorts {
public:
    BindSerialPorts();
    virtual ~BindSerialPorts();

    void bind( SbotThread* sbot, void * gps, ImuThread* imu );
private:

};

#endif	/* BINDSERIALPORTS_H */

