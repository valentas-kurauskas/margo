#include "utils.h"
#include <iostream>
#include "../shapelib/shapelib-1.3.0/shapefil.h"
#include <stdint.h>
#include "MImage.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

bool FULL_RECT = true;

///Copyright (c) 1970-2003, Wm. Randolph Franklin
///
///Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
///    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimers.
///    Redistributions in binary form must reproduce the above copyright notice in the documentation and/or other materials provided with the distribution.
///    The name of W. Randolph Franklin may not be used to endorse or promote products derived from this Software without specific prior written permission. 

int pnpoly(int nvert, double *vertx, double *verty, double testx, double testy)
{
  //print_vector("vertx", 0, nvert-1, vertx);
  //print_vector("verty", 0, nvert-1, verty);
  //cout << "testx: " << (long)testx <<" testy: "<< (long)testy << endl;
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((verty[i]>testy) != (verty[j]>testy)) &&
	 (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
       c = !c;
  }
  //cout << "result: "<<c <<endl;
  return c;
}

MImage * IMG = new MImage();
int_coord SHEET; 
string DATA_PATH = "";
string SAVE_PATH = "";
//SHEET.x = (int32_t)0; 
//SHEET.y = (int32_t)0;


double * fill_polygon(double * coordsx, double * coordsy, int n, int mini, int maxi, int minj, int maxj) {
    int h = maxi - mini + 1;
    int w = maxj - minj + 1;
    double * rect = new double[w * h];
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) {
            double_coord square = get_LKS_coords(SHEET, mini+i + 0.5, minj+j + 0.5);
            if (!FULL_RECT)
                rect[j + w * i] = (double) pnpoly(n, coordsx, coordsy, square.first, square.second);
            else rect[j + w * i] = 1.0; //take full rectangle
        };
    return rect;
}



void save_mask(int size, double * mask, double * weights, string id_str) {
  string filename = SAVE_PATH + "/"+id_str+ "_"+toString(size)+"x"+toString(size) + ".txt";
  cout << "Saving " << filename << endl;
  ofstream file;
  file.open(filename.c_str());
  file << id_str<< endl;
  file << size << " " << size << endl;
  for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++)
        file << mask [j + size * i] << " ";
      file << endl;
  };
  cout << endl;

  for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++)
        file << weights [j + size * i] << " ";
      file << endl;
  };
 file.close();
}

void extract_mask(int LKS_x, int LKS_y, int radius) {
     int_coord SHEET = get_sheet_coords(LKS_x, LKS_y);

     cout << "SHEET "<<SHEET.first <<"_"<<SHEET.second << endl;
     char x_str[33], y_str[33];
     //cout << SHEET.x << " "<<SHEET.y<<endl;
     sprintf(x_str, "%d", SHEET.first);
     sprintf(y_str, "%d", SHEET.second);
     string fname =  string(x_str) + "_" +string(y_str) + ".hfz";
     string path;
     //if (DATA_PATH.rfind(".hfz") == DATA_PATH.size() - 4) #todo..
     //else DATA_PATH + "/"+ fname;
     path = DATA_PATH;
     IMG -> loadFromHFZ(path);
     if (IMG->data == NULL) return;

     int_coord coords = get_matrix_coords(LKS_x, LKS_y, SHEET);
     int mini = max(0, coords.first - radius);
     int maxi = min(IMG->height-1, coords.first + radius);
     int minj = max(0, coords.second - radius);
     int maxj = min(IMG->width-1, coords.second +radius);

     int h = maxi - mini + 1;
     int w = maxj - minj + 1;
     double weights[w * h];
     for (int i = 0; i < w*h; i++)
         weights[i] = 1;

    //print_matrix("weights", weights, h, w);            
    double mask[w * h];
    double minh = 10000; //higher then Everest
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) {
            //cout << mini+i << " " << minj+j << " "<<  (minj+j) + (mini+i) * IMG->width << " "<<endl;
            double x = mask[j+ w * i]  = IMG->data[ (minj+j) + (mini+i) * IMG->width];
            if (x < minh) minh = x;
        };
    for (int i = 0; i < w * h; i++)
        mask[i] = weights[i] * (mask[i] - minh);

    //print_matrix("mask", mask, h, w);
    
    int size = max(w, h);
    if (size % 2 == 0) size +=1;
    double new_mask[size * size];
    double new_weights[size * size];
    for (int i = 0; i < size * size; i++) {
        new_mask[i] = 0;
        new_weights[i] = 0;
    };
    int pad_j = (size - w)/2; //center
    int pad_i = (size - h)/2; //the old shape
    for (int i = 0 ; i < h; i++)
        for (int j = 0; j < w; j++) {
            new_mask[(pad_j + j) + size * (pad_i + i)] = mask[j + w * i];
            new_weights[(pad_j + j)+size * (pad_i + i)] = weights[j + w * i];
        }

    //print_matrix("new_mask", new_mask, size, size);
    //print_matrix("new_weights", new_weights, size, size);

    string id_str = toString(LKS_x) + "_" + toString(LKS_y); 
    save_mask(size, new_mask, new_weights, id_str);
}


