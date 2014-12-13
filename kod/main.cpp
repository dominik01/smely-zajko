/* 
 * File:   main.cpp
 * Author: myron
 *
 * Created on Sobota, 2010, august 14, 17:23
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <opencv/highgui.h>
#include <sys/time.h>
#include <valarray>

#include "SbotThread.h"
#include "BindSerialPorts.h"
#include "rapidxml.hpp"
#include "fstream"
#include "oneLine.h"
#include "Jazda.h"
//#include "ImuThread.h"

using namespace std;
using namespace rapidxml;
using namespace cv;

const bool START_CAM = 1;
//const bool USE_LOCALIZATION = 0;
const int camera_w = 320;
const int camera_h = 240;

FILE* mainLog;
int log_counter=0;
bool autonomy = true;

/** Function Headers */
bool detectAndDisplay( Mat frame );
void update_position(int index, int lstep, int rstep);

 /** Global variables */
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

void init_video(){
    FILE* mplayer;
    char *command=(char *)"mplayer tv:// -tv driver=v4l2:device=/dev/video1:immediatemode=0:normid=0:outfmt=YUY2:input=1";

    if ( !(mplayer = (FILE*)popen(command,"r")) ){
        // If fpipe is NULL
        perror("Problems with pipe");
        exit(1);
    }

    char b[1024];

    while ( fgets( b, sizeof b, mplayer) ){
        if( strcmp( b, "Starting playback...\n" )==0){
            break;
        }
        printf("%s", b );
    }

    pclose( mplayer );

    system("clear");
}
 

//void log_data(SbotData sdata, Ll gdata, ImuData idata, double mapAngle, double kmtotarget )
void log_data(SbotData sdata ) //zapis vystupov do suboru, vypis vystupov
{
    time_t t;
    time(&t);
    fprintf(mainLog, " %ld %d %d %d %d %d %d %d %d %d %d %d\n", t, sdata.lstep, sdata.rstep, sdata.lspeed,
        sdata.rspeed, sdata.blocked, sdata.obstacle, sdata.distRL, sdata.distFL, sdata.distM, sdata.distFR, sdata.distRR );

    fflush(mainLog);
    
    printf("data > %d %d %d %d %d %d %d %d %d %d %d \n", sdata.lstep, sdata.rstep, sdata.lspeed,
           sdata.rspeed, sdata.blocked, sdata.obstacle, sdata.distRL, sdata.distFL, sdata.distM, sdata.distFR, sdata.distRR);
}

bool detectAndDisplay( Mat frame )
{
  std::vector<Rect> faces;
  Mat frame_gray;

  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  //-- Detect faces
  
  face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
  
  for( size_t i = 0; i < faces.size(); i++ )
  {
    Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
    ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

    Mat faceROI = frame_gray( faces[i] );
    std::vector<Rect> eyes;

    //-- In each face, detect eyes
    eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    for( size_t j = 0; j < eyes.size(); j++ )
     {
       Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
       int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
       circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
     }
  }
  //-- Show what you got
  imshow( window_name, frame );
  return (faces.size() > 0);
 }

