#include "MObject.h"

using namespace std;

void MBlobObjectLocal::print(ostream& os){
    os << "NAME=BL" << id << endl;
    //os << "AREA="<< n_coords << endl;
    os << "AREA="<< area << endl;
    os << "MAX_HEIGHT="<<max_height <<endl;

    if (n_variances >= 3) {
       os << "NOISE1=" << variances[VAR_SHORT_IDX]<<endl;
       os << "NOISE2=" << variances[VAR_MEDIUM_IDX]<<endl;
       os << "NOISE3=" << variances[VAR_LONG_IDX]<<endl;
       //for (int i = 0; i < n_variances; i++)
       //    os <<variances[i] << " ";
       //os << endl;
    };

    if (n_templates > 0) {
      os << "MEDIAN_CORRELATION="<<median_correlation()<<endl;
      os << "TEMPLATE_CORRELATION=";
      for (int i = 0; i < n_templates; i++)
          os <<max_template_correlation[i] << " ";
      os << endl;
    };

    if (n_templates2 > 0) {
      os << "MEDIAN_CORRELATION2=" <<median_correlation2()<<endl;
      os << "TEMPLATE_CORRELATION2=";
      for (int i = 0; i < n_templates2; i++)
          os <<max_template_correlation2[i] << " ";
      os << endl;
    };
     
    /*
    if (n_neighbours > 0) {
      //cout << n_neighbours << endl;
      os << "NEIGHBOURS=";
      for (int i = 0; i < n_neighbours; i++) {
          //cout << neighbours[i] << endl;
          os << neighbours[i]->id << " ";
      };
      os << endl;
    };
    */

    if (circleness != 0)
        os << "CIRCLENESS="<<circleness << endl;

    if (max_angle > 0)
        os << "MAX_ANGLE="<< max_angle << endl;

    if (n_ellipses > 0) {
      os << "ELLIPSE_A=";
      for (int i = 0; i < n_ellipses; i++)
          os <<ellipse_a[i] << " ";
      os << endl;

      os << "ELLIPSE_B=";
      for (int i = 0; i < n_ellipses; i++)
          os <<ellipse_b[i] << " ";
      os << endl;

      os << "ELLIPSE_PHI=";
      for (int i = 0; i < n_ellipses; i++)
          os <<ellipse_phi[i] << " ";
      os << endl;

      os << "ELLIPSE_MSE=";
      for (int i = 0; i < n_ellipses; i++)
          os <<ellipse_mse[i] << " ";
      os << endl;
    };

    if (raw_cols * raw_rows != 0) {
        os << "RAW_COLS="<<raw_cols << endl;
        os << "RAW_ROWS="<<raw_rows << endl;
        os << "RAW_HEIGHTS=";
        int nn = -1 + raw_cols * raw_rows;
        for (int i = 0; i < nn; i++)
            os << raw_heights[i] << " ";
        os << raw_heights[nn] << endl;
    };
    if (n_ellipse_profile > 0) {
       os << "ELLIPSE_PROFILE=";
       for (int i = 0; i < n_ellipse_profile; i++)
           os <<ellipse_profile[i] << " ";
       os << endl;
       os << "ELLIPSE_PROFILE_VAR=";
       for (int i = 0; i < n_ellipse_profile; i++)
           os <<ellipse_profile_var[i] << " ";
       os << endl;
       os << "REF_ELLIPSE="<< ref_el << endl;
    };
    if (rescaled_width > 0) {
       os << "RESCALED_WIDTH=" << rescaled_width << endl;
       os << "RESCALED=";
       for (int i = 0; i < sqr(rescaled_width); i++)
           os <<rescaled[i] << " ";
       os << endl;
    };

    if (furrow_distance > 0)
        os << "DITCH_DISTANCE="<< furrow_distance << endl;


    //os << "ZZ_LCOORDS=" <<coord << endl; //<< i <<" "<< j << endl;
    //os << "ZZ_MASS_CENTER_COORD=" <<mass_center_coord << endl; //<< i <<" "<< j << endl;
  
}

double MBlobObjectLocal::median_correlation() {
    return median(max_template_correlation, n_templates);
}


double MBlobObjectLocal::median_correlation2() {
    return median(max_template_correlation2, n_templates2);
}



void MBlobObjectLocal::print_extended(ostream& os){
    os << "NAME=BL" << id << endl;
    //os << "AREA="<< n_coords << endl;
    os << "AREA="<< area << endl;
    os << "LCOORDS=" <<coord << endl; //<< i <<" "<< j << endl;
    os << "BBOX="<< mini <<" "<< maxi << " " << minj << " " << maxj << endl;
    os << "MAX_HEIGHT="<<max_height <<endl;
    os << "MAX_HEIGHT_COORD="<<max_height_coord <<endl;
    if (n_templates > 0) {
      os << "TEMPLATE_CORRELATION=";
      for (int i = 0; i < n_templates; i++)
          os <<max_template_correlation[i] << " ";
      os << endl;
    };

    if (n_templates2 > 0) {
      os << "TEMPLATE_CORRELATION2=";
      for (int i = 0; i < n_templates2; i++)
          os <<max_template_correlation2[i] << " ";
      os << endl;
    };

    os << "CIRCLENESS="<<circleness << endl;

    if (n_variances >= 3) {
       os << "NOISE1=" << variances[VAR_SHORT_IDX]<<endl;
       os << "NOISE2=" << variances[VAR_MEDIUM_IDX]<<endl;
       os << "NOISE3=" << variances[VAR_LONG_IDX]<<endl;
       //for (int i = 0; i < n_variances; i++)
       //    os <<variances[i] << " ";
       //os << endl;
    };

    /*if (n_variances > 0) {
       os << "VARIANCE=";
       for (int i = 0; i < n_variances; i++)
           os <<variances[i] << " ";
       os << endl;
    }; */

    os << "COORDS=";
    for (int i = 0; i < n_coords; i++)
        os <<coords[i] << " ";
    os << endl;

}

