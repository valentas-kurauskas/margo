#include "MConstraints.h"

using namespace std;

bool select1 (MCoordDBElement * el) {
    //cout << el << endl;
    MBlobObjectLocal * blob = (MBlobObjectLocal*) el;
    //if ( (blob->n_coords >= 2)  && (blob->n_coords <= 20) ) cout << blob->max_height << " ";
    return (blob->area >= double_setting("min_blob_area",8))  && (blob->area <= double_setting("max_blob_area", 320)) && (blob->max_height < double_setting("max_blob_height",2)); //0 -> 0.5
    //platesnis pasirinkimas nei select2, kadangi šiems taškams reikia suskaičiuoti
    //neighbours_good.
    //TMP 2->6
}



bool good_neighbour(MCoordDBElement * el) {
 return select1(el) && select2(el); // &&  (( (MBlobObjectLocal*) el)->median_correlation() > 0.6);
};


int number_of_good_neighbours(MBlobObjectLocal * y) {
    int good_neighbours = 0;
    for (int i = 0; i < y->n_neighbours; i++) {
        if ( good_neighbour(y->neighbours[i]))
            good_neighbours++;
    };
    return good_neighbours;
}

float ellipse_vert_mse(MBlobObjectLocal * y) {
    float result = 0;
    int n = 0;
    int M = min(y->n_ellipse_profile, int_setting("vert_mse_max",10));
    for (int i = int_setting("vert_mse_min",2); i < M; i++) {
        result+=y->ellipse_profile_var[i];
        n+=1;
    };
    return result/n;
}



int number_of_good_ellipses(MBlobObjectLocal * y) {
    int good_ellipses = 0;
    for (int i = 0; i < y->n_ellipses; i++) {
        float mse = y->ellipse_mse[i]; 
        float  length = sqrt( sqr(y->ellipse_a[i]) + sqr(y->ellipse_b[i]));
        if ( (length >= double_setting("good_ellipse_min_length", 1.4) ) && (mse != NAN) && (mse < double_setting("good_ellipse_max_mse",2))) good_ellipses++;
    };
    return good_ellipses;
}

double combined_ellipse_mse(MBlobObjectLocal * y) {
    double max_mse = -1;
    for (int i = 0; i < min(int_setting("good_ellipse_levels", 3), y->n_ellipses); i++) { //maximum over 3 levels
        float  length = sqrt( sqr(y->ellipse_a[i]) + sqr(y->ellipse_b[i]));
        if (y->ellipse_mse[i] == NAN) max_mse = 10;
        else if ( (length >= double_setting("good_ellipse_min_length", 1.4)) && (y->ellipse_mse[i] > max_mse)) max_mse = y->ellipse_mse[i];
    };
    if (max_mse == -1) max_mse = 40;
    return max_mse;
}

double ellipse_asymmetry(MBlobObjectLocal * y) {
    double max_asymmetry = -1;
    for (int i = 0; i < min(int_setting("good_ellipse_levels", 3), y->n_ellipses); i++) { //maximum over 3 levels
        float length = sqrt( sqr(y->ellipse_a[i]) + sqr(y->ellipse_b[i]));
        float asym = log(y->ellipse_a[i] / y->ellipse_b[i]);
        if ( (length >= double_setting("good_ellipse_min_length", 1.4)) && (max_asymmetry < asym) ) max_asymmetry = asym;
    };
    if (max_asymmetry == -1) max_asymmetry  = 10;
    return max_asymmetry;
}



//output of this should be used by human learning



//output of this should be used by human learning
//TODO: loadable from file
//Grąžina apie 50/120 pilkapių iš training set
bool select2 (MCoordDBElement * el) {
    MBlobObjectLocal * blob = (MBlobObjectLocal*) el;

    //cout << (*blob) << endl;

    //tai svarbiausias požymis, todėl verta ištraukt
    //int gn = number_of_good_neighbours(blob);
    //if ((gn >=3) && (gn*1.0/blob->n_neighbours > 0.5)) return true;
    if ( (blob->n_ellipses >=int_setting("n_good_ellipses_auto_select", 4)) && (combined_ellipse_mse(blob) < double_setting("good_ellipse_mse_auto_select",0.4))) return true; 
    //rankinis
    int ref_ellipse_id = int_setting("ref_ellipse_id",1);

    return (blob->area >= double_setting("min_mound_area",16))  &&
         (blob->area<=double_setting("max_mound_area",140)) &&
         (blob->max_height >= double_setting("min_mound_height", 0.6)) &&
         (blob->max_height <= double_setting("max_mound_height", 2)) &&  //buvo 1.35
         (blob->median_correlation() >  double_setting("min_median_correlation", 0.31)) &&
         (blob->variances[VAR_MEDIUM_IDX] < double_setting("max_noise", 0.9)) && 
         (ellipse_asymmetry(blob) < double_setting("max_ellipse_asymmetry", 1.0)) &&
         (blob->n_ellipses>=int_setting("good_ellipse_levels",3)) &&
         (blob->ellipse_a[ref_ellipse_id] >= double_setting("min_ellipse_a", 2)) &&
         (blob->ellipse_a[ref_ellipse_id] <= double_setting("max_ellipse_a", 6.2)) &&
         //(blob->n_neighbours < 15) && //nepamato didelių pilkapynų!!!
         (combined_ellipse_mse(blob) < double_setting("max_ellipse_combined_mse",12.0)); 
}



//a simple selector function
//(not used in default algo)
bool select3 (MCoordDBElement * el) {
    MBlobObjectLocal * blob = (MBlobObjectLocal*) el;

    if (blob->n_ellipses < 3) return false;
    if ( ellipse_asymmetry(blob) > 0.6) return false;
    float mse = combined_ellipse_mse(blob);
    if (mse > 1) return false;
    if ( (blob->n_ellipses >=4) && (mse < 0.3)) return true;

    if ((blob->median_correlation() > 0.8) && (blob->variances[VAR_MEDIUM_IDX] < 0.7)) return true;


    int ngn = number_of_good_neighbours(blob);
    if ( (ngn >= 5) && blob->median_correlation() > 0.7) return true;
    if ( (blob->n_neighbours >= 6) && (ngn <= 2)) return false;
    if ( (blob->n_neighbours >= 10) && (ngn <= 4)) return false;

    if (blob->variances[VAR_MEDIUM_IDX] > 0.4) return false;
    //if (blob->variances[0] > 0.04) return false;
    if (blob->variances[VAR_LONG_IDX] > 0.5) return false;
    //if (blob->circleness < 0.5) return false;
    if (blob->median_correlation() < 0.7) return false;
    if (blob->n_coords < 4) return false;
    return true;
}


void print_extra_info(MCoordDBElement * x, std::ostream& o) {
    MBlobObjectLocal * y = (MBlobObjectLocal*) x;
    o << "NEIGHBOURS_GOOD=" << number_of_good_neighbours(y) << endl;
    o << "NEIGHBOURS_TOTAL="<<y->n_neighbours<<endl;

    o << "ELLIPSE_N_GOOD=" << number_of_good_ellipses(y) << endl;
    o << "ELLIPSE_COMBINED_MSE="<<combined_ellipse_mse(y) << endl;
    o << "ELLIPSE_ASYMMETRY="<<ellipse_asymmetry(y) << endl;
    o << "ELLIPSE_VERT_MSE="<<ellipse_vert_mse(y) << endl;
}


