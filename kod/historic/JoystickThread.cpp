/* 
 * File:   JoystickThread.cpp
 * Author: robotour
 * 
 * Created on September 15, 2010, 5:36 PM
 */

//DEPRECATED
//TODO: rewrite for new type of joystick, when necessary

#include "JoystickThread.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#define JOY_DEV "/dev/input/js0"

int JoystickThread::joy_fd, *JoystickThread::axis=NULL, JoystickThread::num_of_axis=0, JoystickThread::num_of_buttons=0;
char *JoystickThread::button=NULL, JoystickThread::name_of_joystick[80];
struct js_event JoystickThread::js;
bool JoystickThread::error = false;

bool JoystickThread::autonomny = true;

SbotThread* JoystickThread::sbot;

JoystickThread::JoystickThread(SbotThread* sbot1) {
    this->error = !joystickInit();
    this->sbot = sbot1;
}

JoystickThread::~JoystickThread() {
}

void JoystickThread::run(){

    if(error){
        return;
    }

    pthread_create( &t, NULL, mainLoop, NULL );
    pthread_detach( t );
}

bool JoystickThread::joystickInit(){

    if( ( joy_fd = open( JOY_DEV , O_RDONLY)) == -1 )
    {
            printf( "Couldn't open joystick\n" );
            return false;
    }

    ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
    ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
    ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );

    axis = (int *) calloc( num_of_axis, sizeof( int ) );
    button = (char *) calloc( num_of_buttons, sizeof( char ) );

    printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
            , name_of_joystick
            , num_of_axis
            , num_of_buttons );

    fcntl( joy_fd, F_SETFL, O_NONBLOCK );   /* use non-blocking mode */

    return true;
}

void JoystickThread::update_joystick()
{
    /* read the joystick state */
    read(joy_fd, &js, sizeof(struct js_event));

            /* see what to do with the event */
    switch (js.type & ~JS_EVENT_INIT)
    {
            case JS_EVENT_AXIS:
                    axis   [ js.number ] = js.value;
                    break;
            case JS_EVENT_BUTTON:
                    button [ js.number ] = js.value;
                    break;
    }

}

void JoystickThread::send_command(char d){

    if(error){
        return;
    }

    char s[2];
    s[0] = d;
    s[1] = 0;
    sbot->sendCommand( s );
}

void JoystickThread::wait_release_button(int b)
{
    while (button[b])
    {
        update_joystick();
        usleep(1000);
    }
}

void* JoystickThread::mainLoop(void*){

    int speed = 0;
    int direction = 0;

    while(!error){
        update_joystick();
        if( axis[1]<0 ){
            usleep(200000);
            
            speed++;
            if( speed>10 ){
                speed = 10;
            }

            sbot->setSpeed(speed);

        } else if( axis[1]>0 )
        {
            usleep(200000);

            speed--;
            if( speed<0){
                speed = 0;
            }

            sbot->setSpeed(speed);
        }

        if( axis[0]<0 ){
            usleep(20000);

            direction--;
            if(direction< -30){
                direction = -30;
            }

            sbot->setDirection(direction);

        } else if( axis[0]>0 ){
            usleep(20000);

            direction++;
            if(direction> 30){
                direction = 30;
            }

            sbot->setDirection(direction);
        }

        if( button[9] ){
            autonomny =! autonomny;
            wait_release_button(9);
            sleep(1);
            printf("autonomny: %d \n", autonomny);
        }
        if( button[10] ){
            sbot->ignoreObstacle(true);
            wait_release_button(10);
            sleep(1);
            printf("ignore obstacle \n");
        }
        if( button[11] ){
            sbot->ignoreObstacle(false);
            wait_release_button(11);
            sleep(1);
            printf("unignore obstacle \n");
        }


        if( button[6] ){
            //send_command(' ');
            speed = 0;
            sbot->setSpeed( 0 );
            wait_release_button(6);
        }
        if( button[7] ){
            //send_command(13);
            speed = 0;
            sbot->setSpeed( 0 );
            wait_release_button(7);
        }

        if( button[3] ){
            //send_command('w');
            sbot->setSpeed( 3 );
            sbot->setDirection( 0 );
            wait_release_button(3);
        }else if( button[0] ){
            //send_command('s');
            sbot->setSpeed( -3 );
            wait_release_button(0);
        }

        if( button[2] ){
            //send_command('a');
            sbot->setDirection( -40 );
            wait_release_button(2);
        }else if( button[1] ){
            //send_command('d');
            sbot->setDirection( 40 );
            wait_release_button(1);
        }

        usleep( 50000 );

//        if( button[9] ){
//            send_command('W');
//            wait_release_button(9);
//        }else if( button[11] ){
//            send_command('S');
//            wait_release_button(11);
//        }
//
//        if( button[8] ){
//            send_command('A');
//            wait_release_button(8);
//        }else if( button[10] ){
//
//            send_command('D');
//            wait_release_button(10);
//        }

        //printf("? \n");
    }
}