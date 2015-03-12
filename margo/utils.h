#ifndef __MUTILS_H
#define __MUTILS_H

#include <cmath>
#include <sstream>
#include <string>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <map>


double Phi(double x);
//#define DEBUGGING false

extern bool DEBUGGING;
extern const double PI;

extern int VAR_SHORT_IDX; 
extern int VAR_MEDIUM_IDX; //variance indices used in main and MConstraints.
extern int VAR_LONG_IDX;   //at present they must be 0,1 and 2 respectively
extern std::map<std::string,std::string> SETTINGS; //settings

template <typename T>
std::string toString (T Number)
  {
     std::ostringstream ss;
     ss << Number;
     return ss.str();
  }

template <typename T>
void print_vector(std::string name, int start, int end, T * vector) {
    std::cout << name <<": ";
    for (int i = start; i < end; i++){
        std::cout << vector[i]<<" ";
    };
    std::cout <<std::endl;
}

template <typename T>
void print_matrix(std::string name, T * matrix, int nrows, int ncols, std::ostream& os) {
    os << name <<": "<<std::endl;
    for (int i = 0; i < nrows; i++){
        for (int j = 0; j < ncols; j++)
        {
            os << matrix[j + i * ncols]<<"\t";
        };
        os <<std::endl;
    };
}

template <typename T>
void print_matrix(std::string name, T * matrix, int nrows, int ncols) {
    print_matrix(name, matrix, nrows, ncols, std::cout);
}




template <typename T1, typename T2>
T1 min(T1 a, T2 b) {
    return a < b? a: b;
}


template <typename T1, typename T2>
T1 * copy_array(T1 * array, T2 length) {
    if (length == 0) return NULL;
    T1 * result = new T1[length];
    for (T2 i = 0; i < length; i++) {
        result[i] = array[i];
    };
    return result;
}


template <typename T1, typename T2>
void normalize_array(T1 * array, T2 length) {
    T1 S = 0;
    for (T2 i = 0; i < length; i++) {
        S+=array[i];
    };

    for (T2 i = 0; i < length; i++)
        array[i]/=S;
}




template <typename T>
T sqr(T x){
    return x*x;
}

template <typename T>
double avg(T * array, int length){
    double S;
    for (int i = 0; i < length; i++)
        S+=array[i];
    return ((double)S)/length;
}


template <typename T>
double median(T * array, int length){
    if (length == 0) return NAN;
    T *  new_array = copy_array(array, length);
    std::nth_element(new_array, new_array+length/2, new_array+length);
    T med = new_array[length/2];
    delete new_array;
    return med;
}

struct int_coord{
    int32_t first;
    int32_t second;
    };

std::ostream& operator<<(std::ostream& os, const int_coord& ic);

struct double_coord{
    double first;
    double second;
    };



//given an LKS coordinate returns 1:5000 (1:10000??) sheet number
int_coord get_sheet_coords(double LKS_x, double LKS_y);
//returns column and row numbers within the sheet 
int_coord get_matrix_coords(double LKS_x, double LKS_y, int_coord sheet, double scale = 2.0);

double_coord get_LKS_coords(int_coord sheet, double i, double j, double scale = 2.0);
//template <typename T>
//void show (T text, bool endLine = true, int level = 1)
//  {
//     if (level >= SILENCE_LEVEL){
//         std::cout  << text;
//         if (endLine) std::cout << std::endl;
//     }
//  }


void load_mask(std::string path, std::string& identifier, int& w, int& h, float *& mask, float *& weights);





int StringToNumber ( const std::string Text );

double StringToDouble ( const std::string Text );




double correlation(double xy_conv, double x_avg, double x_sqr, double y_avg, double sigma_y);


std::string string_setting(std::string name, std::string default_value = "");

int int_setting(std::string name, int default_value = 0);

double double_setting(std::string name, double default_value = NAN);

bool bool_setting(std::string name, bool default_value = false);

#endif
