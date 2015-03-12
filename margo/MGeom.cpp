#include "MGeom.h"

using namespace std;

using namespace Eigen;

double dist(vector3d a, vector3d b) {
    return sqrt( sqr(a.x - b.x) + sqr(a.y-b.y) + sqr(a.z - b.z));
}

bool intersects(vector3d a, vector3d b, double h) {
        //cout << a << " X "<<b <<": ";
        //cout << (((a.z > h) ^ (b.z > h)) ? "YES":"NO") <<endl; 
	return (a.z > h) ^ (b.z > h);
}

ostream& operator<<(ostream& os, vector3d& v) {
    os << "("<<v.x<<", "<<v.y<<", "<<v.z<<")";
    return os;
}



vector3d intersection(vector3d a, vector3d b, double h) {
	double r = (h - a.z) / (b.z - a.z);
	vector3d c(a.x + (b.x - a.x) * r,
                   a.y + (b.y - a.y) * r,
	           h);

        //cout << "intersection: " << a << " and "<< b<<endl;
        //cout << c << endl;
	return c;
}


vector<triangle> triangulation(float * elevation, int width, int height) {
    vector<triangle> result;
    for (int i = 1; i < height; i++)
        for (int j = 1; j< width; j++) {
           triangle t;
           t.a = vector3d(i-1, j-1, elevation[width * (i-1) + j-1]); //  
           t.b = vector3d(i, j-1, elevation[width * i +  j-1]);
           t.c = vector3d(i-1, j, elevation[width * (i-1) + j]);
           result.push_back(t);
           t.a = vector3d(i-1, j, elevation[width * (i-1) +  j]); //  
           t.b = vector3d(i, j, elevation[width * i + j]);
           t.c = vector3d(i, j-1, elevation[width * i + j-1]);
           result.push_back(t);
        };
    return result;
}




vector<Curve3d*> contour(const vector<triangle> triangulation, double h){
    Graph3d graph;
    for (unsigned int i = 0; i < triangulation.size(); i++) {
        triangle t = triangulation[i];
	bool b1 = intersects(t.a, t.b,h);
        bool b2 = intersects(t.b, t.c,h);
        bool b3 = intersects(t.a, t.c,h);

        if (b1 && b2) graph.addEdge(intersection(t.a, t.b, h), intersection(t.b, t.c, h));
        else if (b2 && b3) graph.addEdge(intersection(t.a, t.c, h), intersection(t.b, t.c, h));
        else if (b1 && b3) graph.addEdge(intersection(t.a, t.b, h), intersection(t.a, t.c, h));
	};
    return graph.buildContours();
}

Graph3dVertex * Graph3dVertex::findNeighbour(vector3d x, double prec) {
    for (unsigned int i = 0; i < neighbours.size(); i++)
        if (dist(neighbours[i]->coord, x) < prec)
            return neighbours[i];
    return NULL;
}

Graph3dVertex * Graph3d::findVertex(vector3d x) {
    for (unsigned int i = 0; i < vertices.size(); i++)
        if (dist(vertices[i]->coord, x) < prec)
            return vertices[i];
    return NULL;
}

bool Graph3d::addVertex(vector3d x) {
    if (!findVertex(x)) return false;
    vertices.push_back(new Graph3dVertex(x));
    return true;
}

bool Graph3d::hasEdge(vector3d x, vector3d y) {
    Graph3dVertex * v = findVertex(x);
    if (!v) return false;
    return (v->findNeighbour(y));
}

bool Graph3d::addEdge(vector3d x, vector3d y) {
    //cout << "adding edge: "<<x<< "--"<<y<<endl;
    Graph3dVertex * a = findVertex(x);
    if (!a) {
        a = new Graph3dVertex(x);
        vertices.push_back(a);
    };
    Graph3dVertex * b = findVertex(y);
    if (!b) {
        b = new Graph3dVertex(y);
        vertices.push_back(b);
    };
    if (a->findNeighbour(y)) return false;
    a->neighbours.push_back(b);
    b->neighbours.push_back(a); //undirected;
    return true;
}

Graph3d::~Graph3d() {
    for (unsigned int i = 0; i < vertices.size(); i++)
        delete vertices[i];
}


