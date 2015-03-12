#include "MFilter.h"
using namespace std;

void MFilter::connect(MFilterData * outside) {
    this->outside = outside;
    //MCoordDB * a = new MCoordDB();
    //a->getX();
}

void MFilter::upload(){
    cout << "WARNING, MFilter.upload should not be called";
}

string MFilter::getName(){return "MFilter"; }

int MFilter::run(){
    if (DEBUGGING) cout << "Running Filter check "<<inputs2D.size()<< " "<<inputsDB.size()<<"==="<<endl;
    if ((inputs2D.size() == 0) && (inputsDB.size() == 0)) cout << getName() << " warning: no inputs set."<<endl;
    for (vector<string>::iterator it = inputs2D.begin(); it != inputs2D.end(); ++it){
        if (DEBUGGING) cout << *it << endl;
        //map<string, MImage*>::iterator img = images.find(*it);
        if (!outside->hasImage(*it)){
            cout << "Error in MFilter::run(): a required input image is missing: "<<*it<< ". Did you call Connect()?" <<endl;
            return -1;
        };
    };
    for (vector<string>::iterator it = inputsDB.begin(); it != inputsDB.end(); ++it){
        if (DEBUGGING) cout << *it << endl;
        //map<string, MImage*>::iterator img = images.find(*it);
        if (!outside->hasDB(*it)){
            cout << "Error in MFilter::run(): a required input image is missing: "<<*it<< ". Did you call Connect()?" <<endl;
            return -1;
        };
    };
    if (DEBUGGING) cout <<"MFilter.run() ends"<<endl;
    return 0;
}



vector<string> MFilter::getInputs2D(){
    return inputs2D;
}

vector<string> MFilter::getInputsDB(){
    return inputsDB;
}

vector<string> MFilter::getInputs(){
    vector<string> result = inputs2D;
    result.insert(result.end(), inputsDB.begin(), inputsDB.end());
    return result;
}


void MFilter::printname(){
    cout << getName() <<endl;
}




map<string, MImage*> MFilter2D::lastResult(){
    return results;
}

void MFilter2D::upload(){
        map<string, MImage*>::iterator it;
        if (results.size() == 0)
            cout << getName() << " warning: no results produced." << endl;
        for (it = results.begin(); it != results.end(); ++it){
            outside->setImage((*it).first, (*it).second); //overwrites the existing values.
            if (DEBUGGING) {
                cout << "MFilter2D: upload "<<(*it).first << endl;
				if (!(*it).second)
					cout << "NULL result" << endl;
				else {
					(*it).second->printDebugInfo();
					cout << "red dot: "<< (*it).second->data[2309 + 2500 * 1254] << endl;
				};
            }
        };
        if (DEBUGGING) cout << getName()<< ": update completed"<<endl;
}



string MFilter2D::getName(){
    return "MFilter2D";
}





string MFilterCoordDB::getName(){
    return "MFilterCoordDB";
}


void MFilterCoordDB::upload(){
        if (dbs.size() == 0)
            cout << "MFilterCoordDB: " << getName() << " warning: no results produced." << endl;
        //map<string, MImage*> l = results[output];
        map<string, MCoordDB*>::iterator it;
        for (it = dbs.begin(); it != dbs.end(); ++it){

            if (DEBUGGING) cout << "MFilterCoordDB: upload "<<(*it).first << endl;
            outside->setDB((*it).first, (*it).second); 
            //overwrites the existing values.
        };
        if (DEBUGGING) cout <<"MFilterCoordDB: " << getName()<< ": update completed"<<endl;
        //cout << "update completed"<<endl;
}



void MFilterMixed::upload(){
    MFilter2D::upload();
    MFilterCoordDB::upload();
}


string GaussianFilter::getName(){return "GaussianFilter";};

inline double sqr(double x){
    return x*x;
};


inline long max (const long a, const long b){
    return a < b? b:a;
};

inline long min (const long a, const long b){
    return a < b? a:b;
}

int GaussianFilter::get_template_size(double scale){
    return 2 * get_discrete_radius(radius/scale)+ 1;
}

float* GaussianFilter::get_template(double scale){
    //MImage * img = outside->getImage(inputs2D[0]);
    double sigma = radius / scale;
    int k = get_discrete_radius(sigma);
    double *  values = get_values(k, sigma);
    float *  matrix = new float [(2*k+1) * (2*k+1)];
    for (int i = 0; i < 2*k+1; i++)
        for (int j = 0; j < 2*k+1; j++)
            matrix[j + i * (2*k+1)] = (float) values[i] * values[j];
    delete values;
    return matrix;
}

double * GaussianFilter::get_values(int k, double sigma)
{
    double sv = 0;
    double *  values = new double[2*k+1];
    for (int i = 0; i<2*k+1; i++){
        values[i] = (Phi( (i - k +0.5) / sigma) - Phi( (i - k -0.5) / sigma));  //sqrt(height)??
        // 1/sqrt(2*PI * sigma) * exp(- sqr((i-k)/sigma)); 
        // //height is the height of the curve, h,w are the dimensions of the image
        sv += values[i];
    };

    if (height > 0) { //make middle component exactly height.
        //height = height * sqrt(2 * PI  * sigma);
        double r = sqrt(height) / values[k];
        for (int i = 0; i < 2*k+1; i++)
            values[i] = values[i] * r;
    };

    if (DEBUGGING){
       for (int i = 0; i < 2*k+1; i++)
           cout << values[i]<<" ";
       cout << endl;
    };

    if (DEBUGGING) cout << "| "<<sv<<endl;

    return values;
};

int GaussianFilter::get_discrete_radius(double sigma){
    if (template_size == 0)
        return (int) 1 + 3 * sigma;
    else {
        if (template_size % 2 == 0)
            cout << "Warning: even template size will be reduced by 1";
        return (template_size - 1) /2;
       };
    
 }


//convolution with Gaussian
int GaussianFilter::run(){
    //cout << "Running Gaussian filter" << endl;
    if ((-1 == MFilter::run())) return -1;
    MImage * img = outside->getImage(inputs2D[0]);
    float * data = img->data;
    long h = img->height;
    long w = img->width;
    double scale = img->scale;

    MImage * result = new MImage(w, h, scale);
    MImage * finalresult = new MImage(w, h, scale);

    double sigma = radius / scale;

    int k = get_discrete_radius(sigma);
    if (DEBUGGING) cout<<"Values: ";
    
    double * values = get_values(k, sigma);

    if (DEBUGGING) cout << "Horizontal sweep"<<endl;
    bool alerted = 0;

    //horizontal sweep
    for (int i = 0; i<h; i++)
        for (int j = 0; j < w; j++){
            double s = 0;
            for (int l = 0; l < 2*k+1; l++) {
                int index = (i * w) +  min(w-1, max(0,j+l-k));
                s += data[index] * values[l];
                //float delta;
                //if ((index != i* w) && (abs(delta = data[index] - data[index-1]) > 2)) {
                //    cout << "Producing "<< output<<": abrupt change: "<<data[index-1] << "->"<<data[index]<<" at "<<index <<" i="<<i<<" j="<<j << " l="<<l << endl;
                //  cout << w <<" "<<h<<endl;
                //    exit(-1);
                //};
                if ( (data[index] < -99990.0) && !alerted) {
                    cout << " negative data!! (horizontal) "<<data[index]<<" at "<<index <<" i="<<i<<" j="<<j << endl;
                    alerted = 1;
                }
            }
            result->data[j + i * w] = (float) s;
        };
    alerted = 0;
    if (DEBUGGING) cout << "Vertical sweep"<<endl;
   //vertical sweep
    for (int j = 0; j<w; j++)
        for (int i = 0; i < h; i++){
            double s = 0;
            for (int l = 0; l < 2*k+1; l++){
                int index = j + w * min(h-1, max(0,i+l-k));
                s += result->data[index] * values[l];
                if ( (result->data[index] < -99990.0) && !alerted){
                    cout << "negative data (vertical) "<<result->data[index]<<"at "<<index <<" i="<<i<<" j="<<j << endl;
                    alerted = 1;
                }
                
            }
            finalresult->data[j + i * w] = (float) s;
        };
    delete result;
    delete [] values;
    results[output] = finalresult;
    return 0;
}

SeparableFilter::SeparableFilter(string input, string output, int sizeX, int sizeY, float * templateX, float * templateY, bool normalize): output(output), sizeX(sizeX), sizeY(sizeY), normalize(normalize)
         { inputs2D.push_back(input); 
           this->templateX  = copy_array(templateX, sizeX);
           this->templateY = copy_array(templateY, sizeY);
         };

SeparableFilter::~SeparableFilter(){
    if (templateX)
        delete [] templateX;
    if (templateY)
        delete [] templateY;
}


float * SeparableFilter::step_template(int step_radius, int total_radius){
    int s = total_radius * 2 + 1;
    float * result = new float[s];
    for (int i = 0; i < s; i++)
        result[i] = 0;

    for (int i = total_radius - step_radius; i<= total_radius+step_radius; i++)
        result[i] = 1;
    return result;
}


float * SeparableFilter::constant_template(int size){
    float * result = new float[size];
    for (int i = 0; i < size; i++)
        result[i] = 1;
    return result;
}