int main(int argc, char** argv) {
//    IplImage* localizationFrame;
    Mat frame2;

    setlocale(LC_ALL, "C");
    time_t t;
    time(&t);

    char logname[64];
    sprintf(logname, "logs/%ld.log", t);
    mainLog = fopen( logname, "w");
    if( mainLog == NULL ){
        printf("log cannot be created.\n");
        exit(-1);
    }

    IplImage* empty_frame = cvCreateImage( cvSize( camera_w, camera_h ), 8, 3);

    cvNamedWindow( "camera", CV_WINDOW_AUTOSIZE );
    //cvNamedWindow( "path", CV_WINDOW_AUTOSIZE );

    cvShowImage( "camera", empty_frame );

    cvMoveWindow( "camera", 1000, 30 );

    CvCapture* capture;
    
   if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
   if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
    
    //if( START_CAM ){
        init_video();
        printf("mplayer init video done.\n");

        capture = cvCaptureFromCAM(1);  //(1);
    
    //}
    
    
    //else{ 
        //debug
        //capture = cvCreateFileCapture( "../../video/b23.avi");
       // capture = cvCreateFileCapture( "../../stromovka-friday/236.jpg");
      //  }
    
    //ImuThread imu;
    SbotThread sbot;
//    PhoneThread phone(&sbot);

    BindSerialPorts bs;

//    bs.bind( &sbot, &gps, &imu );
    
    bs.bind( &sbot, NULL, NULL );

    //sbot.init();
    //imu.init();

    sbot.run();

    sleep(1);
    sbot.setSpeed(1);
    sleep(2);
    sbot.setSpeed(0);

    //imu.run();
    
    IplImage* frame;
    setlocale(LC_ALL, "C");
    
    autonomy = true;

    bool was_obstacle = false;
    long found_obstacle;
    
    int finnumber = 1;
    double lft = 0;
    double rgh = 0;
    int bakdir = 0;

    time_t tmnow = time(NULL);

    int number_of_states = 4;
    int state = 0;
    time_t time_to_switch_state = tmnow + 5;
    
    Jazda jazda;
    SbotData sdata;
    jazda.load_map();
    int angle;
    
    /*sleep(1);
    sbot.setDirection(90);
    sbot.setSpeed(2);
    sleep(4);
    sbot.setSpeed(0);*/
    while(1) {
            if( capture ) {   
                frame2 = cvQueryFrame( capture );
                if( !frame2.empty() ) {
                      if (detectAndDisplay( frame2 )) {
                          
                          sbot.setDirection(0);
                          sbot.setSpeed(0);
                          
                      } else {
                          sbot.setDirection(0);
                          sbot.setSpeed(2);
                      }
                  }
                else { 
                      printf(" --(!) No captured frame -- Break!"); break; 
                }     
      
            }
        
        /*tmnow = time(NULL);
//        printf("time:%d state: %d %d\n", tmnow, state, time_to_switch_state);
        if (tmnow > time_to_switch_state) 
        {
           // state = (state + 1) % number_of_states;
            time_to_switch_state = tmnow + 5;
        
            switch (state)
            {
            case 0: 
            {
                sbot.setDirection(0);
                sbot.setSpeed(3);
                break;
            }   
            case 1:
            {
                sbot.setDirection(80);
                sbot.setSpeed(1);
                break;
            }   
            case 2:
            {
                sbot.setDirection(0);
                sbot.setSpeed(3);
                break;
            }   
            case 3:
            {
                sbot.setDirection(0);
                sbot.setSpeed(-3);
                break;
            }
            }             
        }*/
        
        /*frame = cvQueryFrame( capture );
        if( !frame ){
            printf("no frame, exiting..\n");
            break;
        }*/

        //IplImage* tmp_frame = cvCreateImage( cvSize( camera_w, camera_h ), frame->depth,frame->nChannels);
        //IplImage* rgb_frame = cvCreateImage( cvSize( camera_w, camera_h ), frame->depth,frame->nChannels);
        //cvResize( frame, tmp_frame );
        //cvResize( frame, rgb_frame );
	//cvCvtColor(tmp_frame, tmp_frame, CV_BGR2Lab);
        //cvFlip( tmp_frame, tmp_frame, 0);
        //cvFlip( tmp_frame, tmp_frame, 1);

        sdata = sbot.getData();
        jazda.update_position(1, sdata.lstep, sdata.rstep);
        //jazda.lines[jazda.get_lineId(jazda.position[0], jazda.position[1])].get_info();
        angle = jazda.v_strede(sdata.distRL, sdata.distRR);
        if (angle) {
            jazda.vyrovnavaj(angle);
        }
        
        //ImuData idata = imu.getData();
        
        if (sdata.obstacle && (!was_obstacle))
        {
            time_t tobs = time(NULL);
            found_obstacle = tobs;
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!OBSTACLE\n");
        }
        if ((sdata.obstacle) && (tmnow - found_obstacle > 20))
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BACKING UP\n");
            found_obstacle = found_obstacle +30;
            // cuvat alebo aspon tocit!!
            
            if(true){
                bakdir = -40;
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! LEFT\n");
            }
            else{
                bakdir = 40;
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RIGHT\n");
            }
            sbot.ignoreObstacle(true);
            sbot.setDirection( 0 );
            sbot.setSpeed( -3 );
            sleep(4);
            sbot.setDirection( bakdir );
            sbot.setSpeed( 2 );
            sleep(2);
            sbot.setDirection( 0 );
            sbot.setSpeed( 0 );
            sleep(2);
            sbot.ignoreObstacle(false);
        }
        else if (sdata.obstacle)
        {
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %ld\n", tmnow - found_obstacle);
            fflush(stdout);
        }
        was_obstacle = sdata.obstacle;

        log_data(sdata);
        

        //cvShowImage( "camera", frame );
        
        //cvReleaseImage( &frame );
        
        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }

    sbot.stop();
    //imu.stop();
    
    cvReleaseCapture( &capture );
    cvDestroyWindow( "camera" );
    cvDestroyWindow( "debug" );
    
    fclose(mainLog);

    return 0;
}

