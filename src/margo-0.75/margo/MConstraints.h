#ifndef __MCONSTRAINTS_H
#define __MCONSTRAINTS_H

#include "MCoordDB.h"
#include "MObject.h"
#include "utils.h"
#include <ostream>

bool select1 (MCoordDBElement * el); //select points just based on height and area for main processing
bool select2 (MCoordDBElement * el); //output should make up automatic learning set
bool select3 (MCoordDBElement * el); //hand-crafted constraints to pick the "best" matches

//derived parameters
bool good_neighbour(MCoordDBElement * el);
int number_of_good_neighbours(MBlobObjectLocal * y);
int number_of_good_ellipses(MBlobObjectLocal * y); 
double combined_ellipse_mse(MBlobObjectLocal * y);
double ellipse_asymmetry(MBlobObjectLocal * y); 
void print_extra_info(MCoordDBElement * x, std::ostream& o); 


#endif
