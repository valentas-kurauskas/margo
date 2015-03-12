#include "MCoordDB.h"

using namespace std;


//bool operator< (const MCoordDBElement*& lhs, const MCoordDBElement*& rhs) 
//    {return (lhs->LKS_x < rhs_LKS_x) || ( (lhs->LKS_x == rhs->LKS_x) && (lhs->LKS_y < lhs_LKS_y))};

void MCoordDBElement::print(ostream& out) {
    //cout << coordToString() <<endl;
    //cout << (*this) << endl;
    out << "<Basic MCoordDBElement>";
}

string MCoordDBElement::coordToString() {
    return("<" + toString(LKS_x) + "," + toString(LKS_y)+">");
}

MCoordDBElement * MCoordDBElement::getCopy() {
    MCoordDBElement * result = new MCoordDBElement();
    result->LKS_x = LKS_x;
    result->LKS_y = LKS_y;
    return result;
}

//template <typename Type> 
void MCoordDB::append(MCoordDBElement *  d){
    data.push_back(d);
}

//template <typename Type> 
vector<double> MCoordDB::getX(){
    vector<double> result;
    for (unsigned int i = 0; i < data.size(); i++)
        result.push_back(data[i]->LKS_x);
    return result;
};

//template <typename Type> 
vector<double> MCoordDB::getY(){
    vector<double> result;
    for (unsigned int i = 0; i < data.size(); i++)
        result.push_back(data[i]->LKS_y);
    return result;
};



ostream& operator<<(ostream& os, MCoordDBElement& ic) {
    //os << "<Basic MCoordDBElement>"; // ic.LKS_x << " " << ic.LKS_y;
    ic.print(os);
    return os;
}

//template <typename Type> 
vector<MCoordDBElement*> MCoordDB::getData(){return data;};

//void MCoordDB::setData(){this->data = data;}
void MCoordDB::setData(vector<MCoordDBElement*>& new_data) {
    data = new_data;
}


void MCoordDB::print(){
    for (unsigned int i = 0; i<data.size(); i++) {
        cout << data[i]->LKS_x <<", "<< data[i]->LKS_y << ": "<<endl;
        cout <<"#"<<i << " "<< (*data[i]); //data[i]->print();
    };
};

unsigned int MCoordDB::size(){
    return data.size();
}


void MCoordDB::saveToTextFile(string filename){
    cout << "Saving "<<filename <<endl;
    ofstream f;
    f.open(filename.c_str());
    for (unsigned int i = 0; i < data.size(); i++){
        f << "#" <<i<< " " << fixed<< data[i]->LKS_x << " "<<data[i]->LKS_y << endl;
        f << (*data[i]) << endl;
    };
    f.close();
}

void MCoordDB::saveToXYZFile(string filename){
    cout << "Saving "<<filename <<endl;
    ofstream f;
    f.open(filename.c_str());
    for (unsigned int i = 0; i < data.size(); i++){
        f << "DESCRIPTION=Point " << i << " " << endl;
        //f << "NAME=" << (*data[i]) << endl;
        f << (*data[i]); //assumes it will print endl
        if (derivedInfoPrinter != NULL) derivedInfoPrinter(data[i], f);
        f << fixed << data[i]->LKS_x << ","<<data[i]->LKS_y << endl << endl;
    };
    f.close();
}

MCoordDBElement * MCoordDB::getElement(int i) {
    return data[i];
}

void MCoordDB::deleteAll() {
    for (unsigned int i = 0; i < data.size(); i++)
        if (data[i] != NULL) delete data[i];
}

MCoordDB::~MCoordDB(){
    deleteAll();
}
