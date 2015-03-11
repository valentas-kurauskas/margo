#ifndef __MFILTER_H
#define __MFILTER_H
#include <vector>
#include <map>
#include "MImage.h"
#include <cmath>
#include <iostream>
#include "utils.h"
#include <stdlib.h>
#include "MCoordDB.h"
#include "MProcessor.h"
#include <queue>
#include <algorithm>
#include "MObject.h"
#include <set>
#include "MGeom.h"
#include "MConstraints.h"

//TODO (big): filters that run on blob DBs should be separated and blobs treated as particles.
//This would save doing things like triangulation twice.

class MProcessor;
class MFilterData;

class MFilter{
    protected:
	//std::map<std::string, MImage*> images;
        //std::map<std::string, MImage*> results;
        std::vector<std::string> inputs2D;
        std::vector<std::string> inputsDB;
        MFilterData * outside; //data outside this filter from which the input is taken and result returned
    public:
        //MFilter();

        virtual std::vector<std::string> getInputsDB();
        virtual std::vector<std::string> getInputs2D();
        virtual std::vector<std::string> getInputs();
        //void connect(std::map<std::string, MImage*> partial);
        virtual void connect(MFilterData * outside);
        virtual int run(); //runs the filter
        virtual void upload(); //returns results to the outside
        //virtual void setResult(const MProcessor * proc);
        virtual void printname();            //only for debugging
        virtual std::string getName();
        virtual ~MFilter(){};
};

//A filter that maps MImages -> MImages
class MFilter2D: virtual public MFilter{
    protected:
        std::map<std::string, MImage*> results;
        std::map<std::string, MImage*> lastResult();
    public:
        std::string getName();
        void upload();
        //void setResult(const MProcessor * proc);
};

//A filter that maps MImages -> CoordDB structures
class MFilterCoordDB: virtual public MFilter{
    protected:
        std::map<std::string, MCoordDB*> dbs;
        std::map<std::string, MCoordDB*> lastDBS(); //do not need?
    public:
         std::string getName();
         void upload();
        //void setResult(const MProcessor * proc);
};

// much needs to be redefined..


class MFilterMixed: public MFilter2D, public MFilterCoordDB {
	public:
		std::string getName() {return "MFilterMixed";};
		void upload();
};


class GaussianFilter: public MFilter2D{
    double radius; 
    double height;
    std::string output;
    int template_size;
    double * get_values(int k, double sigma);
    int get_discrete_radius(double sigma);

    public:
        //Gaussian filter
        GaussianFilter(double radius, std::string input, std::string output, double height=0.0, int template_size = 0): radius(radius), height(height), output(output), template_size(template_size)
         { inputs2D.push_back(input);};



        int run();
        int get_template_size(double scale = 2.0);
        float * get_template(double scale = 2.0);
        //void printname();
        std::string getName(); 
};

//more general than GaussianFilter
class SeparableFilter: public MFilter2D{
    std::string output;
    int sizeX; //in image units
    int sizeY; //in image units
    float * templateX;
    float * templateY;
    bool normalize;


    public:
        //Gaussian filter
        SeparableFilter(std::string input, std::string output, int sizeX, int sizeY, float * templateX, float * templateY, bool normalize);
        //not useful - it gives a cross
    
        static float * step_template(int step_radius, int total_radius);

        static float * constant_template(int size);

        int run();
        std::string getName() {return "SeparableFilter";};
       ~SeparableFilter(); 
};



//Mask filter
//for each point return the "generalized" dot product
//of the square centered at that point and
//the mask


