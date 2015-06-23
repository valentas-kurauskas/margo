#include "utils.h"

using namespace std;

bool DEBUGGING = false;
const double PI = 2 * acos(0);
int VAR_SHORT_IDX = 0;
int VAR_MEDIUM_IDX = 1; 
int VAR_LONG_IDX = 2;
std::map<std::string,std::string> SETTINGS;

double Phi(double x){
    // constants
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x)/sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return 0.5*(1.0 + sign*y);
}

int_coord get_sheet_coords(double LKS_x, double LKS_y) {
    int_coord result;
    //std::cout << (LKS_x - 200000)/5000 << std::endl;
    //std::cout << (int32_t) (LKS_x - 200000)/5000 << std::endl;
    result.first = (int32_t)((LKS_x - 200000) / 5000);
    result.second = (int32_t)( (LKS_y - 5900000) / 5000);
    return result;
}
//(i,j)
int_coord get_matrix_coords(double LKS_x, double LKS_y, int_coord sheet, double scale) {
    int_coord result;
    result.second = (int32_t) (LKS_x - (200000 + sheet.first * 5000))/scale;
    result.first = (int32_t) (LKS_y - (5900000 + sheet.second * 5000))/scale;
    return result;
};

double_coord get_LKS_coords(int_coord sheet, double i, double j, double scale) {
    double_coord result;
    result.first = (200000 + sheet.first * 5000) + scale * j;
    result.second = (5900000 + sheet.second * 5000) + scale * i;
    return result;
}

void load_mask( std::string path, std::string& identifier, int& w, int& h, float *& mask, float *& weights) {
    std::ifstream file(path.c_str());
    file >> identifier;
    file >> w;
    file >> h;
    //std::cout << identifier <<" "<< w<< " "<<h << std::endl;
    mask = new float[w * h];
    weights = new float[w * h];
    for (int i = 0; i < w * h; i++)
        file >> mask[i];
    for (int i = 0; i < w * h; i++)
        file >> weights[i];
    file.close();
}



int StringToNumber ( const std::string Text )
{                              
    std::stringstream ss(Text);
	int result;
	return ss >> result ? result : -1;
}


std::ostream& operator<<(std::ostream& os, const int_coord& ic)
{
    os << "<" << ic.first << "," << ic.second << ">" <<std::endl;
    return os;
}


double correlation(double xy_conv, double x_avg, double x_sqr, double y_avg, double sigma_y) {
    double result = (xy_conv  - x_avg * y_avg) / (sqrt(x_sqr - x_avg*x_avg) * sigma_y);
    return result;
}


double StringToDouble (const std::string s){
    return atof(s.c_str());
}


//TODO: move to a seperate file and cleanup
string string_setting(string name, string default_value) {
    if (SETTINGS.find(name) == SETTINGS.end())
            return default_value;
    return SETTINGS[name];
}

int int_setting(string name, int default_value){
        if (SETTINGS.find(name) == SETTINGS.end())
            return default_value;
        return StringToNumber(SETTINGS[name]);
}

double double_setting(string name, double default_value) {
    if (SETTINGS.find(name) == SETTINGS.end())
            return default_value;
    return StringToDouble(SETTINGS[name]);
}

bool bool_setting(string name, bool default_value) {
    if (SETTINGS.find(name) == SETTINGS.end())
            return default_value;
    return SETTINGS[name] == "true";
}
