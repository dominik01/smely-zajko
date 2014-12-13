/* 
 * File:   Neural_Network4.cpp
 * Author: myron
 * 
 * Created on Pondelok, 2011, február 28, 14:58
 */

//#include <opencv/cxtypes.h>
#include <opencv2/core/types_c.h>

#include "VisionHistogram.h"

VisionHistogram::VisionHistogram(int image_width, int image_height, int step_x, int step_y, int histogram_size): VisionBase(image_width, image_height, step_x, step_y) {
    this->input_neurons = (step_x * step_y * 3) + (histogram_size*3) ; // extended input
    this->histogram_size = histogram_size;
}

int VisionHistogram::create_training(vector<IplImage*> inputs, vector<CvMat*> outputs, char* out_file){
    printf("Not implemented!\n");
    exit(0);
    return 0;
}

int VisionHistogram::create_random_training( vector<IplImage*> inputs, vector<CvMat*> outputs, int sampleCount, char* out_file ) {

    FILE* outFile = fopen(out_file, "wt");
    if(outFile == NULL ){
        return 0;
    }

    int positives = 0;

    //head
    fprintf(outFile, "%d %d %d\n", inputs.size()*sampleCount, this->input_neurons, this->output_neurons );

    for(int k=0; k<inputs.size(); k++ ){

        vector<CvHistogram*> input2 = create_histograms( inputs[k] );

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


            //printf( ">>size: %d \n", input2.size()  );
            //fflush( stdout );

            for(int hi=0; hi<input2.size(); hi++){
                for(int hj=0; hj<this->histogram_size; hj++){
                    float histValue = cvQueryHistValue_1D( input2[hi], hj);
                    fprintf(outFile, "%f ", histValue );
                }
            }
            fprintf(outFile, "\n");

            double val = (double)(positivePixels)/(step_x*step_y);
            val = val*val;
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
vector<CvHistogram*> VisionHistogram::create_histograms(IplImage* input){

    IplImage* r_frame = cvCreateImage( cvGetSize( input ), 8, 1 );
    IplImage* g_frame = cvCreateImage( cvGetSize( input ), 8, 1 );
    IplImage* b_frame = cvCreateImage( cvGetSize( input ), 8, 1 );

    cvSplit( input , b_frame, g_frame, r_frame, NULL );

    int size[] = {this->histogram_size};
    float range[] = {0, 255 };
    float* ranges[] = {range};

    CvHistogram* r_hist = cvCreateHist(1, size, CV_HIST_ARRAY, ranges, 1);
    CvHistogram* g_hist = cvCreateHist(1, size, CV_HIST_ARRAY, ranges, 1);
    CvHistogram* b_hist = cvCreateHist(1, size, CV_HIST_ARRAY, ranges, 1);

    cvCalcHist(&r_frame, r_hist, 0, 0);
    cvCalcHist(&g_frame, g_hist, 0, 0);
    cvCalcHist(&b_frame, b_hist, 0, 0);

    cvReleaseImage( &r_frame );
    cvReleaseImage( &g_frame );
    cvReleaseImage( &b_frame );

    vector<CvHistogram*> result;

    result.push_back( r_hist );
    result.push_back( g_hist );
    result.push_back( b_hist );

    return result;
}

vector<fann_type*> VisionHistogram::create_predict_mat(IplImage* input){

    vector<fann_type*> result;

    printf("num input %d \n", ann->num_input);

    vector<CvHistogram*> input2 = create_histograms( input );

    for(int x=0; x<(image_width-step_x) ; x++ ){
        for(int y=0; y<(image_height-step_y) ; y++ ){

            fann_type* row = new fann_type[ ann->num_input ];

            int p = 0;

            for(int i=0; i<step_x; i++){
                for(int j=0; j<step_y; j++){
                    CvScalar s = cvGet2D(input, y+j, x+i);
                    for(int c=0; c<channels; c++){
                        row[ p ] = s.val[c];
                        p++;
                    }
                }
            }

            for(int hi=0; hi<input2.size(); hi++){
                for(int hj=0; hj<this->histogram_size; hj++){
                    float histValue = cvQueryHistValue_1D( input2[hi], hj);
                    row[ p ] = histValue;
                    p++;
                }
            }

            result.push_back( row );
        }
    }

    return result;
}