class MaskFilter: public MFilter2D{
	int size; 
	float * mask; //(2 * size - 1) x (2 * size - 1)
    float * weights;
    //bool normalize;
	std::string output;
    double (*f)(int, float*, float*, float*);
	public:
    //size:   integer radius of the mask
    //mask:   an array of length (2*size-1) x (2 * size-1) (interpret as a rectangle). copied.
    //f:      a function (double,double)->double 
    //weights: an array of the same dimensions as mask, only values marked
    //        by true will be summed in the generalized dot product. Value NULL corresponds to an all-1 matrix. copied.
	  MaskFilter(int size, float * mask, std::string input,
			  std::string output, double (*f)(int, float * , float *, float *), float * weights = NULL);
      ~MaskFilter();
  	//Gaussian mask      
  	  //MaskSumFilter(int size, double sigma, double height, std::string input, std::string output);
	  int run();
	  std::string getName();
      float * get_mask(); //returns a copy of mask
      float * get_weights(); //returns a copy of weights
      int get_mask_size();

      static MaskFilter * createGaussianMaskFilter(std::string input, std::string output, double sigma, double r, double height, int template_size, bool normalize=false);

      static MaskFilter * createCircularMaskFilter(std::string input, std::string output, double r, int template_size, bool normalize=false);
      static double product_f(int n, float * data, float * mask, float * weights); //n = width * height
      static double ridge_f(int n, float * data, float * mask, float * weights); //n = 3
      static double product_f_normalized(int n, float * data, float * mask, float * weights); //n = width * height
      static double correlation_f(int n, float * data, float * mask, float * weights); //straightforward correlation
      static double L1_dist_f(int n, float * data, float * mask, float * weights);
      static double L1_dist_f_normalized(int n, float * data, float * mask, float * weights);
      static double weighted_median(int n, float * data, float * weights); //TODO
      static double L1_median_f(int n, float * data, float * mask, float * weights); // minimizes sum abs(x_i - y_i + c) over c
      static double L2_avg_f(int n, float * data, float * mask, float * weights); // minimizes sum (x_i - y_i + c)^2 over c
};

//replaces everything below the threshold by 0 and above by 1
class ThresholdFilter: public MFilter2D{
	std::string output;
	double lo, hi, lo_new, hi_new, mid_new, change_mid;
	public:
		//use mid_new = NULL to leave the mid value intact
       		ThresholdFilter(double lo, double hi, double lo_new, double hi_new,
		        	double mid_new, bool change_mid, std::string input, std::string output):
			output(output), lo(lo), hi(hi), lo_new(lo_new), hi_new(hi_new), mid_new(mid_new), change_mid(change_mid)
			{inputs2D.push_back(input);};
	int run();
	std::string getName() {return "ThresholdFilter";};
};

//thresholds the image and produces a list of connected components above the threshold
//Produces a pair of (MImage* I, MCoordDB* of MBlobObjectLocal* records)
//The pair is linked through indexing: the value of I at [i, j] is, when converted to int,
//the index of the record (unless it is -1), which means it is not inside a component
class ConnectedComponentsFilter: public MFilterMixed{
	std::string outputImg;
    std::string outputDB;
    int_coord sheet;
    //int min_area; //in squares, not in square meters
    //int max_area;
    double threshold;
	MBlobObjectLocal * bfs_label_component(float * data, int i, int j, long h, long w, int label, float scale);
	public:
		ConnectedComponentsFilter(std::string input, std::string outputImg, std::string outputDB, 
                                  int_coord sheet, double threshold):  //TODO: 'sheet' should be a field of MImage
			outputImg(outputImg), outputDB(outputDB), sheet(sheet),
   threshold(threshold) { inputs2D.push_back(input);};
		int run();
		std::string getName() {return "ConnectedComponentsFilter";}
};


//made redundant by the ArithmeticFilter
class MinusFilter: public MFilter2D{
    //std::vector<std::string> inputs;
    std::string output;
    public:
        MinusFilter(std::string input1, std::string input2, std::string output) : output(output) {inputs2D.push_back(input1); inputs2D.push_back(input2); };
        int run();
        std::string getName(); 
};


//This filter modifies existing images in place
class GapFillFilter: public MFilter2D{
    //std::vector<std::string> inputs;
    void fill_backwards(float * list, long len);
    void fill_forwards(float * list, long len);
    public:
        GapFillFilter(std::string input1)  {inputs2D.push_back(input1);};
        int run();
        std::string getName(); 

};

