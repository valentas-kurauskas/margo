#ifndef __MGEOM_H
#define __MGEOM_H

#include "utils.h"
#include <iostream>
#include <vector>

#include <cmath>
#include "../eigen/Eigen/Dense"

struct vector2d {
        double x;
        double y;
        vector2d(){x =0; y=0;};
        vector2d(double x, double y): x(x), y(y) {};
        vector2d rotate(double angle);
        vector2d scale(double sx, double sy);
};


struct vector3d {
        double x;
        double y;
        double z;
        vector3d(){x =0; y=0; z=0;};
        vector3d(double x, double y, double z): x(x), y(y), z(z) {};
};

std::ostream& operator<<(std::ostream& os, vector3d& v);

struct triangle {
        vector3d a;
        vector3d b;
        vector3d c;
        //triangle(vector3d a, vector3d b, vector3d c): a(a), b(b), c(c) {};
};

inline double dist(vector3d x, vector3d y);

class Curve3d {
    public:
        std::vector<vector3d> vertices;
        //bool isCycle() {return (coords.size() > 0) && (coords[coords.size()-1] == coords[0]);};
        bool isCycle;
        bool contains(double testx, double testy);
};

class MEllipse {
    public:
        double x0;
        double y0;
        double phi;
        double a;
        double b;
        std::vector<double> A;
        MEllipse(const std::vector<double> A); // A[0] x^2 + A[1] xy + A[2] y^2 + A[3]x + A[[4]y + 1 = 0
        MEllipse(double x0, double y0, double a, double b, double phi): x0(x0), y0(y0), phi(phi), a(a), b(b){};
        static MEllipse *  fitSecondOrderCurve(Curve3d * curve);
        static bool isEllipse(const std::vector<double> A);
        Curve3d * getPoints(int n_steps);
        void print(std::ostream& o);
        //double getMSE(Curve3d * curve);
};

//internal to Graph3d
class Graph3dVertex{
  public:
    vector3d coord;
    bool visited; //internal;
    std::vector<Graph3dVertex*> neighbours;
    Graph3dVertex(vector3d coord): coord(coord) {};
    Graph3dVertex * findNeighbour(vector3d x, double prec = 0.00001);
};

class Graph3d {
        void lineSearchInt(Graph3dVertex * start, Curve3d * c); //DFS that does not branch
    public:
        double prec; //distance within which points are treated as the same
        Graph3d() {prec = 0.00001;};
        std::vector<Graph3dVertex*> vertices;
        Graph3dVertex * findVertex(vector3d x);
        bool addVertex(vector3d x);
        bool addEdge(vector3d x, vector3d y);
        bool hasEdge(vector3d x, vector3d y);
        std::vector<Curve3d*> buildContours(); //explores the graph assuming it consists of paths and cycles
        Curve3d * lineSearch(Graph3dVertex * start); //DFS that does not branch
        ~Graph3d(); //deletes all vertices
};

//if the last element is the same as the first, then the curve is closed
typedef std::vector<vector3d> curve;

//tests if the section (a,b) intersects the plane z=h
inline bool intersects(vector3d a, vector3d b, double h);

//returns the intersection of (a,b) with z=h
inline vector3d intersection(vector3d a, vector3d b, double h);

//returns list of triangles 
std::vector<triangle> triangulation(float * elevation, int width, int height);

//main function of this header
std::vector<Curve3d*> contour(const std::vector<triangle> triangulation, double h);

//MSE of the curve & a second order curve A[0] x^2 + A[1] xy + A[2]y^2 + A[3] x + A[4]y + 1 = 0 | : A[2]
double getMSE(Curve3d * curve, const std::vector<double> A); 

vector3d barycentric_coords(vector2d a, vector2d b, vector2d c);

//also a faster way: if a,b,c are C + (1,0), (0,0), (0,1), then the result is (p.y - c.y, c.x-p.x+c.y-p.y, 1 - ...)
//if a,b,c are C + (1,0), (0,1), (1,1), then the result is (c.x-p.x, c.y-p.y, 1 - ...)

#endif