//there must be a simpler way to copy structures like this? boost?
//CURRENTLY UNUSED. TODO: fill missing info (ellipse)
MCoordDBElement * MBlobObjectLocal::getCopy() {
    MBlobObjectLocal * r = new MBlobObjectLocal();
    r->LKS_x = LKS_x;
    r->LKS_y = LKS_y;
    r->id = id;
    r->coord = coord;
    r->mini = mini;
    r->maxi = maxi;
    r->minj = minj;
    r->maxj = maxj;
    r->n_coords = n_coords;
    r->coords = copy_array(coords, n_coords);
    r->max_height = max_height;
    r->max_height_coord = max_height_coord;
    r->mass_center_coord = mass_center_coord;
    r->n_templates = n_templates;
    r->max_template_correlation = copy_array(max_template_correlation, (int)n_templates);
    r->max_template_correlation_coords = copy_array(max_template_correlation_coords, (int)n_templates);
    r->n_templates2 = n_templates2;
    r->max_template_correlation2 = copy_array(max_template_correlation2, (int)n_templates2);
    r->max_template_correlation_coords2 = copy_array(max_template_correlation_coords2, (int)n_templates2);
    r->n_variances = n_variances;
    r->variances = copy_array(variances, (int)n_variances);  
    
    r->n_neighbours = n_neighbours;
    r->neighbours = copy_array(neighbours, (int)n_neighbours);

    r->n_ellipse_profile = n_ellipse_profile;
    r->ellipse_profile = copy_array(ellipse_profile, (int) n_ellipse_profile);
    r->ellipse_profile_var = copy_array(ellipse_profile_var, (int)n_ellipse_profile);
    r->ref_el = ref_el;

    r->circleness = circleness;
    return r;

}

MBlobObjectLocal::~MBlobObjectLocal() {
    if (n_coords > 0) delete coords;
    if (n_templates > 0) {
        delete max_template_correlation;
        delete max_template_correlation_coords;
    };
    if (n_templates2 > 0) {
        delete max_template_correlation2;
        delete max_template_correlation_coords2;
    };
    if (n_variances > 0) delete variances;
    if (n_neighbours > 0) delete neighbours;
    if (n_ellipse_profile > 0) {
        delete ellipse_profile;
        delete ellipse_profile_var;
    };
}

/*
static MBlobObjectDerived::MBlobObjectDerived * read_next(std::ifstream& file){
    MBlobObjectLocal * result;
    for (int i = 0; file.good()  && (i < 1000000);  i++){
        while (file.good() && (line.length()==0)) getline(file, line);
        int pos = line.find_first_of("=");
        if (result == NULL) result = new MBlobObjectLocal();
        if (pos < line.length()) {
            string fname = line.substr(0, pos);
            int pos2 = line.find_last_of("\r\n");
            string data = line.susbtr(pos+1, pos2);

            if (fname == "MAX_HEIGHT") result->max_height = StringToDouble(data);
            else if (fname == "AREA") result->n_coords = StringToNumber(data);
            else if (fname == "NAME") result->id = StringToNumber(data.substr(2));
            else if (fname == "LOCAL_VARIANCE") StringToDoubleVector(data, result->n_variances, result->variances);
            else if (fname == "TEMPLATE_CORRELATION") StringToDoubleVector(data, result->n_templates, result->max_template_correlation);
            else if (fname == "CIRCLENESS") result->circleness = StringToNumber(data);
            else if (fname == "ELLIPSE_A") StringToDoubleVector(data, result->n_ellipses, result->ellipse_a);
            else if (fname == "ELLIPSE_B") StringToDoubleVector(data, result->n_ellipses, result->ellipse_b);
            else if (fname == "ELLIPSE_PHI") StringToDoubleVector(data, result->n_ellipses, result->ellipse_phi);
            else if (fname == "ELLIPSE_MSE") StringToDoubleVector(data, result->n_ellipses, result->ellipse_mse);
            else if (fname == "NEIGHBOURS_GOOD") result->neighbours_good = StringToNumber(data); //TODO: išspręsti DERIVED->FILE->DERIVED
            else if (fname == "NEIGHBOURS_TOTAL") result->neighbours_total = StringToNumber(data); //: išsaugoti visą reikiamą informaciją?
            else if (fname == "ELLIPSE_N_GOOD") result->ellipse_n_good = StringToNumber(data);
            else if (fname == "ELLIPSE_COMBINED_MSE") result->ellipse_combined_mse = StringToDouble(data);
            else if (fname == "ELLIPSE_ASSYMETRY") result->ellipse_asymetry = StringToDouble(data);
            else if (fname == "NICE_V") result->y = StringToDouble(data);
        };
        else {
                pos = line.find_first_of(",");
                result->LKS_x = StringToDouble(data.substr(0,pos));
                line = line.substr(pos+1);
                pos = line.find_first_of(",");
                result->LKS_y = StringToDouble(data.substr(0,pos));
            };


        };

    };
}

*/