//This filter modifies existing images in place
class NormalizeFilter: public MFilter2D{
    public:
        NormalizeFilter(std::string input1)  {inputs2D.push_back(input1);};
        int run();
        std::string getName() { return "NormalizeFilter";};

};


//This filter modifies existing images in place
//Helpful when results are wrong around edges
class SetEdgesFilter: public MFilter2D{
    int margin;
    double value;
    public:
        SetEdgesFilter(std::string input1, int margin, double value = NAN): margin(margin), value(value)  {inputs2D.push_back(input1);};
        int run();
        std::string getName() { return "SetEdgesFilter"; };

};


  //ArithmeticFilter mimics some functional behaviour.
  //The syntax is (arg1, ..., argk, output, fn), where fn has 
  //arity k, arg1, ..., argk are image names or constants. Images must be of the same dimensions.
  //arg1 must be necessarily an image name, and image names must precede constants.
  //output consists of a matrix fn(arg1, ..., argk) of the same dimension as img1.

//not very nice.. but fast and nice usage:  tries to replicate functional programming.
class ArithmeticFilter: public MFilter2D{
    std::string output;
    double (*f1)(double);
    double (*f2)(double, double);
    double (*f3)(double, double, double);
    double (*f4)(double, double, double, double);
    double (*f5)(double, double, double, double, double);
    int arity;
    double c2;
    double c3;
    double c4;
    double c5;
    public:
        ArithmeticFilter(std::string input1,  double (*f)(double), std::string output): 
               output(output), f1(f) {inputs2D.push_back(input1); arity = 1; c2 = c3 = c4 = c5 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, double (*f)(double, double), std::string output): 
               output(output), f2(f) {inputs2D.push_back(input1); inputs2D.push_back(input2); arity = 2; c2 = c3 = c4 = c5 = NAN;};
        ArithmeticFilter(std::string input1, double c2, double (*f)(double, double), std::string output): 
               output(output), f2(f), c2(c2) {inputs2D.push_back(input1);  arity = 2; c3 = c4 = c5 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, std::string input3, double (*f)(double, double,double), std::string output): 
               output(output), f3(f) {inputs2D.push_back(input1); inputs2D.push_back(input2); inputs2D.push_back(input3); arity = 3; c2 = c3 = c4 = c5 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, double c3, double (*f)(double, double,double), std::string output): 
               output(output), f3(f), c3(c3) {inputs2D.push_back(input1); inputs2D.push_back(input2); arity = 3; c2 = c4 = c5 = NAN;};
        ArithmeticFilter(std::string input1, double c2, double c3, double (*f)(double, double,double), std::string output): 
               output(output), f3(f), c2(c2), c3(c3) {inputs2D.push_back(input1); arity = 3; c4 = c5 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, std::string input3, std::string input4, double (*f)(double, double,double,double), std::string output): output(output), f4(f) {inputs2D.push_back(input1); inputs2D.push_back(input2); inputs2D.push_back(input3); inputs2D.push_back(input4); arity = 4; c2 = c3 = c4 = c5 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, std::string input3, double c4, double (*f)(double, double,double,double), std::string output): output(output), f4(f),c4(c4) {inputs2D.push_back(input1); inputs2D.push_back(input2); inputs2D.push_back(input3);  arity = 4; c2 = c3  = c5 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, double c3, double c4, double (*f)(double, double,double,double), std::string output): output(output), f4(f), c3(c3), c4(c4) {inputs2D.push_back(input1); inputs2D.push_back(input2);  arity = 4; c2 = c5 = NAN;};
        ArithmeticFilter(std::string input1, double c2, double c3, double c4, double (*f)(double, double,double,double), std::string output): output(output), f4(f), c2(c2), c3(c3), c4(c4) {inputs2D.push_back(input1); arity = 4; c5 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, std::string input3, std::string input4, std::string input5,  double (*f)(double, double,double,double,double), std::string output): output(output), f5(f) {inputs2D.push_back(input1); inputs2D.push_back(input2); inputs2D.push_back(input3); inputs2D.push_back(input4); inputs2D.push_back(input5); arity = 5; c2 = c3 = c4 = c5 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, std::string input3, std::string input4, double c5, double (*f)(double, double,double,double,double), std::string output): output(output), f5(f), c5(c5) {inputs2D.push_back(input1); inputs2D.push_back(input2); inputs2D.push_back(input3); inputs2D.push_back(input4); arity = 5; c2 = c3 = c4 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, std::string input3,  double c4, double c5, double (*f)(double, double, double, double, double), std::string output): output(output), f5(f), c4(c4), c5(c5) {inputs2D.push_back(input1); inputs2D.push_back(input2); inputs2D.push_back(input3); arity = 5; c2 = c3 = NAN;};
        ArithmeticFilter(std::string input1, std::string input2, double c3,  double c4, double c5, double (*f)(double, double, double, double, double), std::string output): output(output), f5(f), c3(c3), c4(c4), c5(c5) {inputs2D.push_back(input1); inputs2D.push_back(input2); arity = 5; c2 = NAN;};
        ArithmeticFilter(std::string input1, double c2, double c3,  double c4, double c5, double (*f)(double, double, double, double, double), std::string output): output(output), f5(f), c2(c2), c3(c3), c4(c4), c5(c5) {inputs2D.push_back(input1); arity = 5;};
        
