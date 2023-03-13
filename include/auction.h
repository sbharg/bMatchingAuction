#ifndef AUCTION_H
#define AUCTION_H

#include "graph.h"
#include "adjust_pq.h"
#include <set>
#include <utility>
#include <unordered_set>
#include <set>
#include <unordered_map>


void bMatchingAuction(CSR* G, Node* S, double epsilon, bool verbose);

void bFactorAuction(CSR* G, Node* S, double epsilon, bool verbose);

vector<pair<float, Edge>> kBestObject(vector<pair<float, Edge>>& objs, int k);

class Auction { 
    public:
        Auction(CSR* G, Node* S, double epsilon);
        void bMatching(bool verbose);
        void bFactor(bool verbose);
        vector<Edge> getMatching();
};

struct ObjectCopy {
    ObjectCopy(float price, int object_id)
        : price(price), object_id(object_id) {}

    float price;
    int object_id;
    Edge matched = {-1, 0.0};
    int heap_index = -1;

    bool operator>(const ObjectCopy& other) const {
        return (price > other.price);
    }

    void SetHeapIndex(int index) {
        heap_index = index;
    }

    int GetHeapIndex() const {
        return heap_index;
    }
};

struct Bidder {
    unordered_map<int, ObjectCopy*> matched;
};

struct Object {
    AdjustablePriorityQueue<ObjectCopy, greater<ObjectCopy>> pq;
};

#endif  //AUCTION_H
