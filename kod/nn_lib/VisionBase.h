#ifndef NEURAL_NETWORK_H_INCLUDED
#define NEURAL_NETWORK_H_INCLUDED

#include <cv.h>
#include <vector>

#include <fann.h>

using namespace std;

class PredictData{
public:
    fann_type* ft;
    int x;
    int y;
//    ~PredictData(){
//        delete[] ft;
//    }
};

class VisionBase{

public:
    int n_count;

    int image_width,image_height;
    int parts_x,parts_y;
    int step_x, step_y;
    int channels;

    int input_neurons;
    //int hidden_neurons;
    int output_neurons;

    VisionBase(int image_width, int image_height, int step_x, int step_y);
    void init(int hidden_neurons, int hidden_layers);
    void load(const char* filepath);
    void save(const char* filepath);

    //double get_error();

    double train( const float desired_error, const unsigned int max_epochs, char* train_file );

    double test(char* train_file);

    CvMat* predict(IplImage* inputs);
    //CvMat* random_predict(IplImage* inputs, int sampleCount);

    void close();

    virtual int create_training( vector<IplImage*> inputs, vector<CvMat*> outputs, char* out_file );
    virtual int create_random_training( vector<IplImage*> inputs, vector<CvMat*> outputs, int count, char* out_file );

    void normalize_data(char* file, char* out_file);
    void scale_data(char* file, char* out_file);
    void divide_data(char* input_file, char* train_file, char* test_file, char* val_file);

protected:

    struct fann *ann;

    virtual vector<fann_type*> create_predict_mat(CvArr* mat);
    //vector<PredictData> create_random_predict_mat(CvArr* mat, int sampleCount );

    virtual CvMat* create_out_mat(vector<fann_type> calc_out);
    //CvMat* create_random_out_mat(vector<fann_type> calc_out, vector<PredictData> predict);

};
#endif // NEURAL_NETWORK_H_INCLUDED