//convolution with Gaussian
int SeparableFilter::run(){
    //cout << "Running Gaussian filter" << endl;
    if ((-1 == MFilter::run())) return -1;
    MImage * img = outside->getImage(inputs2D[0]);
    float * data = img->data;
    long h = img->height;
    long w = img->width;
    double scale = img->scale;

    MImage * result = new MImage(w, h, scale);
    MImage * finalresult = new MImage(w, h, scale);

    //double sigma = radius / scale;
    
    //double * values = get_values(k, sigma);
    int k = (sizeX - 1)/2;

    if (DEBUGGING) cout << "Horizontal sweep"<<endl;
    bool alerted = 0;

    float * tX = copy_array(templateX, sizeX);
    if (normalize) normalize_array(templateX, sizeX); 

    //horizontal sweep
    for (int i = 0; i<h; i++)
        for (int j = 0; j < w; j++){
            double s = 0;
            for (int l = 0; l < 2*k+1; l++) {
                int index = (i * w) +  min(w-1, max(0,j+l-k));
                s += data[index] * tX[l];
                //float delta;
                //if ((index != i* w) && (abs(delta = data[index] - data[index-1]) > 2)) {
                //    cout << "Producing "<< output<<": abrupt change: "<<data[index-1] << "->"<<data[index]<<" at "<<index <<" i="<<i<<" j="<<j << " l="<<l << endl;
                //  cout << w <<" "<<h<<endl;
                //    exit(-1);
                //};
                if ( (data[index] < -99990.0) && !alerted) {
                    cout << " negative data!! (horizontal) "<<data[index]<<" at "<<index <<" i="<<i<<" j="<<j << endl;
                    alerted = 1;
                }
            }
            result->data[j + i * w] = (float) s;
        };


    float * tY = copy_array(templateY, sizeY);
    if (normalize) normalize_array(templateY, sizeY); 

    k = (sizeY - 1)/2;
    alerted = 0;
    if (DEBUGGING) cout << "Vertical sweep"<<endl;
   //vertical sweep
    for (int j = 0; j<w; j++)
        for (int i = 0; i < h; i++){
            double s = 0;
            for (int l = 0; l < 2*k+1; l++){
                int index = j + w * min(h-1, max(0,i+l-k));
                s += result->data[index] * tY[l];
                if ( (result->data[index] < -99990.0) && !alerted){
                    cout << "negative data (vertical) "<<result->data[index]<<"at "<<index <<" i="<<i<<" j="<<j << endl;
                    alerted = 1;
                }
                
            }
            finalresult->data[j + i * w] = (float) s;
        };
    delete result;
    delete [] tX;
    delete [] tY;
    results[output] = finalresult;
    return 0;
}



int MinusFilter::run(){
    //cout << "Running Minus filter"<<endl;
    if ((-1 == MFilter::run())) return -1; 
    MImage * img = outside->getImage(inputs2D[0]);
    MImage * img2 = outside->getImage(inputs2D[1]);
    long w = img->width;
    long h = img->height;
    MImage * out = new MImage(w, h,img->scale);

    float * data = out->data;
    for (int i = 0; i < w * h; i++)
        data[i] = img->data[i] - img2->data[i];
    results[output] = out;
    return 0;
}

string MinusFilter::getName(){
    return "MinusFilter";
}


int ArithmeticFilter::run(){
    //cout << "Running Arithmetic filter"<<endl;
    if ((-1 == MFilter::run())) return -1; 
    MImage * img1, *img2, *img3, *img4, *img5;
    img1 = img2 = img3 = img4 = img5 = NULL;
    //if (inputs2D.size() > 0) 
    img1 = outside->getImage(inputs2D[0]);
    if (inputs2D.size() > 1) img2 = outside->getImage(inputs2D[1]);
    if (inputs2D.size() > 2) img3 = outside->getImage(inputs2D[2]);
    if (inputs2D.size() > 3) img4 = outside->getImage(inputs2D[3]);
    if (inputs2D.size() > 4) img5 = outside->getImage(inputs2D[5]);

    long w = img1->width;
    long h = img1->height;
    MImage * out = new MImage(w, h,img1->scale);
    float * data = out->data;


    //15 cases ... 
    if (arity == 1)
        for (int i = 0; i < w * h; i++)
            data[i] = f1(img1->data[i]);
    else if (arity ==2 && inputs2D.size()==2)
        for (int i = 0; i < w * h; i++)
            data[i] = f2(img1->data[i], img2->data[i]);
    else if (arity ==2 && inputs2D.size()==1)
        for (int i = 0; i < w * h; i++)
            data[i] = f2(img1->data[i], c2);
    else if (arity ==3 && inputs2D.size()==3)
        for (int i = 0; i < w * h; i++)
            data[i] = f3(img1->data[i], img2->data[i], img3->data[i]);
    else if (arity ==3 && inputs2D.size()==2)
        for (int i = 0; i < w * h; i++)
            data[i] = f3(img1->data[i], img2->data[i], c3);
    else if (arity ==3 && inputs2D.size()==1)
        for (int i = 0; i < w * h; i++)
            data[i] = f3(img1->data[i], c2, c3);
    else if (arity ==4 && inputs2D.size()==4)
        for (int i = 0; i < w * h; i++)
            data[i] = f4(img1->data[i], img2->data[i], img3->data[i], img4->data[i]);
    else if (arity ==4 && inputs2D.size()==3)
        for (int i = 0; i < w * h; i++)
            data[i] = f4(img1->data[i], img2->data[i], img3->data[i], c4);
    else if (arity ==4 && inputs2D.size()==2)
        for (int i = 0; i < w * h; i++)
            data[i] = f4(img1->data[i], img2->data[i], c3, c4);
    else if (arity ==4 && inputs2D.size()==1)
        for (int i = 0; i < w * h; i++)
            data[i] = f4(img1->data[i], c2, c3, c4);
    else if (arity ==5 && inputs2D.size()==5)
        for (int i = 0; i < w * h; i++)
            data[i] = f5(img1->data[i], img2->data[i], img3->data[i], img4->data[i], img5->data[i]);
    else if (arity ==5 && inputs2D.size()==4)
        for (int i = 0; i < w * h; i++)
            data[i] = f5(img1->data[i], img2->data[i], img3->data[i], img4->data[i], c5);
    else if (arity ==5 && inputs2D.size()==3)
        for (int i = 0; i < w * h; i++)
            data[i] = f5(img1->data[i], img2->data[i], img3->data[i], c4, c5);
    else if (arity ==5 && inputs2D.size()==2)
        for (int i = 0; i < w * h; i++)
            data[i] = f5(img1->data[i], img2->data[i], c3, c4, c5);
    else if (arity ==5 && inputs2D.size()==1)
        for (int i = 0; i < w * h; i++)
            data[i] = f5(img1->data[i], c2,c3,c4,c5);

    results[output] = out;
    return 0;

}

void GapFillFilter::fill_backwards(float * list, long len){
    if (len < 1) return;
    float a = list[len-1];
    for (int i = len-2; i >= 0; i--){
        float b = list[i];
        if (b < -99990.0) list[i] = a;
        else a = b;
    };
}

void GapFillFilter::fill_forwards(float * list, long len){
    if (len < 1) return;
    float a = list[0];
    for (int i = 1; i < len; i++){
        float b = list[i];
        if (b < -99990.0){
            if (DEBUGGING) cout << "*";
            list[i] = a;
        }
        else a = b;
    };
}


int GapFillFilter::run(){
    MImage * img = outside->getImage(inputs2D[0]); //TODO: remove images) // images[inputs2D[0]];
    //cout << img << endl;
    float * data = img->data;
    long w = img->width;
    long h = img->height;
    long w2 = w/2;
    long h2 = h/2;
    float a = data[w2 + h * h2];
    if (a < -99990.0) cout << "FATAL ERROR: CENTER POINT HAS NO DATA"<<endl;
    for (int i = h2-1; i >= 0; i--){ //fill values from the center point upwards
        float b = data[w2 + h * i];
        if (b < -99990.0) data[w2 + h*i] = a;
        else a = b;
    };
    a = data[w2 + h * h2];
    for (int i = h2+1; i < h; i++){ //fill values from the center point upwards
        float b = data[w2 + h * i];
        if (b < -99990.0) data[w2 + h*i] = a;
        else a = b;
    };
    for (int i = 0; i < h; i++){
        fill_backwards(data + h*i, w2+1);
        fill_forwards(data + h*i + w2, 1+w-w2);
    }
    if (DEBUGGING) cout <<"ends"<<endl;
    results[inputs2D[0]] = img;
    return 0;
}


string GapFillFilter::getName(){
    return "GapFillFilter";
}


int NormalizeFilter::run() {
    MImage * img = outside->getImage(inputs2D[0]); //TODO: remove images) // images[inputs2D[0]];
    //cofixed_mask;
    float * data = img->data;
    long w = img->width;
    long h = img->height;
    double S = 0.0;
    for (int i = 0; i < w * h; i++)
        S += data[i];
    double mu = S / ( w * h);
    for (int i = 0; i < w * h; i++)
        data[i] = data[i] - mu;
    results[inputs2D[0]] = img; 
    return 0;
}

