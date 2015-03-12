#include "MProcessor.h"
using namespace std;

MProcessor::MProcessor(){
    data = new MFilterData();
}

void MProcessor::setData(MFilterData * data){
    this->data = data;
}

MFilterData * MProcessor::getData(){
    return data;
}

void MProcessor::addFilter(MFilter * filter){
    filters.push_back(filter); 
}

void MProcessor::processDataFile(string filename){
    MImage * input = new MImage();
    input->loadFromHFZ(filename);
    data->setImage("input", input);

    for (unsigned int i = 0; i < filters.size(); i++){
        //if(DEBUGGING) 
            //filters[i]->printname();
        //if (DEBUGGING) 
        cout << "Running "<<filters[i]->getName()<<"..."<<endl;
        filters[i]->connect(data);
        if (DEBUGGING) cout <<"connected"<<endl;
        filters[i]->run();
        if (DEBUGGING) cout << "run completed"<<endl;
        filters[i]->upload();
        //filters[i]->setResult(this);

        //map<string, MImage*> l = filters[i]->lastResult();
        //map<string, MImage*>::iterator it;
        //for (it = l.begin(); it != l.end(); ++it){
        //    images[(*it).first] = (*it).second; //overwrites the existing values.
        //}
        //cout << "update completed"<<endl;
        //images.insert(l.begin(), l.end());
    };

}

void MFilterData::clear(){
    for (map<string, MImage*>::iterator it = images.begin(); it != images.end(); ++it)
        images.clear();
}

void MFilterData::loadImages(){
}


bool MFilterData::hasImage(string name){
    return images.find(name) != images.end();
}

bool MFilterData::hasDB(string name){
    return dbs.find(name) != dbs.end();
}


MImage * MFilterData::getImage(string name){
    if (images.find(name) == images.end()){
        cout << "FATAL ERROR: no image with name "<<name<<endl;
        return NULL;
    };
    return images[name];
}


void MFilterData::setImage(string name, MImage * image){
    images[name] = image;
}

MCoordDB * MFilterData::getDB(string name){
    if (dbs.find(name) == dbs.end()){
        cout << "FATAL ERROR: no image with name "<<name<<endl;
        return NULL;
    };
    return dbs[name];
}


void MFilterData::setDB(string name, MCoordDB * db){
    dbs[name] = db;
}

vector<string> MFilterData::getImageNames(){
	vector<string> v;
	for(map<string,MImage*>::iterator it = images.begin(); it != images.end(); ++it) {
		  v.push_back(it->first);
	};
	return v;
}

vector<string> MFilterData::getDBNames(){
	vector<string> v;
	for(map<string,MCoordDB*>::iterator it = dbs.begin(); it != dbs.end(); ++it) {
		  v.push_back(it->first);
	};
	return v;
}


