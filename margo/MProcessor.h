
#ifndef __MARGO_PROCESSOR_H
#define __MARGO_PROCESSOR_H

#include <vector>
#include "MFilter.h"
#include "MImage.h"
#include <iostream>

class MFilter;
class MFilterData;

class MProcessor{
    protected:
       std::vector<std::string> filenames; //input images   
       std::vector<MFilter*> filters;
       MFilterData * data;
    public: 
        MProcessor();
        void setData(MFilterData * data);
        MFilterData * getData();
        void processDataFile(std::string filename);
        void addFilter(MFilter * filter);
        //void clearImages();
        //MImage * getImage(std::string name);
        //void setDB(std::string name, MCoordDB * db);
        //MCoordDB * getDB(std::string name);
        //void setImage(std::string name, MImage * img);
};

class MFilterData{
    protected: 
       std::map<std::string, MImage*> images; //initial and temporary 2D maps
       std::map<std::string, MCoordDB*> dbs;  //temporary/final data in coord->data format
    public:
       void loadImages(); //load specified filenames from HFZ files into the images vector
       void setImage(std::string name, MImage * image);
       bool hasImage(std::string name);
       MImage * getImage(std::string name);
       void setDB(std::string name, MCoordDB* db);
       bool hasDB(std::string name);
       MCoordDB* getDB(std::string name);
       void clear(); //deletes all images. TODO: delete all dbs.
       std::vector<std::string> getImageNames();
       std::vector<std::string> getDBNames();
};




#endif
