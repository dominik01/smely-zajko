/* 
 * File:   VisionBase.h
 * Author: mnam
 *
 * Created on April 27, 2014, 9:01 PM
 */

#ifndef NEURAL_NETWORK_H_INCLUDED
#define NEURAL_NETWORK_H_INCLUDED

#include <opencv2/opencv.hpp>
#include <vector>
#include <stdio.h>
#include <fann.h>

using namespace std;

class VisionBase{

public:
    int n_count;

    int image_width,image_height;
    int parts_x,parts_y;
    int step_x, step_y;
    int in_x, in_y;
    int out_x, out_y;
    int out_width,out_height;
    int channels;
    bool context;

    
    int input_neurons;
    int output_neurons;

    VisionBase(int image_width, int image_height, int step_x, int step_y, int in_x, int in_y, int out_x, int out_y,bool kontext);
    void init(int hidden_neurons, int hidden_layers);
    void load(const char* filepath);
    void save(const char* filepath);


    double train( const float desired_error, const unsigned int max_epochs, char* train_file );

    double test(char* train_file);

    CvMat* predict(IplImage* inputs);
    
    void close();

    virtual int create_training_file_full( vector<IplImage*> inputs, vector< vector<int> >& outputs, char* out_file );
    virtual int create_training_file_partial( vector<IplImage*> inputs, vector< vector<int> >& outputs,  char* out_file, int samples );
    virtual  vector<int>  upscale_result(vector<fann_type> v);
    void scale_data(char* file, char* out_file);
protected:

    struct fann *ann;

    //virtual vector<fann_type*> create_predict_mat(CvArr* mat);

    virtual CvMat* create_out_mat(vector<fann_type> calc_out);

};

#endif	/* NEURAL_NETWORK_H_INCLUDED */

