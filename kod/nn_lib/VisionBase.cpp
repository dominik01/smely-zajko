
#include <stdio.h>
#include <cv.h>
#include <vector>
#include <opencv/cxcore.h>
//#include <opencv/cxtypes.h>
#include <opencv2/core/types_c.h>

#include "VisionBase.h"

using namespace std;

/*
        PUBLIC
*/

VisionBase::VisionBase(int image_width, int image_height, int step_x, int step_y){
    this->step_x = step_x;
    this->step_y = step_y;
    this->image_width = image_width;
    this->image_height = image_height;

    this->parts_x = image_width / step_x;
    this->parts_y = image_width / step_y;
    channels = 3;

    this->input_neurons = step_x * step_y * 3;
    this->output_neurons = 1;

}

void VisionBase::init( int hidden_neurons, int hidden_layers ){

    unsigned int ls[hidden_layers+2];

    ls[0] = this->input_neurons;
    for(int i=1; i<=hidden_layers; i++){
        ls[i] = hidden_neurons;
    }
    ls[hidden_layers+1] = this->output_neurons;


    ann = fann_create_standard_array( sizeof(ls)/sizeof(int), ls );

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    //this->hidden_neurons = second_layer;
}

void VisionBase::load(const char* filepath){
    ann = fann_create_from_file( filepath );
}

void VisionBase::save(const char* filepath){
    fann_save( ann, filepath );
}

void VisionBase::close(){
    fann_destroy(ann);
}


double VisionBase::train(const float desired_error, const unsigned int max_epochs, char* train_file ){

    const unsigned int epochs_between_reports = 10;

    fann_reset_MSE( ann );

    fann_train_on_file(ann, train_file, max_epochs, epochs_between_reports, desired_error);

    return fann_get_MSE(ann);
}

CvMat* VisionBase::predict(IplImage* inputs){//predicts each pixel

    vector<fann_type> calc_out;
    vector<fann_type*> input_array;

    input_array = create_predict_mat( inputs );

    vector<fann_type*>::iterator it;
    for ( it=input_array.begin() ; it != input_array.end(); it++ ){

        fann_scale_input(ann, (*it));
        fann_type* p = fann_run(ann, (*it) );
        //printf( ">> d:%f ad:%d\n", p[0], p );
        calc_out.push_back( p[0] );

        delete[] (*it);
    }

    CvMat* mat = create_out_mat( calc_out );

    input_array.clear();
    calc_out.clear();

    return mat;
}


double VisionBase::test( char* train_file){

    fann_reset_MSE( ann );

    fann_train_data* data = fann_read_train_from_file( train_file );

    fann_test_data( ann, data);

    fann_destroy_train( data );

    return fann_get_MSE(ann);
}

/*
 standard training
 */

int VisionBase::create_training( vector<IplImage*> inputs, vector<CvMat*> outputs, char* out_file ) {

    FILE* outFile = fopen( out_file, "wt");
    if(outFile == NULL ){
        return 0;
    }

    int positives = 0;

    //head
    fprintf(outFile, "%d %d %d\n", inputs.size()*parts_x*parts_y, this->input_neurons, this->output_neurons );

    for (int k = 0; k < inputs.size(); k++) {
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
                fprintf(outFile, "\n");

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

//sampleCount means how many random samples are taken from one input image
int VisionBase::create_random_training( vector<IplImage*> inputs, vector<CvMat*> outputs, int sampleCount,  char* out_file ) {

    FILE* outFile = fopen(out_file, "wt");
    if(outFile == NULL ){
        return 0;
    }

    int positives = 0;

    //head
    fprintf(outFile, "%d %d %d\n", inputs.size()*sampleCount, this->input_neurons, this->output_neurons );

    for(int k=0; k<inputs.size(); k++ ){
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
            fprintf(outFile, "\n");

            double val = (double)positivePixels/(step_x*step_y);
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

void VisionBase::scale_data(char* file, char* out_file){
    
    fann_train_data* data = fann_read_train_from_file( file );

    fann_set_scaling_params( ann, data, 0.0f, 1.0f, 0.0f, 1.0f );

    fann_scale_train(ann, data);

    fann_save_train(data, out_file);
}

void VisionBase::divide_data(char* input_file, char* train_file, char* test_file, char* val_file){

    fann_train_data* data = fann_read_train_from_file( input_file );

    int length = fann_length_train_data( data );

    int test_num = 0.3 * length; //30%
    int val_num = 0.1 * length; //10%
    int train_num = length - test_num - val_num;

    fann_shuffle_train_data( data );

    fann_train_data* train_data = fann_subset_train_data(data, 0, train_num);
    fann_save_train( train_data, train_file);
    fann_destroy_train( train_data );

    fann_train_data* test_data = fann_subset_train_data(data, train_num, test_num);
    fann_save_train( test_data, test_file);
    fann_destroy_train( test_data );

    fann_train_data* val_data = fann_subset_train_data(data, train_num+test_num , val_num);
    fann_save_train( val_data, val_file);
    fann_destroy_train( val_data );

    fann_destroy_train( data );
}

/*
        PROTECTED
*/

//divides image into training samples
vector<fann_type*> VisionBase::create_predict_mat(CvArr* mat){

    vector<fann_type*> result;


 //   for(int x=0; x<(image_width-step_x) ; x++ ){
 //       for(int y=0; y<(image_height-step_y) ; y++ ){
   for(int x=0; x<(image_width-step_x) ; x+=2 ){
        for(int y=0; y<(image_height-step_y) ; y+=2 ){

            fann_type* row = new fann_type[ ann->num_input ];

            for(int i=0; i<step_x; i++){
                for(int j=0; j<step_y; j++){
                    CvScalar s = cvGet2D(mat, y+j, x+i);
                    for(int c=0; c<channels; c++){
                        row[ c+((i + (j*step_x))*channels) ] = s.val[c];
                    }
                }
            }

            result.push_back( row );
        }
    }

    return result;
}

CvMat* VisionBase::create_out_mat(vector<fann_type> calc_out){

    CvMat* result = cvCreateMat( image_height, image_width, CV_32F );
    cvSetZero( result );

    int k = 0;
    int w4 = image_height-step_y;
    for(int j=0; j<(image_width-step_x); j++){
        for(int i=0; i<(image_height-step_y); i++){
        //    printf("calc: %d %d %d %d %d %d %d \n", calc_out.size(), i, j, k, k - (j+1)/2 * w4 - j / 2 * w4 - (i +1) / 2,
        //             (image_width-step_x), (image_height-step_y));

//            double d = calc_out.at(k - (j+1)/2 * w4 - j / 2 * w4 - (i + 1)/ 2);
            double d = calc_out.at( (k - (j+1)/2 * w4 )/ 2);

//            double d = calc_out.at(k);

            d = d*2; //weight

            if(d<0){
                d=0;
            }else if(d>1){
                d=1;
            }
            //printf( "5 x:%d  y:%d d:%f\n", j, i, d );
            cvSetReal2D(result,i,j,d);
            k++;
        }
    }

    return result;
}