vector<Curve3d*> Graph3d::buildContours() {
    //cout << "Graph has " << vertices.size() << " vertices"<<endl;

    for (unsigned int i = 0; i < vertices.size(); i++)
        vertices[i]->visited = false;

    vector<Curve3d*> result;

    for (unsigned int i = 0; i < vertices.size(); i++)
        if ((!vertices[i]->visited) && (vertices[i]->neighbours.size() <=1))
            result.push_back(lineSearch(vertices[i]));

    for (unsigned int i = 0; i < vertices.size(); i++)
        if ((!vertices[i]->visited) && (vertices[i]->neighbours.size() ==2))
        {
            Curve3d* a  = lineSearch(vertices[i]);
            if ( hasEdge(a->vertices[a->vertices.size()-1], vertices[i]->coord))
                a->isCycle = true; //a->vertices.push_back(vertices[i]->coord); //closing the cycle
            else cout << "missing loop in a contour" << endl;
            result.push_back(a);
            //cout << "Added a contour of size "<< a->vertices.size() << endl;
        };

    for (unsigned int i = 0; i < vertices.size(); i++)
        if (!vertices[i]->visited) cout << "problematic contour: flat triangles?"<<endl;

   
    //cout << "Returned " << result.size() << " contours"<<endl;
    return result;
};

//just descends dfs
void Graph3d::lineSearchInt(Graph3dVertex * start, Curve3d * c) {
    start->visited = true;
    c->vertices.push_back(start->coord);
    for (unsigned int i = 0; i < start->neighbours.size(); i++)
        if (!start->neighbours[i]->visited) {
            lineSearchInt(start->neighbours[i],c);
            return;
         };
};


Curve3d * Graph3d::lineSearch(Graph3dVertex * start) {
    Curve3d * c = new Curve3d();
    lineSearchInt(start, c);
    return c;
}

double getMSE(Curve3d * curve, const vector<double> A) {
    double S = 0;
    for (int i = 0; i < int(curve->vertices.size()) - 1; i++) {
        double x = curve->vertices[i].x;
        double y = curve->vertices[i].y;
        S+= sqr(A[0] * x * x/A[2] + A[1] * x * y/A[2] + y*y + A[3] * x/A[2] + A[4] * y/A[2] + 1/A[2]);
    }
    return S/(curve->vertices.size()-1);
}

MEllipse *  MEllipse::fitSecondOrderCurve(Curve3d * curve) {
    if (curve->vertices.size() <= 0) {
        cout << "fitSecondOrderCurve: zero size" << endl;
        exit(1);
    };
    //cout << "Fitting curve with "<<curve->vertices.size() <<" vertices"<<endl;
    MatrixXd A(curve->vertices.size()-1, 5);
    VectorXd b(curve->vertices.size()-1);
    for (int i =0; i < int(curve->vertices.size()-1); i++) {
        double x = curve->vertices[i].x;
        double y = curve->vertices[i].y;
        A(i, 0) = x*x;
        A(i,1) = x*y;
        A(i,2) = 1;//y*y;
        A(i,3) = x;
        A(i,4) = y;
        b(i) = -y*y; //-1.0;
    };
    //cout << "Here is the matrix A:\n" << A << endl;
    //cout << "Here is the right hand side b:\n" << b << endl;

    VectorXd result = A.jacobiSvd(ComputeThinU | ComputeThinV).solve(b);
    /*if (DEBUGGING) {
      cout << "original result: ";
      for (int i =0; i < 5; i++)
          cout <<result(i) << " ";
      cout << endl;
    };*/


    //cout << "Here is the result:\n" << result << endl;
    
    double C = result(2);//TODO
    result(2) = 1;  
    vector<double> res(5);

    
    for (int i = 0; i < 5; i++){
        res[i] = result(i)/C;
        //cout << res[i] << " ";
    }
    //cout << endl;

    //cout << "MSE: "<<getMSE(curve, res) << endl;
    
    /*
    vector<double> alt(5);
    
    alt[0] = 1.0/25;
    alt[1] =  0;//0.00001;
    alt[2] = 1.0/9;
    alt[3] = -2.0/5;
    alt[4] = -2.0/3;
    cout << "MSE ALT: " << getMSE(curve, alt) << endl;
    */
    if (! isEllipse(res)) return NULL;
    return new MEllipse(res);
}

bool MEllipse::isEllipse(const vector<double> A) 
{
    return 4 * A[0] * A[2] - A[1] * A[1] > 0;
}

double get_ellipse_angle(const vector<double> A) {
    double a = A[0], b= A[1]/2, c=A[2];
    if ((b == 0) && (a < c)) return 0;
    if ((b == 0) && (a >= c)) return 0.5*PI;
    if (a < c) return 0.5 * atan( 2* b/(a-c) );
    if (a == c) return 0.5 * PI;
    if (a >c) return 0.5*PI + 0.5 * atan(2 *b / (a-c)); 
    return NAN;
};

double get_ellipse_a(const vector<double> A) {
    double a = A[0], b= A[1]/2, c=A[2], d = A[3]/2, f=A[4]/2, g = 1.0;
    //Wolfram
    double C1 = 2 * (a * f * f + c * d * d + g*b*b - 2 * b*d*f - a * c*g);
    double C2 = b*b - a * c;
    double C3 = sqrt((a-c) * (a-c) + 4 * b * b) - (a+c);
    return  sqrt(C1/(C2 * C3));
}

