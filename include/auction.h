#ifndef AUCTION_H
#define AUCTION_H

#include "graph.h"
#include <set>
#include <utility>
#include <unordered_set>

void bMatchingAuction(CSR* G, Node* S, double epsilon, bool verbose);

void bFactorAuction(CSR* G, Node* S, double epsilon, bool verbose);

void bFactorAuction2(CSR* G, Node* S, double epsilon, bool verbose);

vector<pair<float, Edge>> kBestObject(vector<pair<float, Edge>> objs, int k);


struct PQCollection {
    set<pair<double, EdgeE>, less<pair<double, EdgeE>>> pq;  // Priority queue for objects
};

struct MatchedCollection {
    unordered_set<Edge> matched; // Priority queue for objects
};

class Auction { 
    public:
        Auction(CSR* G, Node* S, double epsilon);
        void bMatching(bool verbose);
        void bFactor(bool verbose);
        vector<Edge> getMatching();
};

#endif //AUCTION_H
