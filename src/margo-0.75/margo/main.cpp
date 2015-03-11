#include "MImage.h"
#include "MProcessor.h"
#include "MFilter.h"
#include "MRadialFilter.h"
#include "utils.h"
#include "MConstraints.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

// Author: Valentas Kurauskas
// Licence: GNU GPL

using namespace std;

//string DATA_DIR = "/usr/home/diezel/code/post-2011/arch/data/";
//string DATA_DIR = "../test_data/";
//string ALGO = "FIXED_TEMPLATE";
//string MASK = "../learning/masks/0.txt";
//string OUTPUT_DIR = "../output/"; 
//string INPUT_PATH;
int_coord SHEET;
//bool SAVE_COMPONENTS = false; //save full componentsDB


//map<string, string> SETTINGS;

void default_settings() {
    SETTINGS["algo"] = "FIXED_TEMPLATE";
    SETTINGS["mask"] =  "../learning/masks/0.txt";
    SETTINGS["output_dir"] = "../output/"; 
}





void init_settings(int argc, char ** argv) {
    default_settings();
    for (int i = 2; i < argc; i++) {
        string s = string(argv[i]);
        unsigned int l = s.find("=");
        if (l == string::npos) continue;
        SETTINGS[s.substr(1,l-1)] = s.substr(l+1);
        cout << s.substr(1,l-1) << ":" << s.substr(l+1) << endl;
    }
}

//int main(){ //(int argc, const char* argv[]){
//  MImage img;
//  img.loadFromHFZ(DATA_DIR + "/55_59.hfz");
//  double ** data = img.getData();
//  if (data != NULL){
//      cout << "was successfull: "<< data[1000][1000]<<endl;
//  }
//  else cout <<"unsuccessfull"<<endl;
//}

//intent: compare sums of radial deviations (later: weights)
struct Cradialsorter {
      bool operator() (MCoordDBElement * x, MCoordDBElement * y) { //cout << "Comparing "<<x<<","<<y<<endl; 
           return  ((MCoordDBElementMatrix *) x)->avg(1) < ((MCoordDBElementMatrix *)y)->avg(1);} // must satisfy (x,x) = FALSE
} radialsorter;



/*
//a simple selector function
bool select2 (MCoordDBElement * el) {
    MBlobObjectLocal * blob = (MBlobObjectLocal*) el;
    float * cors = blob->max_template_correlation;
    for (int i = 0; i < blob->n_templates; i++)
            if (cors[i] > 0.7) return true; //any correlation exceeds 0.8
    return false;
}
*/



//not sure how to use std::minus
double minusf(double x, double y) {
    return x - y;
}

double minusmulf(double x, double y, double z) {
    return z*(x - y);
}

//radius and radius_to exclude input in meters
string add_variance(MProcessor& proc, int freq, int radius, int radius_to_exclude) {
  string id = toString(freq) + "_" + toString(radius);
  proc.addFilter(new GaussianFilter(freq, "input", "gf"+id)); //averaging, 7 m radius
  proc.addFilter(new MinusFilter("input", "gf"+id, "subtracted"+id));
  proc.addFilter(new ImageDestructorFilter("gf"+id));
  proc.addFilter(new ArithmeticFilter("subtracted"+id, sqr, "squared"+id));
  proc.addFilter(new ImageDestructorFilter("subtracted"+id));
  //proc.addFilter(new GaussianFilter(radius, "squared"+id, "variance" + id));
  //
  radius = radius / 2;  //because scale is 2 meters
  radius_to_exclude = radius_to_exclude / 2; 
  float * bump_tmpl = SeparableFilter::step_template(radius_to_exclude, radius);
  float * const_tmpl = SeparableFilter::constant_template(2 * radius + 1);

  proc.addFilter(new SeparableFilter("squared"+id, "var1_" + id, 2 * radius + 1, 2 * radius + 1, const_tmpl, const_tmpl, false));
  proc.addFilter(new SeparableFilter("squared"+id, "var2_" + id, 2 * radius + 1, 2 * radius + 1, bump_tmpl, bump_tmpl, false));

  delete bump_tmpl;
  delete const_tmpl;

  proc.addFilter(new ArithmeticFilter("var1_" + id, "var2_" + id, 1.0/(sqr(2 * radius+1) - sqr(radius_to_exclude+1)), minusmulf, "variance"+id));

  proc.addFilter(new ImageDestructorFilter("var1_"+id));
  proc.addFilter(new ImageDestructorFilter("var2_"+id));
  proc.addFilter(new ImageDestructorFilter("squared"+id));
  return "variance" + id;
}


