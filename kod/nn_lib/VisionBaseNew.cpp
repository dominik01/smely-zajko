
#include "VisionBaseNew.h"

using namespace std;

//in_x,y je velkost vstupov, out velkost vystupov, in >= out

VisionBase::VisionBase(int image_width, int image_height, int step_x, int step_y, int in_x, int in_y, int out_x, int out_y,bool kontext) {
    this->step_x = step_x;
    this->step_y = step_y;
    this->in_x = in_x;
    this->in_y = in_y;
    this->out_x = out_x;
    this->out_y = out_y;
    this->image_width = image_width;
    this->image_height = image_height;
    this->context = kontext;

    this->out_width = (int)((image_width - (in_x - step_x))/step_x);
    this->out_height = (int)((image_height - (in_y - step_y))/step_y);
    
    channels = 3; //RGB

    this->input_neurons = in_x * in_y * 3;
    if(context)
        this->input_neurons = this->input_neurons + 3;
    this->output_neurons = out_x * out_y;

}
//init nn

void VisionBase::init(int hidden_neurons, int hidden_layers) {

    unsigned int ar[hidden_layers + 2];

    ar[0] = this->input_neurons;
    for (int i = 1; i <= hidden_layers; i++) {
        ar[i] = hidden_neurons;
    }
    ar[hidden_layers + 1] = this->output_neurons;


    ann = fann_create_standard_array(sizeof (ar) / sizeof (int), ar);

    //TODO poskusat ine ako FANN_SIGMOID napr gausian sa zda vhodny
    fann_set_activation_function_hidden(ann, FANN_SIGMOID);
    fann_set_activation_function_output(ann, FANN_SIGMOID);

}

//vytvori trenovaci subor z obrazkov rovnomerne zvolenymi oknami

