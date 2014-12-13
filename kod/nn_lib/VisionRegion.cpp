/* 
 * File:   neural_network3.cpp
 * Author: myron
 * 
 * Created on Pondelok, 2011, február 14, 12:28
 */

#include "VisionRegion.h"

/*
        PUBLIC
*/

VisionRegion::VisionRegion(int image_width, int image_height, int step_x, int step_y, int pattern_size): VisionBase(image_width, image_height, step_x, step_y) {
    this->input_neurons = (step_x * step_y * 3) + ((pattern_size*pattern_size)*3) ; // extended input
    this->pattern_size = pattern_size;
}

int VisionRegion::create_training(vector<IplImage*> inputs, vector<CvMat*> outputs, char* out_file){
    printf("Not implemented!\n");
    exit(0);
    return 0;
}

int VisionRegion::create_random_training( vector<IplImage*> inputs, vector<CvMat*> outputs, int sampleCount, char* out_file ) {

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

            for(int si=0; si<this->pattern_size; si++){
                for(int sj=0; sj<this->pattern_size; sj++){
                    CvScalar s = cvGet2D(input2, si, sj );

                    for(int c=0; c<channels; c++){

                        fprintf(outFile, "%f ", s.val[c] );//TODO: check \n!!!
                    }
                }
            }
            fprintf(outFile, "\n");

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

IplImage* VisionRegion::create_sample_match(IplImage* input){
    int r  = image_width / 8;   // radius of picking sample to compare


    //random position with radius r
    int r_x = (((double)rand() / (double)RAND_MAX)* (2*r) ) - r;
    int y = sqrt( (r*r) - (r_x*r_x) );
    int r_y = y*((double)rand() / (double)RAND_MAX);

    //get subimage at random position
    cvSetImageROI( input, cvRect( r_x + (input->width/2) - (this->pattern_size/2), r_y, this->pattern_size, this->pattern_size  ) );
    IplImage* result = cvCreateImage( cvGetSize(input), input->depth, input->nChannels );
    cvCopy(input, result);

    //printf( "size %d %d \n ", frameTmp->width, frameTmp->height );

    cvResetImageROI( input );

    return result;
}

vector<fann_type*> VisionRegion::create_predict_mat(IplImage* input){

    vector<fann_type*> result;

    printf("num input %d \n", ann->num_input);

    IplImage* input2 = create_sample_match(input);

    for(int x=0; x<(image_width-step_x) ; x++ ){
        for(int y=0; y<(image_height-step_y) ; y++ ){

            fann_type* row = new fann_type[ ann->num_input ];

            int p = 0;

            for(int i=0; i<step_x; i++){
                for(int j=0; j<step_y; j++){
                    CvScalar s = cvGet2D(input, y+j, x+i);
                    for(int c=0; c<channels; c++){
                        row[ p ] = s.val[c];//row[ c+((i + (j*step_x))*channels) ] = s.val[c];
                        p++;
                    }
                }
            }

            for(int si=0; si<this->pattern_size; si++){
                for(int sj=0; sj<this->pattern_size; sj++){
                    CvScalar s = cvGet2D(input2, si, sj );
                    for(int c=0; c<channels; c++){
                        row[ p ] = s.val[c];
                        p++;
                    }
                }
            }


            result.push_back( row );
        }
    }

    return result;
}