void set_db_parameters(MCoordDB * db) {
    db->setDerivedInfoPrinter(print_extra_info);
}



void sliced_components_plan0(MProcessor& proc, vector<string>& to_save) {
  //GapFillFilter * gapf = new GapFillFilter("input");
  //
  proc.addFilter(new GaussianFilter(double_setting("averaging_radius", 7.0), "input", "gf7")); //averaging, 7 m radius
  proc.addFilter(new MinusFilter("input", "gf7", "subtracted7"));

  proc.addFilter(new ThresholdFilter(0.5, 1000, 0.0, 0.0, 1.0, true, "subtracted7", "thresholded"));

  proc.addFilter(new ConnectedComponentsFilter("thresholded", "componentsImg", "componentsDB", SHEET, 0.5));
  to_save.push_back("componentsImg");
}

void sliced_components_plan(MProcessor& proc, vector<string>& to_save) {
  //GapFillFilter * gapf = new GapFillFilter("input");
  //
  proc.addFilter(new GaussianFilter(double_setting("averaging_radius", 7.0), "input", "gf7")); //averaging, 7 m radius
  proc.addFilter(new MinusFilter("input", "gf7", "subtracted7"));

  
  //string var_short = add_variance(proc, 2, 10);
  string var_short = add_variance(proc, double_setting("short_var_r1", 2), int_setting("short_var_r2", 20), int_setting("short_var_exclude", 6)); //important: we now exclude variance
  string var_medium = add_variance(proc, double_setting("medium_var_r1", 10), int_setting("medium_var_r2", 20), int_setting("medium_var_exclude", 6)); //important: we now exclude variance
  string var_long = add_variance(proc, double_setting("long_var_r1", 20), int_setting("long_var_r2", 40), int_setting("long_var_exclude", 12));   //around the point.

  proc.addFilter(new ThresholdFilter(double_setting("slicing_height", 0.6), 1000, 0.0, 0.0, 1.0, true, "subtracted7", "thresholded"));

  proc.addFilter(new ConnectedComponentsFilter("thresholded", "componentsImg", "componentsDB", SHEET, 0.5*double_setting("slicing_height", 0.6)));
  
   //the input to this filter MUST be a pair (image, coorddb) from the last filter
  //proc.addFilter(new ComponentMarkerFilter("componentsImg", "componentsDB", "markedComponents", ComponentMarkerFilter::mf_area_old, false));


  //the input to this filter MUST be a pair (image, coorddb) from the last filter
  proc.addFilter(new FindNeighboursFilter("componentsImg", "componentsDB", int_setting("neighbours_radius", 40))); //adds the list of neighbours
  
  proc.addFilter(new AddMaxHeightFilter("componentsDB", "input", "gf7"));
  proc.addFilter(new SelectorFilter("componentsDB", "componentsDB1", select1, false));
  
  //proc.addFilter(new AddCirclenessFilter("componentsImg", "componentsDB1"));
  //proc.addFilter(new AddMaxAngleFilter("componentsDB1", "input"));
  proc.addFilter(new AddCorrelationFilter("componentsDB1", "input", MMask::read_all_masks(string_setting("mask")), MaskFilter::correlation_f /*, MaskFilter::L1_median_f*/));




  //saving under the same name would destroy original componentsDB which would render
  //neighbours wrong
  proc.addFilter(new SetBlobCenterFilter("componentsImg", "componentsDB1", string_setting("center_type", "max_height"))); 
  //do not know ellipse center yet, so will default to max_height

  proc.addFilter(new EllipseFilter("componentsDB1", "subtracted7"));
 
  if (string_setting("center_type", "max_height") == "ellipse_center") 
    proc.addFilter(new SetBlobCenterFilter("componentsImg", "componentsDB1", "ellipse_center"));
  
  proc.addFilter(new EllipseProfileFilter("componentsDB1", "subtracted7", double_setting("profile_step", 1), double_setting("max_profile_radius", 20))); //should probably use rigid average here; all settings are meters



  proc.addFilter(new EllipseNormalizeFilter("componentsDB1", "subtracted7", double_setting("profile_step", 1),
                                               double_setting("rescale_step", 1), 
                                               double_setting("default_ditch_distance", 6), 
                                               double_setting("ditch_threshold",-0.05),
                                               double_setting("good_ellipse_min_length", 1.4),
                                               double_setting("good_ellipse_max_mse", 0.5),
                                               bool_setting("fully_normalize", true),
                                               int_setting("steps_from_ditch", 1)));
  
  vector<string> vrncs;
  vrncs.push_back(var_short);
  vrncs.push_back(var_medium);
  vrncs.push_back(var_long);
  proc.addFilter(new SetVariancesFilter("componentsDB1", vrncs));

  if (bool_setting("save_components")) //add these later and save time if save_components=false
     proc.addFilter(new AddRawHeightsFilter("componentsDB1", "subtracted7", int_setting("raw_heights_radius", 4)));


  //do not create copies so that the changes are accessible in neighbours
  proc.addFilter(new SelectorFilter("componentsDB1", "selectionsDB", select2, false));
 
  proc.addFilter(new CustomDBModifyFilter("selectionsDB", set_db_parameters)); //add custom output

  if (not bool_setting("save_components"))
     proc.addFilter(new AddRawHeightsFilter("selectionsDB", "subtracted7", int_setting("raw_heights_radius", 4)));

  //to_save.push_back("markedComponents");
  if (bool_setting("save_hconstraints")) {
	  proc.addFilter(new SelectorFilter("selectionsDB", "hconstraintsDB", select3, false));
	  proc.addFilter(new CustomDBModifyFilter("hconstraintsDB", set_db_parameters));
	  to_save.push_back("hconstraintsDB");
  };
  
  if (bool_setting("save_components")) {
    proc.addFilter(new CustomDBModifyFilter("componentsDB1", set_db_parameters));
    to_save.push_back("componentsDB1");
  };
  to_save.push_back("selectionsDB");
}


