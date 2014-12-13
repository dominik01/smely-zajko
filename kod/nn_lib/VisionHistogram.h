/* 
 * File:   Neural_Network4.h
 * Author: myron
 *
 * Created on Pondelok, 2011, február 28, 14:58
 */

#ifndef NEURAL_NETWORK4_H
#define	NEURAL_NETWORK4_H

#include "VisionBase.h"

class VisionHistogram : public VisionBase {
public:

    int histogram_size;

    VisionHistogram(int image_width, int image_height, int step_x, int step_y, int histogram_size);

    int create_training( vector<IplImage*> inputs, vector<CvMat*> outputs, char* out_file );
    int create_random_training( vector<IplImage*> inputs, vector<CvMat*> outputs, int sampleCount, char* out_file );

private:

    vector<CvHistogram*> create_histograms(IplImage* input);

    vector<fann_type*> create_predict_mat(IplImage* mat);
};

#endif	/* NEURAL_NETWORK4_H */

