#ifndef __MCOORDDB_H
#define __MCOORDDB_H

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <ostream>
#include <stdint.h>
#include "utils.h"

class MCoordDBElement{ //is int32_t enough?
    public:
        double LKS_x;
        double LKS_y;
        virtual void print(std::ostream& os=std::cout);
        virtual std::string coordToString();
        virtual MCoordDBElement * getCopy();
        virtual ~MCoordDBElement(){};
        //virtual void toTextStream(std::ostream& f) = 0;
};
//can this be made class member? probably not, because I can't modify ostream
//NOTE: this should not output the coordinate, only data;
std::ostream& operator<<(std::ostream& os, MCoordDBElement& ic);
//inline bool operator< (const MCoordDBElement*& lhs, const MCoordDBElement*& rhs);

//template <typename Type>
class MCoordDB{
    protected:
        //std::vector<int32_t> X; //may be useful to store separately if performance is an issue.
        //std::vector<int32_t> Y; //but is less convenient (i.e. sort)
        std::vector<MCoordDBElement*> data;
        void (*derivedInfoPrinter) (MCoordDBElement * x, std::ostream& o); 
    public:
        //MCoordDB();
        void append(MCoordDBElement * d);
        std::vector<double> getX();
        std::vector<double> getY();
        std::vector<MCoordDBElement*> getData();
        MCoordDBElement * getElement(int i);
        void setData(std::vector<MCoordDBElement*>& new_data); 
        void print(); //print each element
        unsigned int size(); //get the number of stored elements
        void saveToTextFile(std::string name);
        void saveToXYZFile(std::string filename);
        void deleteAll();
        //Set manual function that prints derived info. Assumes that it will print endl;
        void setDerivedInfoPrinter(void (*f) (MCoordDBElement * x, std::ostream& o)) {derivedInfoPrinter = f;};
        ~MCoordDB();
};


#endif
