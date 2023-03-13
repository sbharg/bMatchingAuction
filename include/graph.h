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

struct EdgeE {
    int head;
    int id;         // Edge tail
    float weight;   // Edge weight

    bool operator==(const EdgeE& e) const {
        return (this->id == e.id && this->head == e.head);
    }
    
    bool operator <(const EdgeE& e) const {
        if (this->head != e.head && this->id != e.id)
            return (this->weight < e.weight);
        else
            return (this->id == e.id);
    }

    bool operator >(const EdgeE& e) const {
        if (this->head != e.head && this->id != e.id)
            return (this->weight > e.weight);
        else
            return (this->id == e.id);
    }
};

struct Edge {
    int id;         // Edge tail
    float weight;   // Edge weight

    bool operator==(const Edge& e) const {
        return (this->id == e.id);
    }

    bool operator <(const Edge& e) const {
        return (this->weight < e.weight);
    }

    bool operator >(const Edge& e) const {
        return (this->weight > e.weight);
    }
    
};

namespace std {
    template<> struct hash<Edge> {
        size_t operator()(const Edge& e) const noexcept {
            return e.id;
        }
    };
}

class Node {
    public:
    int b;
    int deg;
    //set<pair<double, EdgeE>> pq;  // Priority queue for objects
    //unordered_set<Edge> matched; // For O(1) lookup times

    void print();
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
