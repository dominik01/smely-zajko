/* 
 * File:   neural_network2.cpp
 * Author: myron
 * 
 * Created on Piatok, 2010, december 10, 9:45
 */

#include <opencv/highgui.h>

#include "VisionRegionReduced.h"

/*
 Training and prediction with additional input(matrix of match samples in front of robot to whole frame)
*/


/*
        PUBLIC
*/

VisionRegionReduced::VisionRegionReduced(int image_width, int image_height, int step_x, int step_y, int pattern_size): VisionBase(image_width, image_height, step_x, step_y) {
    this->input_neurons = (step_x * step_y * 3) + 1; // one for extended input
    this->pattern_size = pattern_size;
}

int VisionRegionReduced::create_training( vector<IplImage*> inputs, vector<CvMat*> outputs, char* out_file ) {

    FILE* outFile = fopen(out_file, "wt");
    if(outFile == NULL ){
        return 0;
    }

    int positives = 0;

    //head
    fprintf(outFile, "%d %d %d\n", inputs.size()*parts_x*parts_y, this->input_neurons, this->output_neurons );

    for (int k = 0; k < inputs.size(); k++) {

        CvArr* input2 = create_sample_match( inputs[k] );

        for (int i = 0; i < parts_x; i++) {
            for (int j = 0; j < parts_y; j++) {

                //printf(">random > %d %d \n", r_x, r_y);

                int positivePixels = 0;

                for (int x = 0; x < step_x; x++) {
                    for (int y = 0; y < step_y; y++) {

                        CvScalar s = cvGet2D(inputs[k], (j*step_y) + y, (i*step_x) + x);

                        for (int c = 0; c < channels; c++) {
                            fprintf(outFile, "%f ", s.val[c]);
                        }

                        s = cvGet2D(outputs[k], (j*step_y) + y, (i*step_x) + x);
                        if (s.val[0] == 1) {
                            positivePixels++;
                        }
                    }
                }

                CvScalar s = cvGet2D(input2, (j*step_y), (i*step_x) );
                fprintf(outFile, "%f\n", s.val[0] );

                double val = (double)positivePixels/(step_x*step_y);
                fprintf(outFile, "%f\n", val  );
                if(val>0.5){
                    positives ++;
                }
            }
        }
    }

    fclose(outFile);
    return positives;
}

int VisionRegionReduced::create_random_training( vector<IplImage*> inputs, vector<CvMat*> outputs, int sampleCount, char* out_file ) {

    FILE* outFile = fopen(out_file, "wt");
    if(outFile == NULL ){
        return 0;
    }

    int positives = 0;

    //head
    fprintf(outFile, "%d %d %d\n", inputs.size()*sampleCount, this->input_neurons, this->output_neurons );

    for(int k=0; k<inputs.size(); k++ ){

        IplImage* input2 = create_sample_match( inputs[k] );

        for(int i=0; i<sampleCount; i++ ){

            int r_x = (((double)rand() / (double)RAND_MAX)* (image_width-step_x-1) );
            int r_y = (((double)rand() / (double)RAND_MAX)* (image_height-step_y-1) );

            //printf(">random > %d %d \n", r_x, r_y);

            int positivePixels = 0;

            for(int x=0; x<step_x; x++ ){
                for(int y=0; y<step_y; y++ ){

                    CvScalar s = cvGet2D(inputs[k], r_y+y, r_x+x );

                    for(int c=0; c<channels; c++){

                        fprintf(outFile, "%f ", s.val[c]);
                    }

                    s = cvGet2D(outputs[k], r_y+y, r_x+x);
                    if( s.val[0]==1 ){
                        positivePixels++;
                    }
                }
            }


            //printf( "r_x %d r_y %d w %d h %d \n", r_x, r_y, input2->width , input2->height  );
            //fflush( stdout );

            CvScalar s = cvGet2D(input2, r_y, r_x );
            fprintf(outFile, "%f\n", s.val[0] );

            double val = (double)positivePixels/(step_x*step_y);
            fprintf(outFile, "%f\n", val  );
            if(val>0.5){
                positives ++;
            }

        }
    }

    fclose(outFile);
    return positives;
}

/*
        PRIVATE
*/

IplImage* VisionRegionReduced::create_sample_match(IplImage* input){
    IplImage* result;
    int r  = image_width / 8; //= 50;  // radius of picking sample to compare


    //random position with radius r
    int r_x = (((double)rand() / (double)RAND_MAX)* (2*r) ) - r; 
    int y = sqrt( (r*r) - (r_x*r_x) );
    int r_y = y*((double)rand() / (double)RAND_MAX);

    //get subimage at random position
    cvSetImageROI( input, cvRect( r_x + (input->width/2) - (this->pattern_size/2), r_y, this->pattern_size, this->pattern_size  ) );
    IplImage* frameTmp = cvCreateImage( cvGetSize(input), input->depth, input->nChannels );
    cvCopy(input, frameTmp);

    //printf( "size %d %d \n ", frameTmp->width, frameTmp->height );

    cvResetImageROI( input );

    // Allocate Output Images:
    int iwidth = input->width - frameTmp->width + 1;
    int iheight = input->height - frameTmp->height + 1;

    //printf( "size2 %d %d \n ", iwidth, iheight );


    IplImage* unscaledResult = cvCreateImage( cvSize( iwidth, iheight ), 32, 1 );

    cvMatchTemplate( input, frameTmp, unscaledResult, CV_TM_SQDIFF_NORMED );
    cvNormalize( unscaledResult, unscaledResult, 1, 0, CV_MINMAX );

    result = cvCreateImage( cvSize( this->image_width, this->image_height ), 32, 1 );
    cvResize(unscaledResult, result);


    cvReleaseImage( &frameTmp );
    cvReleaseImage( &unscaledResult );

    return result;
}

 vector<fann_type*> VisionRegionReduced::create_predict_mat(IplImage* input){

    vector<fann_type*> result;
    
    printf("num input %d \n", ann->num_input);

    IplImage* input2 = create_sample_match(input);

    for(int x=0; x<(image_width-step_x) ; x++ ){
        for(int y=0; y<(image_height-step_y) ; y++ ){

            fann_type* row = new fann_type[ ann->num_input ];

            for(int i=0; i<step_x; i++){
                for(int j=0; j<step_y; j++){
                    CvScalar s = cvGet2D(input, y+j, x+i);
                    for(int c=0; c<channels; c++){
                        row[ c+((i + (j*step_x))*channels) ] = s.val[c];
                    }
                }
            }

            CvScalar s = cvGet2D(input2, y, x);
            row[ ann->num_input-1 ] = s.val[0];

            result.push_back( row );
        }
    }

    return result;
}