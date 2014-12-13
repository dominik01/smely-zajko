/* 
 * File:   neural_network3.h
 * Author: myron
 *
 * Created on Pondelok, 2011, február 14, 12:28
 */

#ifndef NEURAL_NETWORK3_H
#define	NEURAL_NETWORK3_H

#include "VisionBase.h"

class VisionRegion : public VisionBase {
public:
    int pattern_size;

    VisionRegion(int image_width, int image_height, int parts_x, int parts_y, int pattern_size);

    int create_training( vector<IplImage*> inputs, vector<CvMat*> outputs, char* out_file );
    int create_random_training( vector<IplImage*> inputs, vector<CvMat*> outputs, int sampleCount, char* out_file );

private:

    IplImage* create_sample_match(IplImage* input);

    vector<fann_type*> create_predict_mat(IplImage* mat);
};

#endif	/* NEURAL_NETWORK3_H */

