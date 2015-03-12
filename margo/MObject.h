#ifndef __MARGO_OBJECT_H
#define __MARGO_OBJECT_H

#include "MCoordDB.h"
#include <vector>
#include "utils.h"
#include <iostream>

class MObject: public MCoordDBElement {
    public:
        int32_t id;
};

//Truputį chaotiškas apibrėžimas

class MBlobObjectLocal: public MObject {
    public:
        //int32_t area; //in square meters
	    //int32_t id;   //redundant, but safer...
        int32_t coord;  //int32_t i;    //_local coords:_ of the first point of the blob
                        //int32_t j;    //
        int32_t mini; //rectangle boundaries
        int32_t maxi; //TODO: ar jie turi būti lokalioj, ar LKS koord. sistemoj
        int32_t minj; //lokalioj: +greitas apdirbimas
        int32_t maxj; //LKS: +lengvesnis darbas vėliau
        int32_t * coords; //index to local matrix: 0 .. w * h - 1
        int32_t n_coords;

        float area; //in square meters

        int32_t raw_cols;
        int32_t raw_rows;
        float * raw_heights; //dimensions stored outside

        //atributes
        float max_height;
        int32_t max_height_coord;
        int32_t mass_center_coord;
        
        char n_templates;  //waste of space, but simpler
        float * max_template_correlation;  //the number of templates should be stored outside
        int32_t * max_template_correlation_coords;

        char n_templates2;  
        float * max_template_correlation2;           
        int32_t * max_template_correlation_coords2;

        char n_variances;
        float * variances;

        
        int32_t n_neighbours;
        MObject ** neighbours;
        
        float circleness;
        
        float max_angle;

        int32_t n_ellipses;
        float * ellipse_a;
        float * ellipse_b;
        float * ellipse_phi;
        float * ellipse_mse;
        //int32_t * ellipse_center;
        float * ellipse_center_h;
        float * ellipse_center_v;
        
        int32_t n_ellipse_profile;
        float * ellipse_profile;
        float * ellipse_profile_var;

        float * rescaled;
        int32_t rescaled_width;

        float furrow_distance;
        int32_t ref_el;


        MCoordDBElement * getCopy();
        double median_correlation();
        double median_correlation2();
        void print(std::ostream& o);
        static MBlobObjectLocal * read_next(std::ifstream& file);
        void print_extended(std::ostream& o);
        ~MBlobObjectLocal();
};

//class MBlobObjectDerived: public MBlobObjectLocal { //nasty temporary class until I implement saving whole objects in binary format...

//}


/*

class MBlobObject: public MObject {
    public:
        double area;
        int32_t min_LKS_x; //rectangle boundaries
        int32_t max_LKS_x; //TODO: ar jie turi būti lokalioj, ar LKS koord. sistemoj
        int32_t min_LKS_y; //lokalioj: +greitas apdirbimas
        int32_t max_LKS_y; //LKS: +lengvesnis darbas vėliau
};

class MExtendedBlobObject: public MObject {
    public:
        int_coord center;
        int_coord * allCoords;
        double * allElevations;          //  I don't know if I want to store them
//        double area;                     //  can be made into vector later
        double height;                   //  at center; can be made into vector
        double length;                   //  
        double width;                    //
        double amplitude;                //  max - min
        double maxElevation;             //  above the sea level
        double environmentNoise;         //  can be made into vector
//        bool fullSquare;                 //  true
        double * coeffs;                 //  i.e. correlations with templates
        int nCoeffs;                     //  number of coeffs     
        std::vector<MObject*> neighbours;//  all neighbours at certain distance
};
*/

#endif