int SetEdgesFilter::run() {
    MImage * img = outside->getImage(inputs2D[0]); //TODO: remove images) // images[inputs2D[0]];
    //cout << img << endl;
    float * data = img->data;
    long w = img->width;
    long h = img->height;
    if (DEBUGGING) cout << "SetEdges: " << margin << endl;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            if ( (i < margin) || (j < margin) || ((h - i) <= margin) || ((w - j) <= margin)){
                //cout << "(" << i << ", " <<j<< ")" << " ";
                data[j + i * w] = value;
            };
    //cout << endl;
    results[inputs2D[0]] = img; 
    return 0;
}


//map<string, MImage*> MFilterCoordDB::lastDBS(){
//    return dbs;
//}


//design patterns (?)


MaskFilter::MaskFilter(int size, float * mask, std::string input,
			  std::string output, double (*f)(int, float*, float*, float*), float * weights)
{                       
    this->size = size;
    this->output = output;
    this->f = f;
    inputs2D.push_back(input);
    int N = (2 * size - 1) * (2 * size - 1);
    this->mask = new float[N];
    this->weights = new float[N];
    for (int i = 0; i < N; i++){
        this->mask[i] = mask[i];
        this->weights[i] = (weights == NULL) ? 1.0 : weights[i];
    };
};


MaskFilter::~MaskFilter(){
    delete mask;
    delete weights;
}




int MaskFilter::run(){
    //cout << "Running Mask filter" << endl;
    if (DEBUGGING) {
        cout << "size: "<<size << endl;
        print_matrix("mask", mask, 2 * size - 1, 2 * size - 1);
    };
    if ((-1 == MFilter::run())) return -1;
    MImage * img = outside->getImage(inputs2D[0]);
    float * data = img->data;
    long h = img->height;
    long w = img->width;
    int w_mask = 2 * size - 1;
    double scale = img->scale;
     
    MImage * result = new MImage(w, h, scale);

    int sz = w_mask * w_mask;
    
    //double weight_sum = 0;
    //for (int i = 0; i < w_mask * w_mask; i++)
    //    weight_sum += weights[i];
    float rect[w_mask * w_mask];
    double nan = NAN;

    for (int i = 0; i < h; i++)
	  for (int j = 0; j < w; j++){
		 for (int k = 1-size; k < size; k++)
			for (int l = 1-size; l < size; l++){
				//double x_img;
                int i_mask = size - 1 + l + (size + k - 1) * w_mask;
		 		if ((i+k < 0) | (i+k >= h) | (j+l < 0) | (j+l >=w))
                   //continue; //if out of bounds: do not add..
                   rect[i_mask] = nan; //assumed 0 outside
                else 
                   rect[i_mask] = data[w * (i+k) + j+l];
				//S += weights[i_mask] * f(mask[i_mask], x_img); //old mask sum filter: f -> -abs(x+y), convolution: f->x*y
                //W +=weights[i_mask];
			};
		 result->data[j + i * w] = f(sz, rect, mask, weights); // normalize? (S/W):S;
         /* if (i == 20 && j == 20) {
             cout << "i = 10, j = 20" << endl;
             print_matrix("rect", rect, w_mask, w_mask);
             print_matrix("mask", mask, w_mask, w_mask);
             print_matrix("weights", weights, w_mask, w_mask);
             cout << "result: "<<result->data[j + i * w] << endl;
             cout << "sz: "<<sz << endl;
             //exit (1); 
         } */
	  };
   results[output] = result;
   return 0;
}


string MaskFilter::getName(){
    return "MaskFilter";
}

int MaskFilter::get_mask_size(){
    return 2 * size - 1;
}

float * MaskFilter::get_mask(){
    int N = (2*size - 1) * (2 * size - 1);
    float * result = new float [N];
    for (int i = 0 ; i < N; i++)
        result[i] = mask[i];
    return result;
}

float * MaskFilter::get_weights(){
    int N = (2*size - 1) * (2 * size - 1);
    float * result = new float [N];
    for (int i = 0 ; i < N; i++)
        result[i] = weights[i];
    return result;
}


MaskFilter * MaskFilter::createGaussianMaskFilter(string input, string output, double sigma, double r, double height, int template_size, bool normalize) {
    GaussianFilter * dummy = new GaussianFilter(sigma, "", "", height, template_size);

    float * tmpl = dummy->get_template(2.0); //scale hardcoded here.
    int sz = dummy->get_template_size(2.0);

    float weights[sz * sz];
    for (int i = 0; i < sz; i++){
        for (int j = 0; j < sz; j++)
        {
            double x = i - sz / 2;
            double y = j - sz / 2;
            weights[j + sz * i] = (x*x + y*y <= r * r) ? 1.0 : 0.0;
        }
    };

    //cout << "4"<<endl;
    //print_matrix("weights", weights, sz, sz);
    //print_matrix("template", tmpl, sz, sz);

    MaskFilter * result = new MaskFilter(1 + sz/2, tmpl, input, output, normalize?product_f_normalized:product_f, weights);
    delete tmpl;
    delete dummy;
    return result;
}


MaskFilter * MaskFilter::createCircularMaskFilter(string input, string output, double r, int template_size, bool normalize) {
    int sz = template_size;
    float mask[sz * sz];
    for (int i = 0; i < sz; i++){
        for (int j = 0; j < sz; j++)
        {
            double x = i - sz / 2;
            double y = j - sz / 2;
            mask[j + sz * i] = (x*x + y*y <= r * r) ? 1.0 : 0.0;
        }
    };
    MaskFilter * result = new MaskFilter(1 + sz/2, mask, input, output, normalize?product_f_normalized:product_f, mask);
    return result;
}

double MaskFilter::product_f(int n, float * data, float * mask, float * weights){
    double S = 0;
    for (int i = 0; i < n; i++) 
        S += data[i] * mask[i] * weights[i];
    return S;
}



double MaskFilter::product_f_normalized(int n, float * data, float * mask, float * weights){
    double S = 0;
    double W = 0;
    for (int i = 0; i < n; i++){
        S += data[i] * mask[i] * weights[i];
        W += weights[i];
    }
    return S/W;
}

//Not reusing computations but simple.
double MaskFilter::correlation_f(int n, float * data, float * mask, float * weights){
    float ndata[n];
    for (int i =0; i < n; i++)
        ndata[i] = data[i]-data[0]; //normalize to avoid large values
    double S_XY = 0;
    double S_XX = 0;
    double S_YY = 0;
    double S_X = 0;
    double S_Y = 0;
    double W = 0;
    for (int i = 0; i < n; i++){
        S_XX += ndata[i] * ndata[i] * weights[i];
        S_XY += ndata[i] * mask[i] * weights[i];
        S_YY += mask[i] * mask[i] * weights[i];
        S_X += ndata[i] * weights[i];
        S_Y += mask[i] * weights[i];
        W += weights[i];
    };
    S_XX = S_XX/W;
    S_XY = S_XY/W;
    S_YY = S_YY/W;
    S_X = S_X/W;
    S_Y = S_Y/W;

    return  (S_XY - S_X * S_Y) / sqrt((S_XX - S_X * S_X) * (S_YY - S_Y * S_Y));
}


double MaskFilter::L1_dist_f(int n, float * data, float * mask, float * weights){
    double S = 0;
    for (int i = 0; i < n; i++) 
        S += -abs(data[i] - mask[i]);
    return S;
}

double MaskFilter::L1_dist_f_normalized(int n, float * data, float * mask, float * weights){
    double S = 0, W = 0;
    for (int i = 0; i < n; i++) {
        S += - weights[i] * abs(data[i] - mask[i]);
        W += weights[i];
    };
    return S/W;
}

//TODO!! quick version, works only with data entries in {0,1}.
double MaskFilter::weighted_median(int n, float * data, float * weights){
    float data2[n];
    int i = 0;
    for (int j = 0; j < n; j++)
        if (weights[j] > 0.5) data2[i++] = data[j];
    vector<float> v(data2, data2 + i);
    sort(v.begin(), v.end());    //hmm... may slow down..
    //for (int k = 0; k < i; k++)
    //    cout <<"el "<<k<<" "<<v[i];
    //cout << endl;
    return v[i/2];
    //output median of data:
}

double MaskFilter::L1_median_f(int n, float * data, float * mask, float * weights){
    float diff[n];
    for (int i = 0; i < n; i++)
        diff[i] = data[i] - mask[i];
    double median = weighted_median(n, diff, weights);
    //cout << median <<" ";
    double S = 0, W = 0;
    for (int i = 0; i < n; i++) {
        S += weights[i] * abs(diff[i] - median);
        W += weights[i];
    };
    return -S/W;
}

double MaskFilter::L2_avg_f(int n, float * data, float * mask, float * weights){
    double diff[n];
 
    double S = 0, W = 0;
    for (int i = 0; i < n; i++){
        diff[i] = data[i] - mask[i];
        S+= weights[i] * diff[i];
        W += weights[i];
    }
    double wmean = S/W;

    double S2 = 0;
    for (int i = 0; i < n; i++) {
        S2 += weights[i] * sqr(diff[i] - wmean);
    };
    return -S2/W;
}




/*
MaskFilter::MaskFilter(int size, double sigma, double height, string input, string output){
	mask = new double [(2*size-1) * (2*size - 1)];
	for (int i = 1 - size; i < size; i++){
		for (int j = 1 - size; j < size; j++){
			mask[size - 1 + j + (2 * size - 1) * (size + i-1)] = height * exp (- (i*i + j * j)/ (sqr(sigma)));
			cout << mask[size - 1 + j + (2*size - 1) * (size+i-1)] << " ";
		};
		cout << endl;
	};
	this->size = size;
	this->output = output;
	inputs2D.push_back(input);
}
*/