void process_object(SHPObject * obj, int obj_id) {
     //cout <<"object type: "<< obj -> nSHPType << " nVertices: "<< obj -> nVertices << endl;
     //for (int j = 0; j < obj -> nVertices; j++)
     //    cout << "\t" << (long) obj->padfX[j] <<", "<< (long) obj->padfY[j] << endl;
     int_coord c = get_sheet_coords(obj->padfX[0], obj->padfY[0]);
     if (SHEET.first != c.first || SHEET.second != c.second) {
        cout << "SHEET "<<c.first <<"_"<<c.second << endl;
        SHEET = c;
        char x_str[33], y_str[33];
        //cout << SHEET.x << " "<<SHEET.y<<endl;
        sprintf(x_str, "%d", SHEET.first);
        sprintf(y_str, "%d", SHEET.second);
        string fname =  string(x_str) + "_" +string(y_str) + ".hfz";;
        IMG -> loadFromHFZ(DATA_PATH + "/"+ fname);
     };
    int_coord ji[obj->nVertices];
    int minj = IMG->width, maxj = -1, mini =IMG->height, maxi = -1;
    for (int k = 0; k < obj -> nVertices; k++) {
         ji[k] =  get_matrix_coords(obj->padfX[k], obj->padfY[k], SHEET);
            //cout << "vertex "<< ji[k].first << ", " <<ji[k].second << endl;
         if (ji[k].second < minj) minj = ji[k].second;
         if (ji[k].second > maxj) maxj = ji[k].second;
         if (ji[k].first < mini) mini = ji[k].first;
         if (ji[k].first > maxi) maxi = ji[k].first;
    };
  
    //cout << mini << " "<<maxi << " " <<minj << " " << maxj << endl;

    //mini -=5;  //temp -- increase rectangle size.
    //minj -=5;
    //maxi +=5;
    //maxj +=5;

    double * weights =  fill_polygon(obj->padfX, obj->padfY, obj->nVertices, mini, maxi, minj, maxj);
    int w = maxj - minj + 1;
    int h = maxi - mini + 1;
    //print_matrix("weights", weights, h, w);            
    double mask[w * h];
    double minh = 10000; //higher then Everest
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) {
            double x = mask[j+ w * i]  = IMG->data[ (minj+j) + (mini+i) * IMG->width];
            if (x < minh) minh = x;
        };
    for (int i = 0; i < w * h; i++)
        mask[i] = weights[i] * (mask[i] - minh);

    //print_matrix("mask", mask, h, w);
    
    int size = max(w, h);
    if (size % 2 == 0) size +=1;
    double * new_mask = new double[size * size];
    double * new_weights = new double[size * size];
    for (int i = 0; i < size * size; i++) {
        new_mask[i] = 0;
        new_weights[i] = 0;
    };
    int pad_j = (size - w)/2; //center
    int pad_i = (size - h)/2; //the old shape
    for (int i = 0 ; i < h; i++)
        for (int j = 0; j < w; j++) {
            new_mask[(pad_j + j) + size * (pad_i + i)] = mask[j + w * i];
            new_weights[(pad_j + j)+size * (pad_i + i)] = weights[j + w * i];
        }

    //print_matrix("new_mask", new_mask, size, size);
    //print_matrix("new_weights", new_weights, size, size);
    string id_str = (FULL_RECT?"FR":"") + toString(obj_id); 
    save_mask(size, new_mask, new_weights, id_str);
} 


int main(int argc, char * argv[]){
    if (argc < 4) {
        cout << "Usage: convert_shp shp_input template_output_dir hfz_data_dir_or_file [POLYGON]" << endl; 
        cout << "or: convert_shp POINT template_output_dir hfz_data_dir LKS_x LKS_y radius" << endl; 
        exit(1);
    };
    cout << "\nNOTE: assuming LKS sheet naming format XX_YY.hfz\n"<<endl;
    SAVE_PATH = argv[2];
    DATA_PATH = argv[3];

    if (string(argv[1]) == "POINT") {
        if (argc < 7) 
        cout << "Usage: convert_shp POINT output_dir data_dir LKS_x LKS_y radius" << endl; 
        int LKS_x = StringToNumber(argv[4]);
        int LKS_y = StringToNumber(argv[5]);
        int radius = StringToNumber(argv[6]);
        cout << LKS_x << " "<<LKS_y << " "<<radius << endl;
        extract_mask(LKS_x, LKS_y, radius);
        exit(0);
    };

    SHPHandle hshp = SHPOpen(argv[1], "rb");
    if (hshp == NULL) {
        cout << "Error opening " << argv[1] << endl;
        exit(1);
    }
	if (argc >= 5 && string(argv[4]) == "POLYGON")
		FULL_RECT = false;
    int entities, shape_type;
    double min_bound[4];
    double max_bound[4];
    SHPGetInfo(hshp, &entities, &shape_type, min_bound, max_bound);
    cout << "n="<<  entities << " type="<<shape_type<< endl;
    
    for (int i = 0; i < 4; i++)
        cout << (long) min_bound[i] << " ";
    cout << endl;
    for (int i = 0; i < 4; i++)
        cout << (long) max_bound[i] << " ";
    cout << endl;

    for (int i = 0; i < entities; i++){
        SHPObject * obj = SHPReadObject(hshp, i);
        process_object(obj, i);
        SHPDestroyObject(obj);
    };

    SHPClose(hshp);
}