/*
void sliced_components_plan(MProcessor& proc, vector<string>& to_save) {
  //GapFillFilter * gapf = new GapFillFilter("input");
  GaussianFilter * gf_A = new GaussianFilter(7, "input", "gf_A"); //averaging, 7 m radius
  MinusFilter * mf = new MinusFilter("input", "gf_A", "subtracted");
 
  ThresholdFilter * threshf = new ThresholdFilter(0.8, 1000, 0.0, 0.0, 1.0, true, "subtracted", "thresholded");
  ConnectedComponentsFilter * ccf = new ConnectedComponentsFilter("thresholded", "output"); 

  //proc.addFilter(gapf);
  proc.addFilter(gf_A);
  proc.addFilter(mf);
  to_save.push_back("output");

  proc.addFilter(threshf);
  proc.addFilter(ccf);
}
*/


double variance_img(double x_sqr, double x_avg) { 
    return x_sqr - x_avg * x_avg;
}

double covariance_img(double xy_conv, double x_avg, double y_avg) { 
    return xy_conv - x_avg * y_avg;
}

void gaussian_template_plan(MProcessor& proc, vector<string>& to_save) {
  //proc.addFilter(new GapFillFilter("input"));
  //GaussianFilter * gf;
  //proc.addFilter(gf = new GaussianFilter(3, "input", "convolved", 0.8, 7)); //template 3 m radius, 0.8 m height, 10 x 10 m (10 = 2x5)
  proc.addFilter(new NormalizeFilter("input"));
  MaskFilter * mf = MaskFilter::createGaussianMaskFilter("input", "convolved", 3, 3, 0.8, 7, true);
  proc.addFilter(mf);
  //to_save.push_back("convolved");   
  //proc.addFilter(new GaussianFilter(7, "input", "averaged")); //averaging, 7 m radius
  proc.addFilter(MaskFilter::createCircularMaskFilter("input", "averaged", 3, 7, true));
  //to_save.push_back("averaged");
  proc.addFilter(new ArithmeticFilter("input", sqr, "squared")); 
 
  //proc.addFilter(new GaussianFilter(7, "squared", "squared_averaged"));
  proc.addFilter(MaskFilter::createCircularMaskFilter("squared", "squared_averaged", 3, 7, true));
  
  
  float * mask = mf->get_mask();
  float * weights = mf->get_weights(); 
  int sz = mf->get_mask_size();

  double S = 0, S2 = 0, sumW = 0;
  for (int i = 0; i < sz*sz; i++) {
    S += weights[i] * mask[i];
    S2 += weights[i] * mask[i] * mask[i];
    sumW += weights[i];
  };
  double mask_mean = S /sumW; //these will be slightly wrong around the edges, but hopefully not too much
  double mask_sigma = sqrt( S2/sumW - sqr(mask_mean));

  if (DEBUGGING) cout << "S: " << S << "; mask_mean: "<<mask_mean<<"; mask_sigma: "<<mask_sigma<<endl;
  

  proc.addFilter(new ArithmeticFilter("convolved", "averaged", "squared_averaged", mask_mean, mask_sigma, 
                                      correlation,
                                      "gaussian_template")); 
  proc.addFilter(new SetEdgesFilter("gaussian_template", 4, NAN));
  to_save.push_back("gaussian_template");
}