int VisionBase::create_training_file_full(vector<IplImage*> inputs, vector< vector<int> >& outputs, char* out_file) {
    //TODO nejak treshold pridaj nehardkodovo

    FILE* outFile = fopen(out_file, "wt");
    if (outFile == NULL) {
        return 0;
    }

    int counte = (1 + ((image_width - in_y) / step_y))*(1 + (image_height - in_y) / step_y);
    fprintf(outFile, "%d %d %d\n",(int) inputs.size() * counte, this->input_neurons, this->output_neurons);

    CvScalar s;
    int pom = 0;
    int res = 0;
    int xo = 0;
    int yo = 0;
    double kl=0,ka=0,kb=0;
    for (int k = 0; k < inputs.size(); k++) {
        //vytvor kontextove veci
        if(context){
            for (int i = 0; i < image_width/4; i++) {
                for (int j = 0; j < image_height/10; j++) {
                    s = cvGet2D(inputs[k], image_height-image_height/10 +j, image_width/2-image_width/8 + i); //get2D ma druhy parameter y a treti x
                    for (int c = 0; c < channels; c++) {
                        kl +=s.val[0]/255;
                        ka +=s.val[1]/255;
                        kb +=s.val[2]/255;
                    } 

                }
            }
            kl /=(image_width/4)*(image_height/10);
            ka /=(image_width/4)*(image_height/10);
            kb /=(image_width/4)*(image_height/10);
        }
        for (int x = 0; x <= (image_width - in_x); x += step_x) {
            for (int y = 0; y <= (image_height - in_y); y += step_y) {
                //vypis vstup
                for (int i = 0; i < in_x; i++) {
                    for (int j = 0; j < in_y; j++) {
                        //TODO get pixel value sa da aj efektivnejsie
                        s = cvGet2D(inputs[k], y + j, x + i); //get2D ma druhy parameter y a treti x
                        for (int c = 0; c < channels; c++) {
                            fprintf(outFile, "%f ", s.val[c]/255);
                        }
                    }
                }
                if(context){
                    fprintf(outFile, "%f ", kl);
                    fprintf(outFile, "%f ", ka);
                    fprintf(outFile, "%f ", kb);
                }
                
                fprintf(outFile, "\n");

                //vypis chceny vystup

                xo = x + in_x / 2 - step_x / 2;
                yo = y + in_y / 2 - step_y / 2;
                //kazdy neuron
                for (int f = 0; f < out_x; f++) {
                    for (int g = 0; g < out_y; g++) {
                        pom = 0;
                        //vsetky pixely neurona
                        for (int d = xo + f * (step_x / out_x); d < xo + (f + 1)*(step_x / out_x); d++) {
                            for (int e = yo + g * (step_y / out_y); e < yo + (g + 1)*(step_y / out_y); e++) {
                              
                                if (outputs[k][image_width*e + d] >0) {
                                    pom++;
                                }
                            }
                        }
                        res = 0;
                        //ak je pomer viac ako 50% neuron ma svietit TODO nech neni nahardkodene takto
                        if (((float)pom / ((step_x / out_x) * (step_y / out_y))) >= 0.5) {
                            res = 1;
                        }
                        fprintf(outFile, "%d ", res);
                        

                    }
                }

                fprintf(outFile, "\n");

            }
        }
    }
    fclose(outFile);
    return 1;
}
//vytvori trenovaci subor z obrazkov nahodne zvolenymi oknami
//TODO urob na vector ako vo full
int VisionBase::create_training_file_partial(vector<IplImage*> inputs, vector< vector<int> >& outputs, char* out_file, int samples) {

    FILE* outFile = fopen(out_file, "wt");
    if (outFile == NULL) {
        return 0;
    }

    fprintf(outFile, "%d %d %d\n",(int) inputs.size() * samples, this->input_neurons, this->output_neurons);

    CvScalar s;
    int pom = 0;
    int res = 0;
    int x = 0;
    int y = 0;
    int xo = 0;
    int yo = 0;
    for (int k = 0; k < inputs.size(); k++) {

        for (int rnc = 0; rnc < samples; rnc++) {
            //get random x, y
            x = (((double) rand() / (double) RAND_MAX)* (image_width - in_x - 1));
            y = (((double) rand() / (double) RAND_MAX)* (image_height - in_y - 1));
            //vypis vstup
            for (int i = 0; i < in_x; i++) {
                for (int j = 0; j < in_y; j++) {
                    //TODO get pixel value sa da aj efektivnejsie
                    s = cvGet2D(inputs[k], y + j, x + i); //get2D ma druhy parameter y a treti x
                    for (int c = 0; c < channels; c++) {
                        fprintf(outFile, "%f ", s.val[c]);
                    }
                }
            }
            fprintf(outFile, "\n");

            //vypis chceny vystup

            xo = x + in_x / 2 - step_x / 2;
            yo = y + in_y / 2 - step_y / 2;
            
            //kazdy neuron
            for (int f = 0; f < out_x; f++) {
                for (int g = 0; g < out_y; g++) {
                    pom = 0;
                    //vsetky pixely neurona
                    for (int d = xo + f * (step_x / out_x); d < xo + (f + 1)*(step_x / out_x); d++) {
                        for (int e = yo + g * (step_y / out_y); e < yo + (g + 1)*(step_y / out_y); e++) {
                            //TODO nepassuj cvmat ale len vector a getni ho width*e +d
                            
                            if (outputs[k][image_width*e + d]>0) {
                                pom++;
                            }
                        }
                    }
                    res = 0;
                    //ak je pomer viac ako 50% neuron ma svietit treshold
                    if ((pom / (step_x / out_x * step_y / out_y)) >= 0.5) {
                        res = 1;
                    }
                    fprintf(outFile, "%d ", res);

                }
            }

            fprintf(outFile, "\n");


        }
    }
    fclose(outFile);
    return 1;
}

//train on training file

double VisionBase::train(const float desired_error, const unsigned int max_epochs, char* train_file) {
    const unsigned int epochs_between_reports = 5;
    fann_reset_MSE(ann);

    fann_train_on_file(ann, train_file, max_epochs, epochs_between_reports, desired_error);

    return fann_get_MSE(ann);
}

//test returns MSE of data

double VisionBase::test(char* train_file) {

    fann_reset_MSE(ann);

    fann_train_data* data = fann_read_train_from_file(train_file);

    fann_test_data(ann, data);

    fann_destroy_train(data);

    return fann_get_MSE(ann);
}

//process image with nn