int ThresholdFilter::run() {
    //cout << "Running Threshold filter" << endl;
    if ((-1 == MFilter::run())) return -1;
    MImage * img = outside->getImage(inputs2D[0]);
    float * data = img->data;
    long h = img->height;
    long w = img->width;
    double scale = img->scale;
     
    MImage * result = new MImage(w, h, scale);

    if (DEBUGGING) cout << "mid_new:"<< mid_new << endl;

    if (change_mid)
    	for (int i = 0; i < w * h; i++)
		if (data[i] <= lo) result->data[i] = lo_new;
	    	else if (data[i] >= hi) result->data[i] = hi_new;
		else result->data[i] = mid_new;
    else //uses optimal number of checks...
	for (int i = 0; i < w * h; i++)
		if (data[i] <= lo) result->data[i] = lo_new;
	    	else if (data[i] >= hi) result->data[i] = hi_new;
		else result->data[i] = data[i];
    results[output] = result;
    return 0;
}

bool neighbour(int i, int j, int k, int l){
	return abs(i-k) + abs(j-l) < 30;
}

/*
int ConnectedComponentsFilter::bfs_label_component(float * data, int i, int j, long h, long w, int label){
	queue<long> S;
	queue<long> all;
	S.push(j + w * i);
	//mark (i,j) gray
	data[j + w * i] = -0.5;
	int size = 0;
	while (S.size() != 0) {
		long x = S.front(); S.pop();
		int ii = x / w;
		int jj = x % w;
		data[x] = 1;  //=label
		all.push(x);
		size++;
		//search for neighbours
		for (int k = -1; k <= 1; k++)
			for (int l = -1; l<=1; l++)
				if ((abs(k) + abs(l)) == 1) { //4 neighbour model. for 8 neighb: != 0.
					int i1 = ii + k;
					int j1 = jj + l;
					long nn = 0;
					if ((i1 >= 0) && (i1 <h) && (j1 >=0) && (j1<w)
						       	&& (data[nn=j1 + w * i1] < -0.75) ){ //unexplored
						data[nn] = -0.5;
						S.push(nn);
					};

				};
	};

	//if ( neighbour(i,j,1565, 1295) || neighbour (i,j,1320,1379))
    //    	cout << "a component of size "<<size<< " at "<<i << ", " << j << endl;
	
	if ( (size >= 4) && (size <= 20)){
		//cout << "a component of size "<<size<< " at "<<i << ", " << j << endl;
		while (all.size() > 0){
			data[all.front()] = 2.0;
			all.pop();
		};
	};
	return size;
}
*/


MBlobObjectLocal * ConnectedComponentsFilter::bfs_label_component(float * data, int i, int j, long h, long w, int label, float scale){
    //cout << "Enter CCF"<<endl;
	queue<int32_t> S;
	queue<int32_t> all;
	S.push(j + w * i);
	data[j + w * i] = label; 
    int size = 0;

    int maxi=-1, mini=h, maxj=-1, minj=w; //rectangle boundaries

	while (S.size() != 0) {
		int32_t x = S.front(); S.pop();
		int ii = x / w;
		int jj = x % w;

        //cout <<ii << " "<<jj<<endl;
        if (ii > maxi) maxi = ii;
        if (ii < mini) mini = ii;
        if (jj > maxj) maxj = jj;
        if (jj < minj) minj = jj;

		//data[x] = 1;  //=label
		all.push(x);
		size++;
		for (int k = -1; k <= 1; k++)
			for (int l = -1; l<=1; l++)
				if ((abs(k) + abs(l)) == 1) { //4 neighbour model. for 8 neighb: != 0.
					int i1 = ii + k;
					int j1 = jj + l;
					int32_t nn = 0;
					if ((i1 >= 0) && (i1 <h) && (j1 >=0) && (j1<w)
						       	&& (data[nn=j1 + w * i1] < -1.5) ){ //unexplored
						data[nn] = label;
						S.push(nn);
					};

				};
	};

    //cout << "completed bfs"<<endl;

    /*
    if (  (size < min_area) || (size > max_area)  ) {
      //  cout << "Object size not good"<<endl;
        return NULL; //skip object creation
    }; */
    
	//while (all.size() > 0){
	//		data[all.front()] = label;
	//		all.pop();
	//};

	MBlobObjectLocal * result = new MBlobObjectLocal();
    //cout << "Creating object: "<<i << " " << j << endl;
    double_coord coord = get_LKS_coords(sheet, i, j);
    //cout << "Obtained coord"<<endl;
    result->LKS_x = coord.first;
    result->LKS_y = coord.second;

    //result->i = i;
    //result->j = j;
    result->id = label;
    result->coord = j + w * i;
    result->n_coords = size;
    result->area = size * scale * scale;
    result->mini = mini;
    result->maxi = maxi;
    result->minj = minj;
    result->maxj = maxj;
    result->coords = new int32_t[size];

    for (int k = 0; k < size; k++){
        result->coords[k] = all.front();
        all.pop();
    };
    /* if (i==0 && j == 466) {
        result->print(cout); 
        cout << endl;
        cout << size << endl;
        cout << all.size() << endl;
        cout << S.size() << endl;
        exit (1);
    };  */
    //cout << "Created"<< endl;
    return result;
}



int ConnectedComponentsFilter::run(){
    //cout << "Running Connected Components filter" << endl;
    if ((-1 == MFilter::run())) return -1;
    MImage * img = outside->getImage(inputs2D[0]);
    float * data = img->data;
    long h = img->height;
    long w = img->width;
    double scale = img->scale;
    //double sscale = scale * scale;
     
    MImage * result = new MImage(w, h, scale);

    long N = w * h;

    for (int i = 0; i < N; i++)                  //this should actually be a separate filter
	    result->data[i] = (data[i] > threshold) ? -2: -1; // -1 - empty,  -2 - unexplored

    //1, 2, ... will be component numbers
    int current_label = 0;
    //int selected_count = 0;
    MCoordDB * resultDB = new MCoordDB();
    for (int i = 0; i < h; i++)
	    for (int j = 0; j < w; j++){
		    if (result->data[j + w * i] > -1.5) continue;
            //cout << "New component" << i <<" "<<j<< endl;
		    //MBlobObjectLocal * r = bfs_label_component(result->data, i, j, h, w, current_label++);
		    MBlobObjectLocal * r = bfs_label_component(result->data, i, j, h, w, current_label,scale);
            resultDB->append(r);
            current_label++;

            /*
            //cout << "Back: "<<r<<endl;
            if (r != NULL) {
                //cout << selected_count << endl;
                r->id = selected_count++;  //important: id should match the position of the record.
                //r->area *= sscale;         //now in square meters
                resultDB->append(r);
            };*/
	    };
    results[outputImg] = result; 
    dbs[outputDB] = resultDB;
    cout << current_label << " connected component(s)."<<endl;
    return 0;
}


int MaxValuesFilter::run()
{
    if ((-1 == MFilter::run())) return -1;
    MImage * img = outside->getImage(inputs2D[0]);
    float * data = img->data;
    long h = img->height;
    long w = img->width;

    //cout <<"min2, max2: " << min2<< ", " <<max2<<endl;
    bool two_inputs = (inputs2D.size() >= 2);
    if (DEBUGGING) cout << (two_inputs?"two inputs2D":"one input") << endl;
    float * data2 = NULL;
    if (two_inputs) data2 = outside->getImage(inputs2D[1])->data;
    //double scale = img->scale;
    MCoordDB * result = new MCoordDB();
    vector<MCoordDBElement*> records; 
    make_heap(records.begin(), records.end(), MCoordDBElementPoint::larger);
    int removed = 0;
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            if (data[j+ w*i] > threshold){
                double_coord lks = get_LKS_coords(sheet, i, j);

                //if (abs (lks.first - 608052) + abs (lks.second - 6091722) < 20)  //TEMP
                //    cout << lks.first << ","<<lks.second << ": correlation=" <<data[j+w*i] <<" height="<<data2[j+w*i] << endl;

                //cout << data2[j+w*i] << " ";
                if (two_inputs && ((data2[j + w*i] < min2) || (data2[j+w*i] > max2))) continue;
                //result->append(new MCoordDBElementPoint(lks, data[j + w * i], comment));
                if ( int(records.size()) >= max_elements) {
                    if ( ((MCoordDBElementPoint*)records.front())->data >= data[j + w * i]) continue; //at the front is the smallest element
                    else {
                        removed++;
                        pop_heap(records.begin(), records.end(), MCoordDBElementPoint::larger);
                        records.pop_back();
                    };
                };
                records.push_back(new MCoordDBElementPoint(lks, data[j + w * i], comment));
                push_heap(records.begin(), records.end(), MCoordDBElementPoint::larger);
                
            };
    sort_heap(records.begin(), records.end(), MCoordDBElementPoint::larger);
    result->setData(records);
    if (int(records.size()) > max_elements) {
        if (DEBUGGING) cout << getName() << ": retained " << max_elements << " and discarded: " << removed << "values"<<endl;
        records = vector<MCoordDBElement*>(records.begin(), records.begin() + max_elements);
        result->setData(records);
    };
    dbs[output] = result;
    return 0;
}