        int run();
        std::string getName(){return "Arithmetic filter";};
};


class MCoordDBElementPoint: public MCoordDBElement{
    public:
        double_coord LKS_coord;
        float data; //TODO: get/set?
        std::string text; 

        //Redundantly also store (x,y), although it is stored as a key in the database.
        MCoordDBElementPoint(double_coord LKS_coord, float data, std::string text=""): LKS_coord(LKS_coord), data(data), text(text){LKS_x = (int32_t) LKS_coord.first; LKS_y = (int32_t) LKS_coord.second;};
        void print(std::ostream& out = std::cout);
        static bool larger  (MCoordDBElement * x, MCoordDBElement * y);
        //void toTextStream(std::ostream& f);
};



//std::ostream& operator<<(std::ostream& os, const MCoordDBElementPoint& ic);
//inline bool operator< (const MCoordDBElementPoint*& lhs, const MCoordDBElementPoint*& rhs);

 

class MaxValuesFilter: public MFilterCoordDB {
    std::string output;
    double threshold;
    int max_elements;
    int_coord sheet;
    std::string comment;
    double min2;
    double max2;
    public:
    //at most max_elements largest points exceeding threshold.
       MaxValuesFilter(std::string input_img, std::string output, int_coord sheet, std::string comment, double threshold, int max_elements):
           output(output), threshold(threshold), max_elements(max_elements), sheet(sheet),comment(comment) {inputs2D.push_back(input_img);};

       //same, but do not add if input2 value is not within (min2, max2)
       MaxValuesFilter(std::string input1, std::string input2, std::string output, int_coord sheet, std::string comment, double threshold, double min2, double max2,  int max_elements):
           output(output), threshold(threshold), max_elements(max_elements), sheet(sheet),comment(comment), min2(min2), max2(max2) {inputs2D.push_back(input1); inputs2D.push_back(input2);};
       int run();
       std::string getName() {return "MaxValuesFilter";};
};




//Calls marker function for each object in the inputDB
class ComponentMarkerFilter: public MFilter2D {
    std::string output;
    double (*marker_function)(MObject*);
    bool append;
    double default_value;
    public: //(inputImg, inputDB) MUST be in sync as produced by the ConnectedComponentsFilter
        ComponentMarkerFilter(std::string inputImg, std::string inputDB, std::string output, 
                              double (*marker_function)(MObject*), bool append, double default_value = 0):
            output(output), marker_function(marker_function), append(append), default_value(default_value) 
        {inputsDB.push_back(inputDB); inputs2D.push_back(inputImg);};
        int run();
       std::string getName() {return "ComponentMarkerFilter";};
       static double mf_area_old(MObject * o);

};