double get_ellipse_b(const vector<double> A) {
    double a = A[0], b= A[1]/2, c=A[2], d = A[3]/2, f=A[4]/2, g = 1.0;
    //Wolfram
    double C1 = 2 * (a * f * f + c * d * d + g*b*b - 2 * b*d*f - a * c*g);
    double C2 = b*b - a * c;
    double C3 = -sqrt((a-c) * (a-c) + 4 * b * b) - (a+c);
    return  sqrt(C1/(C2 * C3));
}



MEllipse::MEllipse(const vector<double> A) {
    if (!isEllipse(A)) {
        cout << "Error in constructing ellipse: non-ellipse" << endl;
        return;
    };
    //cout << "CONSTRUCTOR!!!"<<endl;
    double a = A[0], b= A[1], c=A[2], d = A[3], e=A[4];
    this->A = A;
    if (a < 0) {
        for (int i = 0; i < 5; i++)
            this->A[i] = - this->A[i];
    };
    double I2 = 4 * a * c - b * b;
    this->x0 = (b*e - 2 * c * d)/I2;
    this->y0 = (b*d - 2 * a *e)/I2;
  
    //double C = 1 + a * x0*x0 + b * x0 * y0 + c*y0 * y0 + d * x0 + e * y0;

    //double B =- (a + c);
    //double DD = sqrt(B * B - I2);
    //double lambda1 = 0.5 * (-B - DD), lambda2 = 0.5 * (DD-B);
    
    this->a = get_ellipse_a(A);
    //cout << "a: "<<this->a << endl;
    //cout << "Karčiausko: "<< sqrt(-C / lambda1)<<endl;

    this->b = get_ellipse_b(A);
    //cout << "b: "<<this->b << endl;
    //cout << "Karčiausko: "<< sqrt(-C / lambda2)<<endl;

    //cout << atan(2*(lambda1 - a)/b) << endl;
    //cout << atan(2*(lambda2 - a)/b) << endl;
    //cout << "lambda1: "<< lambda1 << endl;
    //cout << "lambda2: "<< lambda2 << endl;
    //cout << "a: "<< this->a << endl;
    this->phi = get_ellipse_angle(A); //atan(2*(lambda1 - this->a)/this->b);
    //cout << "phi: "<<this->phi << endl;
    //cout << "Karčiausko: "<<atan(2*(lambda1 - a)/b) << endl;
};


Curve3d * MEllipse::getPoints(int n_steps) {
    if (n_steps > 1000 || n_steps < 2) return NULL; //šlamštas
    Curve3d * result = new Curve3d();
    double delta = 2 * PI / (n_steps-1);
    double sin_phi = sin(phi);
    double cos_phi = cos(phi);
    double t = 0;
    for (int i = 0; i < n_steps; i++) {
        t+=delta;
        double x = x0 + a * cos(t) * cos_phi - b * sin(t) * sin_phi;
        double y = y0 + a * cos(t) * sin_phi + b * sin(t) * cos_phi;
        result->vertices.push_back(vector3d(x, y, 0));
    };
    return result;
}

void MEllipse::print(ostream& o) {
    o << "x0="<<x0 <<", y0="<<y0<<", a="<<a<<", b="<<b<<", phi="<<phi;
}

///Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
///
///    Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimers.
///    Redistributions in binary form must reproduce the above copyright notice in the documentation and/or other materials provided with the distribution.
///    The name of W. Randolph Franklin may not be used to endorse or promote products derived from this Software without specific prior written permission. 

//based on pnpoly (see above)
bool Curve3d::contains(double testx, double testy)
{
  if (!isCycle) return false;
  int i, j, c = 0;
  for (i = 0, j = vertices.size()-1; i < (int)vertices.size(); j = i++) {
    if ( ((vertices[i].y>testy) != (vertices[j].y>testy)) &&
	 (testx < (vertices[j].x-vertices[i].x) * (testy-vertices[i].y) / (vertices[j].y-vertices[i].y) + vertices[i].x) )
       c = !c;
  }
  return (bool) c;
}


vector3d barycentric_coords(vector2d p, vector2d a, vector2d b, vector2d c){
    float D = (b.y - c.y) * (a.x - c.x) + (c.x-b.x) * (a.y-c.y);
    vector3d res;
    res.x = ((b.y - c.y) * (p.x - c.x) + (c.x-b.x) * (p.y-c.y))/D;
    res.y = ((p.y - c.y) * (a.x - c.x) + (c.x-p.x) * (a.y-c.y))/D;
    return res;
}

vector2d vector2d::rotate(double phi) {
    double x_new = x * cos(phi)  - y * sin(phi);
    double y_new = x * sin(phi) + y * cos(phi);
    return vector2d(x_new,y_new);
}

vector2d vector2d::scale(double sx, double sy) {
    return vector2d(x * sx, y * sy);
}
