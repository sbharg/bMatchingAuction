#ifndef GRAPH_H
#define GRAPH_H

#include <omp.h>
#include <cmath>
#include <iostream>
#include <getopt.h>
#include <float.h>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>
using namespace std;

class Node {
    public:
    int b;
    int deg;
    
    void print();
};

struct EdgeE
{
    int head;
    int id;         // Edge tail
    float weight;  // Edge weight
};
struct Edge
{
    int id;         // Edge tail
    float weight;  // Edge weight
};

class CSR
{
    public:
    int nVer;       // number of vertices 
    int nEdge;      // number of edges
    int maxDeg;
    int* verPtr;    // vertex pointer array of size nVer+1
    Edge* verInd;   // Edge array
    int rVer;       // The number of vertices on Right for bipartite graph;
    int lVer;       // The number of vertices on left for bipartite graph;
    
    bool readMtxB(char * filename, bool abs_value, bool verbose); // reading as a bipartite graph
    
    CSR():nVer(0),nEdge(0),verPtr(NULL),verInd(NULL){}
    ~CSR()
    {
        if(verPtr!=NULL)
            delete verPtr;

        if(verInd!=NULL)
            delete verInd;
    }

};

#endif //GRAPH_H