//as a bonus adds the mass center parameter (for free!)
class AddMaxHeightFilter: public MFilterCoordDB {
    public:
        AddMaxHeightFilter(std::string blobsDB, std::string inputImg, std::string averagedImg)     {inputsDB.push_back(blobsDB); inputs2D.push_back(inputImg); inputs2D.push_back(averagedImg);};
        int run();
        std::string getName(){return "AddMaxHeightFilter";};
};

//adds the ratio of shape area and the area enclosed by a circle of the same perimeter (allways <=1)
class AddCirclenessFilter: public MFilterCoordDB {
    public:
        AddCirclenessFilter(std::string blobsImg, std::string blobsDB)
	         {inputsDB.push_back(blobsDB); inputs2D.push_back(blobsImg);};
	int run();
    std::string getName(){return "AddCirclenessFilter";};
};

class AddCorrelationFilter: public MFilterCoordDB {
	std::vector<MMask*> masks;
    bool allocate_memory;
    bool two_functions; //if true, then two correlation fields are filled
    double (*correlation_function1)(int, float*, float*, float*);
    double (*correlation_function2)(int, float*, float*, float*);
	public:
        //TODO: kopijuoti, o ne pavogti pointerį
        static double empty_fn(int a, float* b, float* c, float* d) {return 0;};
		AddCorrelationFilter(std::string blobsDB, std::string inputImg, std::vector<MMask*> masks,
                 double (*cf1)(int, float*, float*, float*),
                 double (*cf2)(int, float*, float*, float*) = empty_fn,
                 bool allocate_memory = true): masks(masks), allocate_memory(allocate_memory), correlation_function1(cf1)
        {
         inputsDB.push_back(blobsDB); 
         inputs2D.push_back(inputImg);
         two_functions = false; 
         if (cf2 != empty_fn) {
            correlation_function2 = cf2;
            two_functions = true;
         };
        };

        //convolves only the blobs
        //unfortunately not as generic as the mask filter (would need to implement similarly as Arithmetic filter)
        void convolveBlobs(MCoordDB * blobsDB, MImage * inputImg, MMask * mask, unsigned char output_index);
		int run();
        std::string getName(){return "AddCorrelationFilter";};
};


class SetBlobCenterFilter: public MFilterCoordDB {
    int center_type;
    public:
        //possible center types: "max_height", "mass_center", "ellipse_center"
        SetBlobCenterFilter(std::string blobsImg, std::string blobsDB, std::string centerType = "max_height"); 
        int run();
        std::string getName(){return "SetBlobCenterFilter";};
};

class SetVariancesFilter: public MFilterCoordDB {
    public:
        SetVariancesFilter(std::string blobsDB, std::vector<std::string> varianceImgs){inputsDB.push_back(blobsDB); inputs2D.insert(inputs2D.end(), varianceImgs.begin(), varianceImgs.end());};
        int run();
        std::string getName(){return "SetVariancesFilter";};
};

//max angle between triangles making up the surface
class AddMaxAngleFilter: public MFilterCoordDB {
    public:
        AddMaxAngleFilter(std::string blobsDB, std::string inputImg){inputsDB.push_back(blobsDB); inputs2D.push_back(inputImg);};
        int run();
        std::string getName(){return "AddMaxAngleFilter";};
};

//TODO: use a relational DB??
class SelectorFilter: public MFilterCoordDB {
    std::string output_name;
    bool (*select)(MCoordDBElement*); //selector function which returns true if the object should be selected
    bool copy;
    public:
        SelectorFilter(std::string input_db, std::string output_db, bool (*select) (MCoordDBElement *), bool copy): output_name(output_db), select(select), copy(copy) {inputsDB.push_back(input_db);};
        int run();
        std::string getName(){return "SelectorFilter";};
};

