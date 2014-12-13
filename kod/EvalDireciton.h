/* 
 * File:   EvalDireciton.h
 * Author: myron
 *
 * Created on Sobota, 2010, august 14, 17:27
 */

#ifndef EVALDIRECITON_H
#define	EVALDIRECITON_H

#include <cv.h>

class EvalDireciton {
     CvPoint A,B,C;
    int S;

    CvMat* mask;

    void set_mask();
    double comp_sum( CvMat* frame);

public:
    int triangle_w;
    int triangle_h;
    int dir_count;
    int frame_w;
    int frame_h;
    
    EvalDireciton( int triangle_w, int triangle_h, int dir_count, int frame_w, int frame_h );
    
    double eval( CvMat* frame, int direction );
    int get_best( CvMat* frame );

    virtual ~EvalDireciton();
private:

};

#endif	/* EVALDIRECITON_H */