void fixed_mask_plan(MProcessor& proc, vector<string>& to_save) {
  //proc.addFilter(new GapFillFilter("input"));
  //GaussianFilter * gf;
  //proc.addFilter(gf = new GaussianFilter(3, "input", "convolved", 0.8, 7)); //template 3 m radius, 0.8 m height, 10 x 10 m (10 = 2x5)
  
  int ww, hh;
  float * mask, * weights;
  string id;
  load_mask(string_setting("mask"), id, ww, hh, mask, weights); //make command line argument...
  if (DEBUGGING) {
      cout << id << endl;
      cout << hh << " " << ww << endl;

      print_matrix("mask", mask, hh, ww);
      print_matrix("weights", weights, hh, ww);
  };
  proc.addFilter(new NormalizeFilter("input"));
  MaskFilter * mf = new MaskFilter(1 + hh/2, mask, "input", "convolved", MaskFilter::product_f_normalized, weights);
  proc.addFilter(mf);
  //to_save.push_back("convolved");   
  //proc.addFilter(new GaussianFilter(7, "input", "averaged")); //averaging, 7 m radius
  proc.addFilter(new MaskFilter(1+hh/2, weights, "input", "averaged", MaskFilter::product_f_normalized, weights));
  //to_save.push_back("averaged");
  proc.addFilter(new ArithmeticFilter("input", sqr, "squared")); 
 
  //proc.addFilter(new GaussianFilter(7, "squared", "squared_averaged"));
  proc.addFilter(new MaskFilter(1+hh/2, weights, "squared", "squared_averaged", MaskFilter::product_f_normalized, weights));
  
  int sz = hh;

  double S = 0, S2 = 0, sumW = 0;
  for (int i = 0; i < sz*sz; i++) {
    S += weights[i] * mask[i];
    S2 += weights[i] * mask[i] * mask[i];
    sumW += weights[i];
  };
  double mask_mean = S /sumW; //these will be slightly wrong around the edges, but hopefully not too much
  double mask_sigma = sqrt( S2/sumW - sqr(mask_mean));

  if (DEBUGGING) cout << "S: " << S << "; mask_mean: "<<mask_mean<<"; mask_sigma: "<<mask_sigma<<endl;
  

  proc.addFilter(new ArithmeticFilter("convolved", "averaged", "squared_averaged", mask_mean, mask_sigma, 
                                      correlation,
                                      id+"_fixed0")); 

  //proc.addFilter(new ArithmeticFilter("convolved", "averaged", mask_mean,  
  //                                    covariance_img,
  //                                    "covariance")); 


  proc.addFilter(new SetEdgesFilter(id+"_fixed0", 1 + hh/2, 0.0));  //for some reason Python does not like too many NaNs

  proc.addFilter(new MaxValuesFilter(id+"_fixed0", id+"_max_values", SHEET, id, 0.6, 300));
  to_save.push_back(id+"_fixed0");
  to_save.push_back(id+"_max_values");
  //to_save.push_back("covariance");
}


