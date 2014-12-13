/* 
 * File:   JoystickThread.h
 * Author: robotour
 *
 * Created on September 15, 2010, 5:36 PM
 */

#ifndef JOYSTICKTHREAD_H
#define	JOYSTICKTHREAD_H

#include <stdlib.h>
#include "SbotThread.h"

class JoystickThread {
public:
    JoystickThread(SbotThread* sbot);
    virtual ~JoystickThread();

    void run();

    static bool joystickInit();
    
    static void send_command(char d);
    

    
    static bool autonomny;

private:

    static bool error;

    static void update_joystick();
    static void wait_release_button(int b);

    static SbotThread* sbot;

    pthread_t t;

    static void* mainLoop(void*);

    static int joy_fd, *axis, num_of_axis, num_of_buttons;
    static char *button, name_of_joystick[80];
    static struct js_event js;
};

#endif	/* JOYSTICKTHREAD_H */