//void MCoordDBElementPoint::print()
//{
//    cout <<"<"<<LKS_coord.first<<","<<LKS_coord.second<<">: "<<data << " "<<text<< endl;
//}


//bool operator< (const MCoordDBElementPoint*& lhs, const MCoordDBElementPoint*& rhs){
//    //if (lhs == NULL || rhs == NULL) return false;
//    return lhs->data < rhs->data;
//}

bool MCoordDBElementPoint::larger  (MCoordDBElement * x, MCoordDBElement * y) { 
           //cout << "Comparing "<<x<<","<<y<<endl; 
           return  ((MCoordDBElementPoint *) x)->data > ((MCoordDBElementPoint *)y)->data;
 } // must satisfy (x,x) = FALSE


//TODO: result ostream&..
//void MCoordDBElementPoint::toTextStream(ostream& f){
//ostream& operator<<(ostream& os, const MCoordDBElementPoint& ic){
void MCoordDBElementPoint::print(ostream& os){
    os << "NAME=" << data;
    if (text != "") os << " "<< text;
}




double ComponentMarkerFilter::mf_area_old(MObject * obj){
    int a = ((MBlobObjectLocal*) obj)->n_coords;
    if ( (a >= 4) && (a <= 20) ) return 2.0;
    else return 1.0;
}


int ComponentMarkerFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MImage * img = outside->getImage(inputs2D[0]);
    float * data = img->data;
    long h = img->height;
    long w = img->width;
    MImage * resultImg = new MImage(w, h, img->scale);
    float * new_data = resultImg->data;
    if (append)
        for (int i = 0; i < w*h; i++)
            new_data[i] = data[i]; 
    else
        for (int i = 0; i < w*h; i++)
            new_data[i] = default_value;  //clear 

    MCoordDB * inputDB = outside->getDB(inputsDB[0]);
    int last_id = -1;
    double last_val = 0.0;
    

    for (int i = 0; i < w*h; i++) {
        int id = int(data[i]);
        //cout <<i << " " << id << " "<<data[i] <<endl;
        if (id < -0.5) continue; //empty
        if (id != last_id) {
            last_val = marker_function( (MObject*) inputDB->getElement(id));
            //cout << id << " " << last_val << endl;
        };
        new_data[i] = last_val;
    };

    results[output] = resultImg;

    return 0;
}

int AddMaxHeightFilter::run() {
    if ((-1 == MFilter::run())) return -1;

    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    float * original = outside->getImage(inputs2D[0])->data;
    float * averaged = outside->getImage(inputs2D[1])->data;

    int32_t w = outside->getImage(inputs2D[0])->width;
    
    for (unsigned int i = 0; i < blobsDB->size(); i++) {
        MBlobObjectLocal * blob = (MBlobObjectLocal *) blobsDB->getElement(i);
        double max_height = -99999.0;
        int32_t max_height_coord = -1;
        long sii = 0;
        long sjj = 0;
        int32_t * crds  = blob->coords;
        for (int k = 0; k < blob->n_coords; k++){
            float h = original[crds[k]]-averaged[crds[k]]; 
            if (h > max_height){
                //height
                max_height = h;
                max_height_coord = crds[k];
                };
             //mass center
             int ii = crds[k] / w;
             int jj = crds[k] % w;
             sii += ii;
             sjj += jj;
            };

        blob->max_height = max_height;
        blob->max_height_coord = max_height_coord; 

        blob -> mass_center_coord = ((int32_t) round(((double) sjj) / blob->n_coords))+
                                    (w * (int32_t) round(((double) sii) / blob->n_coords));


    };
    dbs[inputsDB[0]] = blobsDB; //in-place
    return 0;
}


int AddMaxAngleFilter::run() {
    if ((-1 == MFilter::run())) return -1;

    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    float * data = outside->getImage(inputs2D[0])->data;
    float sc = outside->getImage(inputs2D[0])->scale;

    int32_t w = outside->getImage(inputs2D[0])->width;
    int32_t h = outside->getImage(inputs2D[0])->height;
    
    for (unsigned int i = 0; i < blobsDB->size(); i++) {
        MBlobObjectLocal * blob = (MBlobObjectLocal *) blobsDB->getElement(i);
        double max_angle = -10;
        //long sii = 0;
        //long sjj = 0;
        int32_t * crds  = blob->coords;
        //float * diffsh = new float[blob->n_coords]; 
        //float * diffsv = new float[blob->n_coords];

        for (int k = 0; k < blob->n_coords; k++){
             //float h = original[crds[k]]-averaged[crds[k]]; 
             int ii = crds[k] / w;
             int jj = crds[k] % w;
             float diffh1=0, diffh2=0, diffv1 =0, diffv2 = 0;
             float c = data[crds[k]];
             if (jj != w)
                diffh2 = data[crds[k] + 1] - c;
             if (ii != h)
                 diffv2 = data[crds[k] + w] - c;
             if (jj != 0)
                 diffh1 = c - data[crds[k]-1];
             if (ii != 0)
                 diffv1 = c - data[crds[k]-w];

             float angle = acos ((sc + diffv2 * diffv1)/sqrt((sc*sc + diffv2 * diffv2) * (sc * sc + diffv1 * diffv1)));
             if (max_angle < angle) max_angle = angle;
             angle = acos ((sc + diffh2 * diffh1)/sqrt((sc*sc + diffh2 * diffh2) * (sc * sc + diffh1 * diffh1)));
             if (max_angle < angle) max_angle = angle;
            };

        blob->max_angle = max_angle;

    };
    dbs[inputsDB[0]] = blobsDB; //in-place
    return 0;
}


int AddCirclenessFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    float * data = outside->getImage(inputs2D[0])->data;
    int32_t w = outside->getImage(inputs2D[0])->width;
    int32_t h = outside->getImage(inputs2D[0])->height;
    for (unsigned int i = 0; i < blobsDB->size(); i++) {
       MBlobObjectLocal * blob = (MBlobObjectLocal *) blobsDB->getElement(i);

	//circleness.. may be slow. May be not a good measure...
       int P  = 0; //perimeter
	   int id = blob->id;
	   for (int j =0; j < blob->n_coords; j++) {
           int32_t c = blob->coords[j];
		   int ii = c / w;
		   int jj = c % w;	

           //cout << blob->i << " "<<blob->j << " "<< c << endl;

		   if ( (jj-1 < 0) ||  (id != (int) round(data[c-1])))
			    P+=1;
		   if ( (jj+1 >= w) || (id != (int) round(data[c+1])))
			    P+=1;
		   if ( (ii-1 < 0) || (id != (int) round(data[c-w])))
		   	    P+=1;
		   if ( (ii+1 >=h) || (id != (int) round(data[c+w])))
	    		P+=1;
	   };
	   //the ratio of area of a circle with this perimeter and the current blob
	   //r = P/ (2 PI);  S = P^2/(4 *PI)
	   //smaller blobs will be likely to have larger circleness
	   //non-convex shapes will have very small circleness
	   blob->circleness = blob->n_coords * 4 * PI / (P*P);
       //double r = 0.5 * min(blob->maxi-blob->mini, blob->maxj-blob->minj);
    };
    dbs[inputsDB[0]] = blobsDB; //in-place
    return 0;
}





//Convolves input with mask. Part of the mask can be made transparent
//by setting weights to 0
//f is the convolution function
//
//TODO: speed up by grouping masks of the same size
void AddCorrelationFilter::convolveBlobs(MCoordDB * blobsDB, MImage * inputImg, MMask * mask, unsigned char
       output_index) {
    float * data = inputImg->data;

    long h = inputImg->height;
    long w = inputImg->width;
    int w_mask = mask->w;
    int h_mask = mask->h;
    //double scale = inputImg->scale;
     
    //MImage * result = new MImage(w, h, scale);

    int sz = w_mask * h_mask;
    int half_w_mask = 1 + w_mask/2; //???
    int half_h_mask = 1 + h_mask/2;

    

    double mask_avg = 0, mask_sqr = 0, W = 0;
    for (int i = 0; i < w_mask * h_mask; i++) {
        mask_avg += mask->mask->data[i] * mask->weights->data[i];
        mask_sqr += sqr(mask->mask->data[i]) * mask->weights->data[i];
        W += mask->weights->data[i];
    };
    mask_avg = mask_avg/W;
    mask_sqr = mask_sqr/W;
    //double mask_sigma = mask_sqr - mask_avg * mask_avg;
    
    float rect[w_mask * h_mask];
    //int ict[w_mask * h_mask];
    //int jct[w_mask * h_mask];

    double nan = NAN;

    //for (int i = 0; i < h; i++)
	//for (int j = 0; j < w; j++)
    for (unsigned int k = 0; k < blobsDB->size(); k++)
      {

          MBlobObjectLocal * blob = (MBlobObjectLocal *) blobsDB->getElement(k);
          float max_cor1 = -99999.0, max_cor2 = -99999.0;
          int32_t max_cor_coord1 = -1, max_cor_coord2=-1;
          for (int l = 0; l < blob->n_coords; l++) {
            int32_t cl = blob->coords[l]; 
            int i = cl / w; 
            int j = cl % w;
   		    for (int k = 1-half_h_mask; k < half_h_mask; k++)
   			  for (int l = 1-half_w_mask; l < half_w_mask; l++){
                   int i_mask = half_w_mask - 1 + l + (half_h_mask + k - 1) * w_mask;
                {
                    //ict[i_mask] = i+k;
                    //jct[i_mask] = j+l;
   		 		    if ((i+k < 0) | (i+k >= h) | (j+l < 0) | (j+l >=w))
                      //continue; //if out of bounds: do not add..
                      rect[i_mask] = nan; //nan; //assumed 0 outside
                    else 
                      rect[i_mask] = data[w * (i+k) + j+l]; //smaller numbers
                 }
   			};

      
            double c1 = correlation_function1(sz, rect, mask->mask->data, mask->weights->data);
            
             
            /* if ( (blob->id == 3534) && (output_index==2)) {
                cout << blob->coord /w << " "<<blob->coord % w << endl;
                cout << "c1: "<<c1<< " " << (c1 >  max_cor1) << endl;

                print_matrix("ict", ict, h_mask, w_mask);
                print_matrix("jct", jct, h_mask, w_mask);

                print_matrix("rect", rect, h_mask, w_mask);
                mask->print(cout);
                exit(0);
            }; */

             
               if (c1 > max_cor1) {
                   max_cor1 = c1;
                   max_cor_coord1 = cl;
               };
          

          if (two_functions) {
               double c2 = correlation_function2(sz, rect, mask->mask->data, mask->weights->data);
               if (c2 > max_cor2) {
                   max_cor2 = c2;
                   max_cor_coord2 = cl;
               };
            };

          };
		 blob->max_template_correlation[output_index] = max_cor1;
         blob->max_template_correlation_coords[output_index] = max_cor_coord1;
         if (two_functions){
		   blob->max_template_correlation2[output_index] = max_cor2;
           blob->max_template_correlation_coords2[output_index] = max_cor_coord2;
         };


	  };

}

int AddCorrelationFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    MImage * inputImg = outside->getImage(inputs2D[0]);

    int s = masks.size();
    if (allocate_memory)
        for (unsigned int i = 0; i < blobsDB->size(); i++){ 
            MBlobObjectLocal * blob = (MBlobObjectLocal *) blobsDB->getElement(i);

            blob->n_templates = s;
            blob->max_template_correlation = new float[s];
            blob->max_template_correlation_coords = new int32_t[s];
           
            if (two_functions) {
               blob->n_templates2 = s;
               blob->max_template_correlation2 = new float[s];
               blob->max_template_correlation_coords2 = new int32_t[s];
            };


        };
    cout<< "convolving"<<endl;
    for (unsigned int i = 0; i < masks.size(); i++) {
        cout << masks[i]->id << endl;
        //masks[i]->print(cout);
        convolveBlobs(blobsDB, inputImg, masks[i] , i);
    };

    dbs[inputsDB[0]] = blobsDB; //in-place
    return 0;
};


SetBlobCenterFilter::SetBlobCenterFilter(string blobsImg, string blobsDB,string centerType)
        {

            inputs2D.push_back(blobsImg);
            inputsDB.push_back(blobsDB);
            if (centerType == "max_height")
                center_type = 0;
            else if (centerType == "mass_center")
                center_type = 2;
            else if (centerType == "ellipse_center")
                center_type = 1;
            else {
                cout << "Unknown center type "<<centerType << endl;
                throw("Unknown center type");
            };
            if (DEBUGGING) cout << "center type: "<< center_type << endl;
        };


int SetBlobCenterFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    MImage * blobsImg = outside->getImage(inputs2D[0]);
    int w = blobsImg->width;
    int h = blobsImg->height;
    if (blobsDB->size() == 0) {
        dbs[inputsDB[0]] = blobsDB;
        return 0;
    };

    //we do not assume that all blobs come from the same sheet
    for (unsigned int i = 0; i < blobsDB->size(); i++){ 
            MBlobObjectLocal * blob = (MBlobObjectLocal *) blobsDB->getElement(i);

            //cout << blob->LKS_x << " " << blob->LKS_y << endl;

            int_coord sheet = get_sheet_coords(blob->LKS_x, blob->LKS_y);
            //cout << sheet.first << " " << sheet.second << endl; 
            int32_t center = 0;
            double ii, jj;
            
            //center is mass center
            if (center_type == 2 && blob->mass_center_coord) {
                center = blob->mass_center_coord;
                ii = center / w;
                jj = center % w; 
            };

            //center is ellipse center
            if (center_type == 1 && blob->ref_el >=0 && blob->ref_el < blob->n_ellipses) { 
                ii = blob->ellipse_center_v[blob->ref_el];
                jj = blob->ellipse_center_h[blob->ref_el];

                int ri = int(round(ii)); //can only encode a coord  inside sheet..
                int rj = int(round(jj));

                if ( (ri >=0) && (ri <= h-1) && (rj >= 0) && (rj <= w-1))
                    center = ri * w + rj; //otherwise default to height
            };
 
            //center is max height (default)
            if (center_type == 0 || not center) {
                center = blob->max_height_coord;
                ii = center / w;
                jj = center % w;
            };

            //cout << i << " " << j << endl; 
            blob->coord = center;
            double_coord c_new = get_LKS_coords(sheet, ii, jj); //TODO: get rid of hardcoded LKS!!!
            //cout << center_type << " "<<blob->ref_el << " "<<blob->n_ellipses << endl;
            //cout << fixed << ii << " "<<jj <<" " << c_new.first << " "<<c_new.second << endl;

            //cout << c_new.first << " " << c_new.second << endl; 
            
            //exit(1);

            blob->LKS_x = c_new.first;
            blob->LKS_y = c_new.second; 
    };
    dbs[inputsDB[0]] = blobsDB;
    return 0;

}

int SetVariancesFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    int S = inputs2D.size();
    float* datas[S]; 
    for (int k = 0; k < S; k++)
        datas[k] = outside->getImage(inputs2D[k])->data;

    //we do not assume that all blobs come from the same sheet
    for (unsigned int i = 0; i < blobsDB->size(); i++){ 
            MBlobObjectLocal * blob = (MBlobObjectLocal *) blobsDB->getElement(i);
            if (blob->n_variances != 0)
                delete blob->variances;
            blob->n_variances = S;
            blob->variances = new float[S];
            for (int k = 0; k < S; k++)
                blob->variances[k] = datas[k][blob->coord];
           };

    dbs[inputsDB[0]] = blobsDB;
    return 0;
}


int SelectorFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * inputDB = outside->getDB(inputsDB[0]);
    MCoordDB * outputDB = new MCoordDB();

    if (inputsDB[0] == output_name) copy = true;

    for (unsigned int i = 0; i < inputDB->size(); i++){
        MCoordDBElement * el = inputDB->getElement(i);
        if (select(el)) outputDB->append(copy?el->getCopy():el);
    };
    cout << outputDB->size() << " out of "<<inputDB->size() << " elements selected."<<endl;
    
    /* cannot do this until I find a better way to represent neighbours
    if (inputsDB[0] == output_name) {
        cout << "Deleting "<<inputsDB[0]<<endl;
        delete inputDB; // TODO: find a generic way to do this?
    };
    */
    dbs[output_name] = outputDB;
    return 0;
}

int ImageDestructorFilter::run() {
    if ((-1 == MFilter::run())) return -1; //do not know input types in advance

    /*
    if (  (!outside->hasImage(to_delete)) && (!outside->hasDB(to_delete))) {
        cout << "DestructorFilter warning: "<<to_delete << " is not in the results.";
        return -1;
    }; */

    if (outside->hasImage(to_delete)){
        MImage * img = outside->getImage(to_delete);
        if (img != NULL) delete img;
        results[to_delete] = NULL;
    }
    else cout << "ImageDestructorFilter warning: the image does not exist: "<<to_delete<<endl;
    /*
    if (outside->hasDB(to_delete)){
        MCoordDB * db = outside->getDB(to_delete);
        if (db != NULL) delete db;
        dbs[to_delete] = NULL;
    };
    */
    return 0;
}


int FindNeighboursFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    MImage * blobsImg = outside->getImage(inputs2D[0]);
    int w = blobsImg->width;
    int h = blobsImg->height;
    float * data = blobsImg->data;

    float r = radius / blobsImg->scale;

    //we do not assume that all blobs come from the same sheet
    for (unsigned int k = 0; k < blobsDB->size(); k++){ 
        MBlobObjectLocal * blob = (MBlobObjectLocal*) blobsDB->getElement(k);
        int ci = blob->coord / w;
        int cj = blob->coord % w;
        set<int32_t> nbrs;
        for (int i = max(0, ci - radius); i < min(h, ci + radius); i++)
            for (int j = max(0, cj - radius); j < min(w, cj+radius); j++)
                if ( sqr (i-ci) + sqr(j-cj) <= sqr(r))
                    nbrs.insert((int32_t)data[w*i+j]);
        nbrs.erase(blob->id);
        nbrs.erase(-1); //!!!
        blob->n_neighbours = nbrs.size();
        blob->neighbours = new MObject*[nbrs.size()];

        int n=0;
        for (set<int32_t>::iterator a = nbrs.begin(); a!=nbrs.end(); ++a) {
            MObject * x = (MObject*) blobsDB->getElement(*a);
            if (x == NULL) cout << "NULL!!: "<< *a << endl;
            /* if (blob->id == 1) {
                cout << "Neighbour: "<< *a << endl;
                cout << x->id << endl;
            }; */
            blob->neighbours[n++] = x;
        };
        //if (blob->id == 1) exit(0);
    };

    dbs[inputsDB[0]] = blobsDB;
    return 0;

}


int CustomDBModifyFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * DB = outside->getDB(inputsDB[0]);
    f(DB); //that's it.
    dbs[inputsDB[0]] = DB;
    return 0;
}

EllipseFilter::EllipseFilter(string dbName, string inputImg){
           inputsDB.push_back(dbName); 
           inputs2D.push_back(inputImg); 
           max_radius = double_setting("max_ellipse_radius", 20); 
           for (int i = 1; i < int_setting("max_n_ellipses", 6); i++)
              steps.push_back(double_setting("ellipse_step", 0.15) * i);
 };


int EllipseFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    MImage * inputImg = outside->getImage(inputs2D[0]);
    int w = inputImg->width;
    int h = inputImg->height;
    float * data = inputImg->data;
    unsigned int progress_step = int( double_setting("print_progress_step", 0.1) * blobsDB->size());
    unsigned int next_progress = progress_step;
    float scalef = inputImg->scale;
    int mr = (int)round(max_radius/scalef);

    //for calculating ref_el
    double min_ellipse_length = double_setting("good_ellipse_min_length", 1.4);
    double max_ellipse_mse = double_setting("good_ellipse_max_mse", 2);



  for (unsigned int k = 0; k < blobsDB->size(); k++){
      if (k > next_progress) {
          cout << "[progress: " << (100 * k) / blobsDB->size()<< "%]"<<endl;
          next_progress += progress_step;
      };
	  if (DEBUGGING)
	 	cout << "blob " << k << "/" << blobsDB->size() << endl;
	    //if ((k == 11439)){ //11439
        //		  cout << "stop"<< endl;
	    //};
        MBlobObjectLocal * blob = (MBlobObjectLocal*) blobsDB->getElement(k);
        int ci = blob->coord / w;
        int cj = blob->coord % w;


        int mini = max(0, ci - mr);
        int maxi = min(h-1, ci + mr);
        int minj = max(0, cj - mr);
        int maxj = min(w-1, cj + mr);

        int r_width = maxj - minj + 1;
        int r_height = maxi - mini +1;

        float elev[r_width * r_height];
        for (int i = 0; i < r_height; i++)
            for (int j = 0; j < r_width; j++)
                elev[j + r_width * i] = data[ minj + j + (mini+i) * w];

        vector<triangle> trng  = triangulation(elev, r_width,r_height);

        int rel_ci = ci - mini;
        int rel_cj = cj - minj;

        MEllipse * ellipses[steps.size()];
        Curve3d * the_contours[steps.size()];
        int last_non_null = -1;
        for (unsigned int i = 0; i < steps.size(); i++)
            ellipses[i] = NULL;

        for (unsigned int i = 0; i < steps.size(); i++) {
            if (steps[i] > blob->max_height) break;
            vector<Curve3d*> contours = contour(trng, blob->max_height - steps[i]);
    
            int occurrences = 0;
            for (unsigned int j = 0; j < contours.size(); j++)
                if (contours[j]->isCycle && contours[j]->contains(rel_ci, rel_cj)) {
                    occurrences++;
                    the_contours[i] = contours[j]; //select the contour containing the center
                };

            if (occurrences == 1) {                //do not want double ellipses
                ellipses[i] = MEllipse::fitSecondOrderCurve(the_contours[i]);
                if (ellipses[i] != NULL) last_non_null = i;
            };
          
            for (unsigned int j = 0; j < contours.size(); j++) 
                if (contours[j] != the_contours[i]) delete contours[j];

        };

        blob->n_ellipses = last_non_null+1;
        blob->ref_el = -1;
        if (last_non_null < 0) continue;
        blob->ellipse_a = new float[last_non_null+1];
        blob->ellipse_b = new float[last_non_null+1];
        blob->ellipse_phi = new float[last_non_null+1];
        blob->ellipse_mse = new float[last_non_null+1];
        blob->ellipse_center_h = new float[last_non_null+1];
        blob->ellipse_center_v = new float[last_non_null+1];

        for (int i = 0; i <= last_non_null; i++){
            MEllipse * ellip = ellipses[i];

            if (ellip != NULL){
                    blob->ellipse_a[i] = ellip->a * scalef;
                    blob->ellipse_b[i] = ellip->b * scalef;
                    blob->ellipse_phi[i] = ellip->phi;
                    //blob->ellipse_center[i] = ( (int) round (mini + ellip->x0) ) * w + (int) round(minj + ellip->y0);
                    blob->ellipse_center_h[i] = minj + ellip->y0;
                    blob->ellipse_center_v[i] = mini + ellip->x0;
                    /*
                    if (k==930 && i== 0) {
                        cout <<"DEBUGELLIPSE: " <<ellip->x0 << " "<< ellip->y0<< " "<< ellip->a << " "<< ellip->b << " "<<ellip->phi << " "<< endl;
                        for (int j = 0; j < the_contours[i]->vertices.size(); j++)
                            cout << the_contours[i]->vertices[j] << endl;

                    };*/ 

                    blob->ellipse_mse[i] = getMSE(the_contours[i], ellip->A) * (scalef * scalef);
                    delete ellip;
            }
            else {
                    blob->ellipse_a[i] = NAN;
                    blob->ellipse_b[i] = NAN;
                    blob->ellipse_phi[i] = NAN;
                    //blob->ellipse_center[i] = 0;
                    blob->ellipse_center_h[i] = NAN;
                    blob->ellipse_center_v[i] = NAN;
                    blob->ellipse_mse[i] = NAN; 
            };
            if (the_contours[i] != NULL) delete the_contours[i];
        };
        
          int ref_el = -1; //reference ellipse index
          for (int i = 0; i < blob->n_ellipses; i++) {
              float mse = blob->ellipse_mse[i]; 
              float  length = sqrt( sqr(blob->ellipse_a[i]) + sqr(blob->ellipse_b[i]));
              if ( (length >= min_ellipse_length ) && (mse != NAN) && (mse < max_ellipse_mse)) ref_el = i;
          };
          blob->ref_el = ref_el;

    };
    
    cout << "[progress: 100%]"<<endl;
    dbs[inputsDB[0]] = blobsDB;
    return 0;

}




int EllipseProfileFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    MImage * inputImg = outside->getImage(inputs2D[0]);
    int w = inputImg->width;
    int h = inputImg->height;
    float * data = inputImg->data;
    
    float step_size = step_size_meters / inputImg->scale;
    int max_radius = (int) round(max_radius_meters/inputImg->scale);


    for (unsigned int k = 0; k < blobsDB->size(); k++){ 
          MBlobObjectLocal * blob = (MBlobObjectLocal*) blobsDB->getElement(k);

          int ref_el = blob->ref_el;
          if (ref_el >= blob->n_ellipses || ref_el == -1) continue;
          //cout << k << " "<<ref_el << " "<<blob->n_ellipses << endl;
          //cout << *blob << endl;

          int ci = blob->coord / w;
          int cj = blob->coord % w;

          int mini = max(0, ci - max_radius);
          int maxi = min(h-1, ci + max_radius);
          int minj = max(0, cj - max_radius);
          int maxj = min(w-1, cj + max_radius);

          int r_width = maxj - minj + 1;
          int r_height = maxi - mini +1;

          if ( (r_width < 0) || (r_height < 0)) {
              cout << "Error: wrong coord in blob #" <<k << endl;
              continue;
          };

          //cout << blob->ellipse_center_h[ref_el] << endl;
          //cout << blob->ellipse_center_h[ref_el] << endl;
          //cout << blob->coord << " "<<ci << " "<<cj << " "<<minj << " " << maxj << " " << max_radius << endl;
          //cout << "rw rh " << r_width << " "<<r_height << endl;

          float elev[r_width * r_height];
          for (int i = 0; i < r_height; i++) //is this necessary?
              for (int j = 0; j < r_width; j++) {
                  elev[j + r_width * i] = data[ minj + j + (mini+i) * w];
              };

          //vector<triangle> trng  = triangulation(elev, r_width,r_height); //unfortunate framework shortage: see TODO in MFilter.h

          //int rel_ci = ci - mini;
          //int rel_cj = cj - minj;
         
          //ellipse center coordinates
          float c_hor = blob->ellipse_center_h[ref_el] - minj;
          float c_vert = blob->ellipse_center_v[ref_el] - mini; //corresponds to X coordinate in the ellipse and i coordinate in the matrix

          //ellipse_profile(elev, r_width, r_height, c_hor, c_vert, blob->ellipse_a[ref_el], blob->ellipse_b[ref_el], blob->ellipse_phi[ref_el]);

          float a = blob->ellipse_a[ref_el];
          float b = blob->ellipse_b[ref_el];
          float phi = blob->ellipse_phi[ref_el];
          vector<float> profile, profile_var;
          float h = sqr((a-b)/(a+b));
          float perimeter = PI * (a+b) * (1 + 3 * h / (10 + sqrt(4 - 3 * h))); //wikipedia 

          for (float r = 0; true; r+=step_size) {
              float S = 0;
              double S2 = 0;
              int n = 0;
              bool out_of_bounds = false;

              float a_cos_phi = r * (a/b) * cos(phi);
              float b_sin_phi = r * sin(phi);
              float angular_step_size = 2*PI / ((r/b) * perimeter / step_size);

              for (float angle = 0; angle < 2 * PI; angle += angular_step_size) {
                  float x = c_vert + a_cos_phi * cos (angle) - b_sin_phi * sin(angle);
                  float y = c_hor + a_cos_phi * cos(angle) + b_sin_phi * sin(angle);
                  int ii = (int) x;
                  int jj = (int) y;
                  if ( ii < 0 || ii >= r_height-1 || jj < 0 || jj >= r_width -1)
                  {
                      out_of_bounds = true;
                      break;
                  };
                  
                  // (jj+1, ii)
                  int opp = ((x - ii) + (y -jj) > 0.5); //should we take the bottom left or top right triangle 

                  float z;
                  //interpolate the value from the triangle (ii, jj+1) (ii+1, jj), (ii+opp, jj+opp)
                  //faster version of barycentric_coords (MGeom.h)
                  if (opp == 0) {
                      float x3 = ii+1;
                      float y3 = jj;
                      float lambda1 = y - y3;
                      float lambda2 = x3 - x + y3- y;
                      z =  lambda1 * elev[jj+1 + ii * r_width] + lambda2 * elev[jj + ii * r_width] + (1-lambda1-lambda2) * elev[jj + (ii+1) * r_width];
                  }
                  else {
                      float x3 = ii+1;
                      float y3 = jj+1;
                      float lambda1 = x3-x;
                      float lambda2 = y3-y;
                      z = lambda1 * elev[jj+1 + ii * r_width] + lambda2 * elev[jj + (ii+1) *r_width] + (1-lambda1-lambda2) * elev[jj + 1 + (ii+1) * r_width];
                  };

                  S+=z;
                  S2+=z*z;
                  n+=1;
              }
              if (out_of_bounds) break;
              profile.push_back(S/n);
              profile_var.push_back((float) (S2/n - (S/n) * (S/n)));
          };//creation of the profile
          blob->n_ellipse_profile = profile.size();
          blob->ellipse_profile = new float[profile.size()];
          blob->ellipse_profile_var = new float[profile.size()];
          copy(profile.begin(), profile.end(), blob->ellipse_profile);
          copy(profile_var.begin(), profile_var.end(), blob->ellipse_profile_var);


      };//loop through blobs

    dbs[inputsDB[0]] = blobsDB;
    return 0;
}