CvMat* VisionBase::predict(IplImage* inputs) {

    vector<fann_type*> input_array;
    vector<fann_type> calc_out;
    int pom = 0;int kk=0;
    double kl=0,ka=0,kb=0;
    CvScalar s;
    
    if(context){
        for (int i = 0; i < image_width/4; i++) {
            for (int j = 0; j < image_height/10; j++) {
                s = cvGet2D(inputs, image_height-image_height/10 +j, image_width/2-image_width/8 + i); //get2D ma druhy parameter y a treti x
                for (int c = 0; c < channels; c++) {
                    kl +=s.val[0]/255;
                    ka +=s.val[1]/255;
                    kb +=s.val[2]/255;
                } 

            }
        }
        kl /=(image_width/4)*(image_height/10);
        ka /=(image_width/4)*(image_height/10);
        kb /=(image_width/4)*(image_height/10);
    }
    
    for (int y = 0; y <= (image_height - in_y); y += step_y) {
        for (int x = 0; x <= (image_width - in_x); x += step_x) {
            fann_type* row = new fann_type[ ann->num_input ];

            pom = 0; //pomocny iterator nech je menej instrukcii

            for (int i = 0; i < in_x; i++) {
                for (int j = 0; j < in_y; j++) {
                    //TODO get pixel value sa da aj efektivnejsie
                    s = cvGet2D(inputs, y + j, x + i); //get2D ma druhy parameter y a treti x
                    for (int c = 0; c < channels; c++) {
                        row[ pom ] = s.val[c]/255;
                        pom++;
                    }
                }
            }
            if(context){
                row[ pom ] = kl;
                row[ pom+1 ] = ka;
                row[ pom+2 ] = kb;
            }
            input_array.push_back(row);
        }
    }
    //TODO vytvor vystup  run vracia array tu je len s 1 out neuronom p[0] inak by bolo p[0 - #of neurons]
    //TODO urob to bez input array len v 1 cykle volat hned run takto zbytocne plnim array
    
    vector<fann_type*>::iterator it;
    for (it = input_array.begin(); it != input_array.end(); it++) {

        fann_scale_input(ann, (*it)); //TODO scale to [1,-1] by handwritten function -better lebo fann to robi nespecificky pre problem
        fann_type* p = fann_run(ann, (*it));
        for(int u = 0; u < output_neurons; u++)
            calc_out.push_back(p[u]);

        delete[] (*it);
    }

    input_array.clear();

    CvMat* mat = create_out_mat( calc_out );
    calc_out.clear();

    return mat;
}

CvMat* VisionBase::create_out_mat(vector<fann_type> calc_out){

    CvMat* result = cvCreateMat(  (int)(out_height),(int)(out_width), CV_32F );
    cvSetZero( result );

    int w4 = image_height/step_y;
    int ite=0;
    for (int y = 0; y <= (image_height - in_y); y += step_y) {
        for (int x = 0; x <= (image_width - in_x); x += step_x) { 
    
            double d = calc_out.at(ite);
            ite++;
            d = d*2; //weight

            if(d<0){
                d=0;
            }else if(d>1){
                d=1;
            }
            
            cvSetReal2D(result,(int)(y/step_y),(int)(x/step_x),d);
        }
    }

    return result;
}

//upscalne result tak aby sa dal pouzit ako overlay obrazku
//TODO nejak efektivnejsie sa neda? cez copy alebo nejake memcpy...
 vector<int>  VisionBase::upscale_result(vector<fann_type> v){
    cout<< "upscale "<<v.size()<<"\n";
    vector<int>  res;
    vector<int>  row;
    int itr = 0;
    bool margin = true;
    if(margin){
        //na zaciatok prazdne riadky ktore sa nehodnotili
        for(int s= 0; s< (in_y - step_y)/2; s++){
            for(int z = 0; z< image_width;z++)
                res.push_back(0);        
        }
    } 
    for (int y = 0; y <= (image_height - in_y); y += step_y) {
        if(margin){
            //zaciatok rowu
            for(int s= 0; s< (in_x - step_x)/2; s++){
                row.push_back(0);
            }
        }
        for (int x = 0; x <= (image_width - in_x); x += step_x) {  
            itr++;
            for(int h = 0; h < step_x; h++){
                if(v[itr] > 0.5)
                    row.push_back(1);
                else
                    row.push_back(0);
            }
            
        }
        if(margin){
            //koniec rowu
            for(int s= 0; s< (in_x - step_x) - (int)((in_x - step_x)/2); s++){
                row.push_back(0);
            }
        }
        for(int h = 0; h < step_y; h++){
                for(int k = 0; k < row.size();k++){
                    res.push_back(row[k]);
                }
        }
        row.clear();
    }
    if(margin){
        //na koniec riadky neohodnotene
        for(int s= 0; s< (in_y - step_y) -(int)((in_y - step_y)/2); s++){
            for(int z = 0; z< image_width;z++)
                res.push_back(0);        
        }
    }
    cout<< "upscaled to "<<res.size()<<"\n";
    return res;
   
}

void VisionBase::load(const char* filepath) {
    ann = fann_create_from_file(filepath);
}

void VisionBase::save(const char* filepath) {
    fann_save(ann, filepath);
}

void VisionBase::close() {
    fann_destroy(ann);
}

void VisionBase::scale_data(char* file, char* out_file){
    
    fann_train_data* data = fann_read_train_from_file( file );

    fann_set_scaling_params( ann, data, 0.0f, 1.0f, 0.0f, 1.0f );

    fann_scale_train(ann, data);

    fann_save_train(data, out_file);
}