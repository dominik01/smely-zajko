/* 
 * File:   neural_network2.h
 * Author: myron
 *
 * Created on Piatok, 2010, december 10, 9:45
 */

#ifndef NEURAL_NETWORK2_H
#define	NEURAL_NETWORK2_H

#include "VisionBase.h"

class VisionRegionReduced : public VisionBase {
public:
    int pattern_size;

    VisionRegionReduced(int image_width, int image_height, int step_x, int step_y, int pattern_size);

    int create_training( vector<IplImage*> inputs, vector<CvMat*> outputs, char* out_file );
    int create_random_training( vector<IplImage*> inputs, vector<CvMat*> outputs, int sampleCount, char* out_file );

private:

    IplImage* create_sample_match(IplImage* input);

    vector<fann_type*> create_predict_mat(IplImage* mat);
};

#endif	/* NEURAL_NETWORK2_H */