//also selects points with specific height
void fixed_mask_plan2(MProcessor& proc, vector<string>& to_save) {
  
  int ww, hh;
  float * mask, * weights;
  string id;
  load_mask(string_setting("mask"), id, ww, hh, mask, weights); //make command line argument...
  if (DEBUGGING) {
      cout << id << endl;
      cout << hh << " " << ww << endl;

      print_matrix("mask", mask, hh, ww);
      print_matrix("weights", weights, hh, ww);
  };

  proc.addFilter(new GaussianFilter(18, "input", "averaged")); //averaging, 18 m radius (was 12)

  proc.addFilter(new ArithmeticFilter("input", "averaged", minusf, "subtracted")); 


  MaskFilter * mf = new MaskFilter(1 + hh/2, mask, "subtracted", "convolved", MaskFilter::product_f_normalized, weights);
  proc.addFilter(mf);
  proc.addFilter(new MaskFilter(1+hh/2, weights, "subtracted", "averaged2", MaskFilter::product_f_normalized, weights));
  //to_save.push_back("averaged");
  proc.addFilter(new ArithmeticFilter("subtracted", sqr, "squared")); 
 
  //proc.addFilter(new GaussianFilter(7, "squared", "squared_averaged"));
  proc.addFilter(new MaskFilter(1+hh/2, weights, "squared", "squared_averaged", MaskFilter::product_f_normalized, weights));
  
  int sz = hh;

  double S = 0, S2 = 0, sumW = 0, M = 0.1;
  for (int i = 0; i < sz*sz; i++) {
    if (mask[i] > M) M = mask[i]; //maximum elevation
    S += weights[i] * mask[i];
    S2 += weights[i] * mask[i] * mask[i];
    sumW += weights[i];
  };
  double mask_mean = S /sumW; //these will be slightly wrong around the edges, but hopefully not too much
  double mask_sigma = sqrt( S2/sumW - sqr(mask_mean));

  if (DEBUGGING) cout << "S: " << S << "; mask_mean: "<<mask_mean<<"; mask_sigma: "<<mask_sigma<<endl;
  

  proc.addFilter(new ArithmeticFilter("convolved", "averaged2", "squared_averaged", mask_mean, mask_sigma, 
                                      correlation,
                                      id+"_fixed0")); 

  //proc.addFilter(new ArithmeticFilter("convolved", "averaged", mask_mean,  
  //                                    covariance_img,
  //                                    "covariance")); 


  proc.addFilter(new SetEdgesFilter(id+"_fixed0", 1 + hh/2, 0.0));  //for some reason Python does not like too many NaNs

  proc.addFilter(new MaxValuesFilter(id+"_fixed0", "subtracted", id+"_max_values", SHEET, id, 0.85, 0.8*M, 1.2*M, 100)); //difficult to compare averaged and maxheight
  //was 0.6 * M; 1.2 * M; 300

  //to_save.push_back(id+"_fixed0"); //temp
  //to_save.push_back("subtracted"); //temp

  to_save.push_back(id+"_max_values");
  //to_save.push_back("covariance");
}



void fixed_mask_L1_plan(MProcessor& proc, vector<string>& to_save) {
  int ww, hh;
  float * mask, * weights;
  string id;
  load_mask(string_setting("mask"), id, ww, hh, mask, weights); 
  if (DEBUGGING) {
      cout << id << endl;
      cout << hh << " " << ww << endl;

      print_matrix("mask", mask, hh, ww);
      print_matrix("weights", weights, hh, ww);
  };
  proc.addFilter(new MaskFilter(1 + hh/2, mask, "input", id+"_fixedL1", MaskFilter::L1_median_f, weights));
  proc.addFilter(new SetEdgesFilter(id+"_fixedL1", 1 + hh/2, -999.0));
  proc.addFilter(new MaxValuesFilter(id+"_fixedL1", id+"_mv_L1", SHEET, id, -1000, 50));
  //to_save.push_back(id+"_fixedL1");
  to_save.push_back(id+"_mv_L1");
}


void fixed_mask_L2_plan(MProcessor& proc, vector<string>& to_save) {
  int ww, hh;
  float * mask, * weights;
  string id;
  load_mask(string_setting("mask"), id, ww, hh, mask, weights); 
  if (DEBUGGING) {
      cout << id << endl;
      cout << hh << " " << ww << endl;

      print_matrix("mask", mask, hh, ww);
      print_matrix("weights", weights, hh, ww);
  };
  double M = 0;
  for(int i = 0; i < ww * hh; i++) //maximum elevation
	  if (mask[i] > M) M = mask[i];
  proc.addFilter(new MaskFilter(1 + hh/2, mask, "input", id+"_fixedL2", MaskFilter::L2_avg_f, weights));

  proc.addFilter(new GaussianFilter(18, "input", "averaged")); //averaging, 18 m radius (was 12)
  proc.addFilter(new ArithmeticFilter("input", "averaged", minusf, "subtracted")); 

  proc.addFilter(new SetEdgesFilter(id+"_fixedL2", 1 + hh/2, -999.0));  
  //proc.addFilter(new MaxValuesFilter(id+"_fixedL2", id+"_mv_L2", SHEET, id, -1000, 50));
  proc.addFilter(new MaxValuesFilter(id+"_fixedL2", "subtracted", id+"_mv_L2", SHEET, id, -1000, 0.8*M, 1.2 * M, 100));
  //to_save.push_back(id+"_fixedL2");
  to_save.push_back(id+"_mv_L2");
}

