#include "MImage.h"
using namespace std;

MImage::MImage(int32_t width, int32_t height, double scale, float * data){
    if (data == NULL) {
        this->data = NULL;
        setDimensions(width, height);
    }
    else {
        this->width = width;
        this->height = height;
        this->data = data;
    };
    this->scale = scale;
}

//destroys the old image, if the dimensions change
void MImage::setDimensions(int32_t w, int32_t h){

 if (((w != width) || (h != height)) && (data != NULL)){
     delete[] data;
     data = NULL;
  }

   // get width/height/scale
  width = w; // in pixels
  height = h; // in pixels

  if (w*h == 0) return; //Do not create nonsense pictures

  if (data == NULL){
    data = new float[height * width];
    //for (int i = 0; i <height; i++)
    //    data[i] = new float[width];
  };

};

int MImage::loadFromHFZ(string filename){
  // declare some variables
  float* pMyMap; // a handle to a float array, to be initialised by LibHFZ
  hfzHeader fh;  // a LibHFZ header struct
 
  // load the file
  int32_t rval = hfzLoadEx(filename.c_str(), fh, &pMyMap);
  if(LIBHFZ_STATUS_OK != rval) {
    // report error (using MFC; sorry!)
        cout << "Error when loading: LibHFZ returned " << hfzGetErrorStr(rval) << endl;
        return -1;
   }
 
  setDimensions(fh.nx, fh.ny);
  scale = fh.HorizScale; // in metres per pixel
  precision = fh.Precis;

  if (DEBUGGING) {
  cout << "width: "<<width << "; height: "<<height << endl;
  cout << "Horizontal scale (metres per pixel): "<<scale << endl;
  cout << "Precision: "<<fh.Precis << "; tilesize: " <<fh.TileSize<<"; header blocks: "<<fh.nExtHeaderBlocks << endl;
  };
  //for (long i= 0; i < height * width; i++)
  //       data[i] = pMyMap[i];
  data = pMyMap;

  hfzHeader_Reset(fh); // deallocate header info
  //hfzFree(pMyMap); // release map data allocated by hfzLoadEx
  this->filename = filename;
  return 0;
 };


MImage::~MImage(){
    setDimensions(0,0);
}


int MImage::saveHFZ(string filename){
 hfzHeader fh;
 int32_t rval = hfzHeader_Init(fh, width, height,256, 0.01, scale,0); // unsigned short TileSize, float Precis, float HorizScale, long nExtHeaderBlocks);
 if(LIBHFZ_STATUS_OK != rval) {
        cout << "Error when initialising header: LibHFZ returned " << hfzGetErrorStr(rval) << endl;
        return -1;
   };
 rval = hfzSave(filename.c_str(), LIBHFZ_FORMAT_HF2_GZ, fh, data);
 if(LIBHFZ_STATUS_OK != rval) {
        cout << "Error when saving header: LibHFZ returned " << hfzGetErrorStr(rval) << endl;
        return -1;
   };
 hfzHeader_Reset(fh); // deallocate header info
 return 0;
 }

void MImage::printDebugInfo() {
    cout << "Dimensions: " << width << ", " << height << endl;
    cout <<"Image info: ";
    if (data == NULL) {
        cout << "NULL" <<endl;
        return;
    };
    double maxv = DBL_MIN, minv = DBL_MAX, min_non_nan = DBL_MAX, max_non_nan = DBL_MIN;
    for (int i = 0; i < width * height; i++)
    {
        if (data[i]  > maxv) maxv = data[i];
        if (data[i] < minv) minv = data[i];
        if ( (data[i]  > max_non_nan) && (data[i] != NAN) ) max_non_nan = data[i];
        if ( (data[i]  < min_non_nan) && (data[i] != NAN) ) min_non_nan = data[i];
    };
    cout << "Max: " <<maxv <<"; min: "<<minv <<"; max non NAN: "<<max_non_nan <<"; min non NAN: "<<min_non_nan << endl;
}


//--------------------------------------------------------------------

MMask::MMask(string filename) {
        float * mask_data, * weights_data;
        source = filename;
        if (DEBUGGING) cout << "Reading mask from: "<< filename << endl;
        load_mask(filename, id, w, h, mask_data, weights_data); //make command line argument...
        if ((w <= 0) || (w > 10000) || (h <=0) || (h >10000)) {
		cout << "Fatal error in reading mask:";
      		cout << "dimensions w: "<< w<<" h:"<<h<<endl;
		exit(1);
	}
	if (DEBUGGING) cout << "w: "<<w<<" h:"<<h<<endl;
	mask = new MImage(w, h, 1.0, mask_data);
        weights = new MImage(w, h, 1.0, weights_data);
        //cout << "mask created."<< endl;
};

void MMask::print(ostream& os) {
    os << "id: "<<id << endl;
    os << "source: "<<source << endl;
    print_matrix("mask", mask->data, w, h, os);
    print_matrix("weights", weights->data, w, h, os);
}

vector<MMask*> MMask::read_all_masks(string directory){
  DIR *dir;
  struct dirent *ent;
  vector<string> names;
  if ((dir = opendir (directory.c_str())) != NULL) {
  /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      string name(ent->d_name);
      if ( (name.size() >=4) && (name.substr(name.size() - 4) == ".txt") ){
          //cout << "Adding new mask "<<name << endl;
          names.push_back(name);
      };
    }
   closedir (dir);
  };

  sort(names.begin(), names.end());
  vector<MMask*> masks;
  for (unsigned int i = 0; i < names.size(); i++)
          masks.push_back(new MMask(directory + "/" + names[i]));
  return masks;
};



