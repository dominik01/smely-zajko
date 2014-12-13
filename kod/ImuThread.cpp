/* 
 * File:   ImuThread.cpp
 * Author: myron
 * 
 * Created on Streda, 2010, september 8, 14:56
 */

#include "ImuThread.h"

#include <stdio.h>

#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>
#include <iosfwd>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include <linux/unistd.h>

using namespace std;

bool ImuThread::end;
pthread_mutex_t ImuThread::m_read;
int ImuThread::imu = 0;
ImuData ImuThread::data;
char ImuThread::IMUPORT[32];

ImuThread::ImuThread() {
    end = true;
}

int ImuThread::validate(char* devName){
    struct termios oldtio,newtio;

    imu = open(devName, O_RDWR | O_NOCTTY | O_NONBLOCK );
    if(imu<0){
        //printf("imu not found at %s\n", devName);
        return 0;
    }

    //strncpy(IMUPORT, devName, strlen(devName)+1);

    tcgetattr(imu,&oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = B38400 | CS8 | CLOCAL | CREAD ; //B57600
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 char received */

    tcflush(imu, TCIFLUSH);
    tcsetattr(imu,TCSANOW,&newtio);

    //validate
    char b[1024];

    for(int i=0; i<10; i++){
        usleep( 100000 );//100ms

        read(imu, b, 1024 );

        //printf( "%s\n", b );

        //parse data, not buffered!!
        istringstream is2(b);

        if(!is2.eof()){//TODO: find newline?

            is2 >> data.xAngle;
            if(is2.fail()){ continue; }

            is2 >> data.yAngle;
            if(is2.fail()){ continue; }

            is2 >> data.zAngle;
            if(is2.fail()){ continue; }

            if( data.xAngle>=0 && data.xAngle<3600 &&
                data.yAngle>=-900 && data.yAngle<=900 &&
                data.zAngle>=-900 && data.zAngle<=900){

                //printf( "%f %f %f\n", data.xAngle, data.yAngle, data.zAngle  );

                return 1;
                close(imu);
            }
        }

    }
    close( imu );
    return 0;

}

void ImuThread::init(){

    if (end) {
        return;
    }

     struct termios oldtio,newtio;

    imu = open(IMUPORT, O_RDWR | O_NOCTTY | O_NONBLOCK );
    if(imu<0){
        //printf("imu not found at %s\n", devName);
        return;
    }

    tcgetattr(imu,&oldtio); /* save current port settings */

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = B38400 | CS8 | CLOCAL | CREAD ; //B57600
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 char received */

    tcflush(imu, TCIFLUSH);
    tcsetattr(imu,TCSANOW,&newtio);
}

void ImuThread::setDeviceName(char* dev_name){
    strcpy(IMUPORT, dev_name );
    end = false;
}

void* ImuThread::mainLoop(void*){

   char b[1024];

   FILE* inlog;
   inlog = fopen( "../logs/imu-in.log", "a" );

    while(!end){
        //reinit();
        usleep( 100000 );//100ms

        
        read(imu, b, 1024 );
        pthread_mutex_lock( &m_read );

        //printf( "%s\n", b );

        //fprintf( inlog, "%ld %s", time(NULL), b );
        time_t t = time(NULL);
        fprintf(inlog, "%s|%s", ctime(&t) ,b);
        fflush(inlog);

        //parse data, not buffered!!
        istringstream is2(b);

        bool valid = true;

        double xAngle;
        double yAngle;
        double zAngle;

        if(!is2.eof()){//TODO: find newline?

            is2 >> xAngle;
            if(is2.fail()){ valid=false; }

            is2 >> yAngle;
            if(is2.fail()){ valid=false; }

            is2 >> zAngle;
            if(is2.fail()){ valid=false; }

            if(valid){
                data.xAngle = xAngle;
                data.yAngle = yAngle;
                data.zAngle = zAngle;

//                printf( "COMPASS: %f %f %f\n", data.xAngle, data.yAngle, data.zAngle  );
            }
        }

        pthread_mutex_unlock( &m_read);
        
    }

   close( imu );
   fclose( inlog );
}

void ImuThread::run(){

    if (end) {
        return;
    }

    pthread_mutex_init( &m_read, NULL );

    pthread_create( &t, NULL, mainLoop, NULL );
    pthread_detach( t );
}

void ImuThread::stop(){
    if(!end){
        end = true;

        if( pthread_join( t, NULL )!=0){
            printf("imu thread join error");
        }
    }
}

ImuData ImuThread::getData(){

    ImuData result;

    if (end) {
        return result;
    }

    pthread_mutex_lock( &m_read);

    result = data;

    pthread_mutex_unlock( &m_read);

    return result;
}