void test_plan(MProcessor& proc, vector<string>& to_save) {
  //GapFillFilter * gapf = new GapFillFilter("input");
  //
  proc.addFilter(new GaussianFilter(7, "input", "gf7")); //averaging, 7 m radius
  proc.addFilter(new MinusFilter("input", "gf7", "subtracted7"));
  proc.addFilter(new ThresholdFilter(0.8, 1000, 0.0, 0.0, 1.0, true, "subtracted7", "thresholded"));
  proc.addFilter(new ConnectedComponentsFilter("thresholded", "componentsImg", "componentsDB", SHEET, 0.5));
  proc.addFilter(new CustomDBModifyFilter("componentsDB", set_db_parameters));
  to_save.push_back("componentsDB");
}



void test_plan_2(MProcessor& proc, vector<string>& to_save) {
  proc.addFilter(new ArithmeticFilter("input", sqr, "squared"));
  proc.addFilter(new MaxValuesFilter("squared", "max_values", SHEET, "test_id", 0.9, 200));
  to_save.push_back("squared");
  to_save.push_back("max_values");
}

void save_img(MImage * img, string name) {
    if (!img) return;
    string outputname = string_setting("output_dir") +  name + ".hfz"; // "_subtracted.hfz";
    cout << "Saving " << outputname << endl;
    img->saveHFZ(outputname.c_str());
}

void explore_matrix(string name, float * matrix, int width) {
  cout << "Exploring matrix "<<name<<endl;
  while (true) {
    cout << "i,j,w? (w = 0 - terminates)"<<endl;
    int32_t i,j,w;
    cin >> i;
    cin >> j;
    cin >> w; 
    if (w <= 0) return;
    for (int32_t y1= i; y1 < i + w; y1++){
          for (int32_t x1 = j; x1 < j+w; x1++)
            cout <<matrix[x1 + y1 * width] <<"\t";
          cout << endl;
      };
  };
}


