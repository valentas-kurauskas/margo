#ifndef __M_RADIAL_FILTER_H
#define __M_RADIAL_FILTER_H
#include <vector>
#include "MImage.h"
#include "utils.h"
#include "MFilter.h"
#include "MCoordDB.h"
#include <string>
#include <ostream>



//radial processor function (int,int, double*) -> double. The parameters are: radius, length, 
//values traveling around the discretized circle at that radius. 
//TODO?: allow to see all levels for this function
typedef double (*arrayfunction)(int, int, float*);

//the "condition function" MImage*, parameters* -> vector<coord>. It should
//return the list of indices which should be processed. 
////The hope here is most of the indices will NOT be processed.
typedef std::vector<unsigned long> (*conditionfunction)(MImage*, void*);

double average(int r, int n, float * input);
double variance(int r, int n, float * input);

std::vector<unsigned long> thresholdFunction(MImage* image, void* param);

//I am now missing functional programming, since I would
//like to pass any list of functions as a parameter for MRadialFilter
//I don't want to mess up with creating containers and objects
//for each function

//BUT IT IS POSSIBLE: http://stackoverflow.com/questions/9410/how-do-you-pass-a-function-as-a-parameter-in-c 

//const double PI = std::atan(1.0f) * 4.0f;

//simple matrix. in our case the first index will be the function(i.e., AVG, VARIANCE,..)
//and the second index will be the level (distance from the center)
//
//TODO: implement getCopy() and destructor.
class MCoordDBElementMatrix: public MCoordDBElement{
    public:
        float ** data; //TODO: get/set?
        int nfunctions;
        int nlevels;

        //Redundantly also store (x,y), although it is stored as a key in the database.
        MCoordDBElementMatrix(unsigned int x, unsigned int y, int nfunctions, int nlevels, float ** data): data(data), nfunctions(nfunctions), nlevels(nlevels){LKS_x = x; LKS_y = y;};
        float avg(int index); //compute average
        void print(std::ostream& out = std::cout); 
        //void toTextStream(std::ostream& f);
        //TODO: destructor, copy?
};



//std::ostream& operator<<(std::ostream& os, const MCoordDBElementMatrix& ic);
//inline bool operator< (const MCoordDBElementMatrix*& lhs, const MCoordDBElementMatrix*& rhs)


class MRadialFilter: public MFilterCoordDB{
    protected:
        std::string output;
        std::string condimage;
        int radius;
        std::vector<arrayfunction> procfns; 
        conditionfunction condfn;
        void* condfnpars;
    public:
        MRadialFilter(int radius, std::string input, std::string condimage, std::string output, 
                arrayfunction processingfn, conditionfunction condfn, 
                void * condfnpars = NULL);
        void addProcessingFunction(arrayfunction fn);
        int run();
        std::string getName();
};


#endif


