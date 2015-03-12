#ifndef __MIMAGE_H
#define __MIMAGE_H

#include "../libhfz/libhfz_margo.h"

#include <string>
#include <iostream>
#include <stdint.h>
#include "utils.h"
#include <float.h>
#include <vector>
#include <dirent.h>
#include <algorithm>

class MImage{
    public:
        float * data;
        std::string filename;
        float scale;
        float precision;
        int32_t width;
        int32_t height;

        MImage(int32_t width=0, int32_t height=0, double scale = 1.0, float * data = NULL);
        void setDimensions(int32_t w, int32_t h); //Destroys the old image if the dimensions differ
        int loadFromHFZ(std::string filename);
        int saveHFZ(std::string filename);
        void printDebugInfo();
        ~MImage();
};

//MMask object holds the 3d shape of the mask
//The weights matrix is there to encode the "NULL" values by 0 and non-null values by 
//source is the filename passed on the mask creation

class MMask {
    public:
        std::string source;
        std::string id;
        int w;
        int h;
        MImage * mask;
        MImage * weights;

        MMask(std::string filename);
        void print(std::ostream& os);
        static std::vector<MMask*> read_all_masks(std::string directory);
};

#endif