//assumes already within bounds;
//inline float interpolate_shift(float * data, int width, int ci, int cj, float dx, float dy) {
//x corresponds to the vertical coord, y to the horizontal
inline float interpolate_height(float * data, int width, float x, float y) {
    //float x = ci + dx;
    //float y = cj + dy;
    int ii = (int) x;
    int jj = (int) y;
    int opp = ((x - ii) + (y - jj) > 0.5); //should we take the bottom left or top right triangle 

    float z;
    //interpolate the value from the triangle (ii, jj+1) (ii+1, jj), (ii+opp, jj+opp)
    //faster version of barycentric_coords (MGeom.h)
    if (opp == 0) {
      float x3 = ii+1;
      float y3 = jj;
      float lambda1 = y - y3;
      float lambda2 = x3 - x + y3- y;
      z =  lambda1 * data[jj+1 + ii * width] + lambda2 * data[jj + ii * width] + (1-lambda1-lambda2) * data[jj + (ii+1) * width];
    }
    else {
      float x3 = ii+1;
      float y3 = jj+1;
      float lambda1 = x3-x;
      float lambda2 = y3-y;
      z = lambda1 * data[jj+1 + ii * width] + lambda2 * data[jj + (ii+1) *width] + (1-lambda1-lambda2) * data[jj + 1 + (ii+1) * width];
    };
    return z;
}

int EllipseNormalizeFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    MImage * inputImg = outside->getImage(inputs2D[0]);
    int w = inputImg->width;
    int h = inputImg->height;
    float * data = inputImg->data;
    
    float step_size_px = step_size / inputImg->scale;
    float default_length_px = default_length/inputImg->scale;
    float nan_value = double_setting("replace_nan", NAN);
    bool squeeze = bool_setting("squeeze", true);

    for (unsigned int k = 0; k < blobsDB->size(); k++){ 
          MBlobObjectLocal * blob = (MBlobObjectLocal*) blobsDB->getElement(k);
          int ref_el = blob->ref_el;
 
          if (ref_el>= blob->n_ellipses || ref_el == -1) continue; //or save ref_el into blob

        
          float a = blob->ellipse_a[ref_el] / inputImg->scale;
          float b = blob->ellipse_b[ref_el] / inputImg->scale;
          float phi = blob->ellipse_phi[ref_el];
        
          //int ci = blob->coord / w;
          //int cj = blob->coord % w;

          float cx = blob->ellipse_center_v[ref_el]; //ellipse center point
          float cy = blob->ellipse_center_h[ref_el];



          blob->furrow_distance = NAN;

          int phase = 0;

          for (int i = 1; i < blob->n_ellipse_profile; i++) //2 could be a parameter
          {
              float t = atan((blob->ellipse_profile[i] - blob->ellipse_profile[i-1])/profile_step_size); //step size should be the same as in the profile filter!
              if (phase == 0) {
                  if (t > furrow_threshold) continue; //skip until starts going down (small pit inside) 
                  else phase = 1;
              };
              if (t > furrow_threshold) {
                  blob->furrow_distance = (i-1) * profile_step_size; 
                  break;
              }
          }
          float length;

          //length in pixels (untransformed)
          if ((blob->furrow_distance != NAN) && (2 * blob->furrow_distance > default_length))
             length = blob->furrow_distance / inputImg->scale;
          else length = default_length_px;

          //int real_rescaled_width = 2 * (int) round(length / step_size)+2; //was +1
          int rescaled_width;
          float real_step_size;
          
          if (fully_normalize) { //constant size -> normalizing to circle with radius default_length
            rescaled_width = 2 * (int) round(default_length / step_size) + 2 * steps_from_ditch; //was +1
            real_step_size = step_size_px * (1.0 * length) / default_length_px;
          }
          else { //variable size -> normalizing to circle with radius b
            rescaled_width = 2 * (int) round(length / step_size_px)+ 2 * steps_from_ditch; //was +1
            real_step_size = step_size_px;
          };
          
          float real_radius = (rescaled_width * 0.5) * real_step_size;
          float * rescaled = new float[sqr(rescaled_width)]; //was +1
  
         
          for (int ii = 0; ii < rescaled_width; ii++) { //was <= 

              for (int jj = 0; jj < rescaled_width; jj++){ //was <=
                  float x = (ii - (rescaled_width-1)/2) * real_step_size; //was rescaled_width/2
                  float y = (jj - (rescaled_width-1)/2) * real_step_size;
                  if ( (x * x + y * y) > sqr(real_radius)) {
                      rescaled[jj + ii * rescaled_width] = nan_value; //outside the bounding circle
                      continue;
                  };
                  vector2d dx;
                  if (squeeze)
                      dx = vector2d(x,y).rotate(-phi).scale(a/b, 1).rotate(phi); //rotate and stretch according to ellipse
                  else dx = vector2d(x,y); //just center and resize according to a
                  int iii = int(cx +  dx.x); 
                  int jjj = int(cy +  dx.y);
                  //cout << "("<<iii<<","<<jjj<<")"<<" ";
                  if (iii < 0 || iii >= h - 1 || jjj < 0 || jjj >= w - 1)
                      continue; //out of bounds
                  rescaled[jj + ii * rescaled_width] = interpolate_height(data, w, cx+dx.x, cy+dx.y); 
              };
              //cout << endl;
          };
          blob -> rescaled = rescaled;
          blob -> rescaled_width = rescaled_width;
          //print_matrix("rescaled", rescaled, rescaled_width, rescaled_width, cout);
          //exit(1);
     };//loop through blobs
    

    dbs[inputsDB[0]] = blobsDB;
    return 0;
}


int AddRawHeightsFilter::run() {
    if ((-1 == MFilter::run())) return -1;
    MCoordDB * blobsDB = outside->getDB(inputsDB[0]);
    MImage * heightsImg = outside->getImage(inputs2D[0]);
    int w = heightsImg->width;
    int h = heightsImg->height;
    
    float * data = heightsImg->data;

    for (unsigned int i = 0; i < blobsDB->size(); i++){ 
            MBlobObjectLocal * blob = (MBlobObjectLocal *) blobsDB->getElement(i);


            int32_t center = blob->coord;
            int ci = center / w;
            int cj = center % w;
            blob->raw_cols = 2 * radius + 1;
            blob->raw_rows = 2 * radius + 1;
            blob->raw_heights = new float[sqr(2 * radius + 1)];

            int ww = 2 * radius + 1;

            for (int k = -radius; k <= radius; k++)
                for (int l = -radius; l<=radius; l++) {
                    int ii = ci+k;
                    int jj = cj+l;
                    float x;
                    if ( (ii >= 0) && (ii < h) && (jj >=0) && (jj<w)) x = data[jj + w * ii];
                    else x = NAN;
                    blob->raw_heights[radius+l+(radius+k)*ww] = x;
                };

    };
    dbs[inputsDB[0]] = blobsDB;
    return 0;

}