int main(int argc, char * argv[]){ //(int argc, const char* argv[]){
  //for (int kkk = 0; kkk < argc; kkk++)
  //    cout <<kkk << " "<< argv[kkk]<<endl;

  if (argc < 2) {
      cout << "Invalid options. The calling syntax is" << endl;
      cout << "./margo <input> [-algo=FIXED_TEMPLATE/SLICES] [-data_dir=PATH_TO_DATA_DIR] [-output_dir=OUTPUT_DIR] [-mask=PATH_TO_MASK_FILE_OR_DIR] [-explore=true] [-save_components=true/false]" << endl;
      cout << "<input> can be a sheet number such as XX_YY or path to hfz file name"<<endl;
      exit(1);
  };
  init_settings(argc, argv);
  SETTINGS["output_dir"] += "/";
  //string s;

  //if (cmd_option_exists(argv, argv+argc, "-algo"))
  //  ALGO = get_cmd_option(argv, argv+argc, "-algo");

  
  //if (cmd_option_exists(argv, argv+argc, "-mask"))
  //    MASK = get_cmd_option(argv, argv+argc, "-mask");

  //if (cmd_option_exists(argv, argv+argc, "-data_dir"))
  //    DATA_DIR = get_cmd_option(argv, argv+argc, "-data_dir") + "/";

  //if (cmd_option_exists(argv, argv+argc, "-output_dir"))
  //    OUTPUT_DIR = get_cmd_option(argv, argv+argc, "-output_dir") + "/";

  //if (cmd_option_exists(argv, argv+argc, "-debug"))
  if (bool_setting("debug")) 
      DEBUGGING = true;

  //if (cmd_option_exists(argv, argv+argc, "-save_components"))
  //    SAVE_COMPONENTS = true;


  string fname(argv[1]);
  if (fname.find(string(".hfz")) == string::npos)
      SETTINGS["input_path"] = string_setting("data_dir") + fname + ".hfz";
  else {
      SETTINGS["input_path"] = fname;
      fname = fname.substr(fname.find(string(".hfz")) - 5, 5); //may segfault here.
  };

  //parse sheet number
  SHEET.first =  StringToNumber(fname.substr(0, 2));
  SHEET.second = StringToNumber(fname.substr(3, 2));
  if (SHEET.first < 0 || SHEET.second < 0) {
      cout << "Fatal error: invalid sheet number: " << fname << endl;
      exit (1);
  };
  if (DEBUGGING) {
      cout <<"ALGO: " << string_setting("algo", string("SLICES")) << endl;
      cout <<"MASK: "<< string_setting("mask") << endl;
      cout <<"INPUT_PATH: "<< string_setting("input_path") << endl;
      cout <<"fname: "<< fname << endl;
      cout << SHEET << endl;
  };
 // if (ALGO == "FIXED_TEMPLATE") {
 //     size_t k = MASK.rfind("/");
 //     if (k == string::npos) k = 0;
 //     string maskname = MASK.substr(k+1, MASK.length() - k - 5);
 //     cout << maskname << endl;
 //     fname = fname + "_" + maskname; 
 // }

  MProcessor proc;
  vector<string> files;
  files.push_back(string_setting("input_path"));
  if (DEBUGGING) cout << files[0]<<endl;

  vector<string> to_save;
  //
  string ALGO = string_setting("algo"); 
  proc.addFilter(new GapFillFilter("input")); //always want to fill the gaps.
  if (ALGO == "GAUSSIAN_TEMPLATE")
    gaussian_template_plan(proc, to_save);
  else if (ALGO == "FIXED_TEMPLATE")
    //fixed_mask_plan(proc, to_save);
    fixed_mask_plan2(proc, to_save);
  else if (ALGO == "FIXED_TEMPLATE_L1")
    fixed_mask_L1_plan(proc, to_save);
  else if (ALGO == "FIXED_TEMPLATE_L2")
    fixed_mask_L2_plan(proc, to_save);
  else if (ALGO == "SLICES")
    sliced_components_plan(proc, to_save);
  else if (ALGO == "TEST")
    test_plan(proc, to_save);
  else {
    cout << "Unknown algo. Options are GAUSSIAN_TEMPLATE/FIXED_TEMPLATE/FIXED_TEMPLATE_L1/SLICES/TEST" << endl;
    exit (1);
  };

  proc.processDataFile(files[0]);
  MFilterData * res =  proc.getData();

  float * in = res->getImage("input")->data;
  //string outputname;
  if (in == NULL){
      cout << "error: input data data is NULL";
      return -1;
   // outputname = "../output/" + fname + "_filled_gaps.hfz";
   // res->getImage("input")->saveHFZ(outputname.c_str());
  }

  vector<string> imgs = res->getImageNames();
  
  if (imgs.size()  > 1){
      //cout << "saving..." << endl;
      if (DEBUGGING) 
        for (unsigned int i = 0; i < imgs.size(); i++) {   
          if (imgs[i] == "input") continue;
	      save_img(res->getImage(imgs[i]), fname + "_" + imgs[i]);
         };
     
        //int i = imgs.size() - 1; //save the last matrix
	  for (unsigned int i = 0; i < to_save.size(); i++)
           if (res->hasImage(to_save[i]))
                save_img(res->getImage(to_save[i]), fname + "_" + to_save[i]);
           else if (res->hasDB(to_save[i]))
               res->getDB(to_save[i])->saveToXYZFile(string_setting("output_dir") + fname+"_"+to_save[i]+".xyz");
           else cout <<"Error: result with name \"" << to_save[i] << "\" has not been produced." << endl;
      
      if (DEBUGGING) cout <<"done"<<endl;
      if (bool_setting("explore") && imgs.size() > 0) {
          string to_explore; // = imgs[imgs.size()-1];
          while (true) {
              cout << "Matrix to explore? Q - quit. All matrices are: "<< endl;
              for (unsigned int j = 0; j < imgs.size(); j++)
                  cout << imgs[j] << endl;
              cin >> to_explore;
              if (to_explore == "Q") break;
              if (res->hasImage(to_explore))
                explore_matrix(to_explore, res->getImage(to_explore)->data, res->getImage(to_explore)->width);
          }
      };
      return 0;
    };
  cout <<"unsuccessfull"<<endl;
  return -1;
}