//deletes the result to_delete. Deletes only image //both image and db if both exist
class ImageDestructorFilter: public MFilter2D {
   std::string to_delete;
   public:
       ImageDestructorFilter(std::string to_delete): to_delete(to_delete) {inputs2D.push_back(to_delete);};
       int run();
       std::string getName(){return "ImageDestructorFilter";};
};

//this filter must be run on the canonic pair (blobsImg, blobsDB)
//where the blobsDB->getElement(i)->id = i
class FindNeighboursFilter: public MFilterCoordDB {
    int radius; //in meters
    public:
       FindNeighboursFilter(std::string blobsImg, std::string blobsDB, int radius): //must be paired
           radius(radius)
       {inputs2D.push_back(blobsImg); inputsDB.push_back(blobsDB);};
       int run();
       std::string getName(){return "FindNeighboursFilter";};

};


class CustomDBModifyFilter: public MFilterCoordDB {
    void (*f)(MCoordDB * db);
    public:
       CustomDBModifyFilter(std::string dbName, void (*f)(MCoordDB * db)): f(f) //must be paired
        {inputsDB.push_back(dbName);};
       int run();
       std::string getName(){return "CustomDBModifyFilter";};
};

//this will be a powerful one
class EllipseFilter: public MFilterCoordDB {
    std::vector<float> steps;
    int max_layers;
    int max_radius;
    public:
      EllipseFilter(std::string dbName, std::string inputImg, std::vector<float> steps, float max_radius = 20): steps(steps), max_radius(max_radius) {inputsDB.push_back(dbName); inputs2D.push_back(inputImg);};
      EllipseFilter(std::string dbName, std::string inputImg); //default steps 

      int run();
      std::string getName(){return "EllipseFilter";};

};

//compute the elliptic profile: average height going around elliptically
//the ellipse angle and eccentricity are determined by the last good countour ellipse
//fills ellipse_profile field in each blob
class EllipseProfileFilter: public MFilterCoordDB {
    float step_size_meters; //controls both increments in radius and steps around the ellipse
    float max_radius_meters;
    public:
      EllipseProfileFilter(std::string dbName, std::string inputImg, float step_size_meters=1, int max_radius_meters = 20): step_size_meters(step_size_meters), max_radius_meters(max_radius_meters) {inputsDB.push_back(dbName); inputs2D.push_back(inputImg);};
      int run();
      std::string getName(){return "EllipseProfileFilter";};

};

//scales the coordinate system, so that elliptic contours become circular
class EllipseNormalizeFilter: public MFilterCoordDB {
    float profile_step_size; //profile step size (meters)
    float step_size; //rescaled profile step size (meters)
    float default_length; //canonical mound radius until the ditch (meters)
    float furrow_threshold; //angle where we say that there is a furrow (typically angles start with negative, then go up at the furrow)
    float min_ellipse_length;
    float max_ellipse_mse;
    bool fully_normalize;
    int steps_from_ditch;
    public:
      EllipseNormalizeFilter(std::string dbName, std::string inputImg, float profile_step_size, float step_size, int default_length,
              float furrow_threshold, float min_ellipse_length, float max_ellipse_mse, bool fully_normalize, int steps_from_ditch): 
          //if decrease is less that 0.05 radians per meter, we treat this as a ditch 
          profile_step_size(profile_step_size),
          step_size(step_size),
          default_length(default_length),
          furrow_threshold(furrow_threshold),
          min_ellipse_length(min_ellipse_length), max_ellipse_mse(max_ellipse_mse), fully_normalize(fully_normalize), steps_from_ditch(steps_from_ditch)
      {inputsDB.push_back(dbName); inputs2D.push_back(inputImg);};
      int run();
      std::string getName(){return "EllipseNormalizeFilter";};

};

class AddRawHeightsFilter: public MFilterCoordDB {
    int radius;
    public:
        AddRawHeightsFilter(std::string blobsDB, std::string heightsImg, int radius): radius(radius)
        {inputs2D.push_back(heightsImg); inputsDB.push_back(blobsDB);};
        int run();
        std::string getName(){return "AddRawHeightsFilter";};
};



#endif
