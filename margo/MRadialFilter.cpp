#include "MRadialFilter.h"

using namespace std;

//TODO: static members of MRadialFilter
double average(int r, int n, float * input){
    float S = 0;
    for (int i = 0; i<n; i++)
        S += input[i];
    return S/n;
};

double variance(int r, int n, float * input){
    double S = 0;

    double a = average(r, n, input);
    for (int i = 0; i<n; i++){
        double x = (input[i] - a);
        S+= x * x;
    };
    return (S/n);
    //double v = (S/n) - a * a;
    //if (v <= -0.5){
    //    cout << "Problem: variance is "<<v<<" S="<<S<<" a="<<a <<endl;
    //    print_vector("input", 0, n-1, input);
    //    exit (1);
    //}
    //return v;
};


MRadialFilter::MRadialFilter(int radius, string input, string condimage, string output, 
                arrayfunction processingfn, conditionfunction condfn, 
                void * condfnpars) //:
           // output(output), radius(radius), condfn(condfn), //for some reason crashed when was in .h
           // condfnpars(condfnpars)
             {
                 this->radius = radius;
                 this->output = output;
                 this->condfn = condfn;
                 this->condfnpars = condfnpars;
                 this -> condimage = condimage;
                 inputs2D.push_back(input);
                 if (input != condimage)
                    inputs2D.push_back(condimage);
              procfns.push_back(processingfn);
             };



vector<unsigned long> thresholdFunction(MImage* image, void * param){
    vector<unsigned long> result;
    double threshold = 0.0; // it would maybe make sense to pick, say, top 5% (though maybe not - pilkapiai are not uniformly distributed...)
    if (param != NULL) 
        threshold = *((double *) param);
    for (int i = 0; i < image->width*image->height; i++){
        if (image->data[i] > threshold)
            result.push_back(i);
    }
    return result;
};

//generates a discretized circle with radius r
void gen_discrete_circle(int r, int& length, int*& xs, int*& ys){
    double step = 0.1;
    int n = (int) (r * PI / 2 / step);
    step = (r * PI / 2) / n;
    int xcoords[4*n], ycoords[4*n];
    for (int i = 0; i < 4*n; i++){
        xcoords[i] = (int) (round(r * cos(i * step)));
        ycoords[i] = (int) (round(r * sin(i * step)));
    };    
    int minrep = 1; //minimum number of times for a coordinate to repeat to be added
    int leave[4*n];
    int j = 0;
    int prevx = 0;
    int prevy = 0;
    int rep = 0;
    for (int i =0; i < 4*n; i++){
        if ((xcoords[i] == prevx) and (ycoords[i] == prevy))
            rep += 1;
        else {
            rep = 1;
            prevx = xcoords[i];
            prevy = ycoords[i];
        };

        if (rep == minrep){ //add
                leave[j] = i;
                j+=1;
        };
    };
    xs = new int[j];
    ys = new int[j];
    for (int i = 0; i < j; i++){
        xs[i] = xcoords[leave[i]];
        ys[i] = ycoords[leave[i]];
    };
    length = j;
};



int MRadialFilter::run(){
    //cout << "Running MRadialFilter..."<<endl;
    if ((-1 == MFilter::run())) return -1;
    MImage * img = outside->getImage(inputs2D[0]);
    float * data = img->data;
    long h = img->height;
    long w = img->width;
    //double scale = img->scale;
    MCoordDB * result = new MCoordDB();
  
    if (DEBUGGING) cout << "Selecting values to run condfn on.."<<endl;
    vector<unsigned long> todo = condfn(outside->getImage(condimage), condfnpars);
    if (DEBUGGING) cout <<todo.size()<<" coordinate(s) selected"<<endl;

    int * npoints = new int[radius+1]; //number of points in each layer
    //int ** xs = new (int*)[radius]; //xs[l] will contain a list of x-coordinates going around the circle of layer l
    //int ** ys = new (int*)[radius]; //same for ys;
    
    int ** offsets = new int*[radius+1]; //determines shapes for each level

    for (int l = 1; l <= radius; l++){ //create the static shape in advance
        int n, *x, *y;
        gen_discrete_circle(l, n, x,y);
        npoints[l] = n;
        offsets[l] = new int[n];
        for (int i = 0; i < n; i++)
            offsets[l][i] = w * y[i] + x[i]; //easy for non-boundary..
        delete x;
        delete y;
    };
    if (DEBUGGING) cout << "discrete circles generated" << endl;
    
    //cout << "profile prepared"<<endl;
    
    if (DEBUGGING) print_vector("offsets[1]", 0, npoints[1], offsets[1]);

    for (unsigned int i = 1; i < todo.size(); i++) {
        unsigned long point = todo[i];
        //filter out the boundary points..
        int x = point % w;
        int y = point / w;
    
        float ** profile  = new float*[procfns.size()];     //prepare a shape for
        for (unsigned int i = 0; i < procfns.size(); i++)            //temporary result --- ACTUALLY NOT TEMPORARY
                  profile[i] = new float[radius];

        if ((y < radius) || (x < radius) || (x > w - radius) || (y > h - radius))
            continue;
        
            for (int l = 1; l <= radius; l++){
                float shape[npoints[l]];
                for (int k = 0; k < npoints[l]; k++)
                    shape[k] = data[point + offsets[l][k]];

          //      if ((x==2387) && (y == 2050) && (l == 4)){
       //             cout << "level "<<l<<endl;
       //             print_vector("offsets:", 0, npoints[l], offsets[l]);
          //          print_vector("shape:", 0, npoints[l], shape);
          //          cout <<"avg: "<< procfns[0](radius, npoints[4], shape) << endl;
          //          cout <<"var: "<< procfns[1](radius, npoints[4], shape) << endl;
          //      };

                for (unsigned int ii = 0; ii < procfns.size(); ii++)
                    profile[ii][l-1] = (float) procfns[ii](radius, npoints[l], shape); 
            };
        //if ((x==2387) && (y == 2050)){
        //    //print_vector("shape:", 0, npoints[4], shape);
        //           print_vector("profile[1]", 0, radius, profile[1]);
        //    exit (0);
       //};
        //result->getX(); //dummy!
        result->append(new MCoordDBElementMatrix(x,y,procfns.size(), radius, profile));
    };
    
    //for (int i = 0; i < procfns.size(); i++)
    //    delete profile[i];
    //delete profile;

    for (int i = 1; i <= radius; i++)
        delete offsets[i];
    delete offsets;
    dbs[output] = result;
    if (DEBUGGING) cout << "MRadialFilter completed"<<endl;
    return 0;
}

string MRadialFilter::getName(){ return "MRadialFilter";}

void MRadialFilter::addProcessingFunction(arrayfunction fn){
  procfns.push_back(fn);
};

/*
void MCoordDBElementMatrix::print(){
    cout <<"<"<<LKS_x<<","<<LKS_y<<">"<<endl;
    for (int i = 0; i < nfunctions; i++){
        for (int j = 0; j < nlevels; j++)
            cout << data[i][j] << " ";
        cout << endl;
    };
}
*/

float MCoordDBElementMatrix::avg(int index){
        float S = 0;
        for (int j = 0; j < nlevels; j++)
            S+=data[index][j];
        return S / nlevels;
    };

//void MCoordDBElementMatrix::toTextStream(ostream& f){
//ostream& operator<<(ostream& os, const MCoordDBElementMatrix& ic){
void MCoordDBElementMatrix::print(ostream& os){
  os << nfunctions << " "<<nlevels<< endl;
    for (int i = 0; i < nfunctions; i++){
        for (int j = 0; j < nlevels; j++)
                os << data[i][j] << " ";
        os<<endl;
    };
}


//bool operator< (const MCoordDBElementMatrix*& lhs, const MCoordDBElementMatrix*& rhs) {
//    return lhs->avg(0) < rhs->avg(0); //stupid rule..
//}
