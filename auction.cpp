#include "include/auction.h"
#include <iostream>
#include <deque>
#include <queue>
#include <algorithm> 
#include <random>

void bMatchingAuction(CSR* G, Node* S, double epsilon, bool verbose) {
    if (verbose) 
        cout << "Running b-Matching Auction" << endl;
}

void bFactorAuction(CSR* G, Node* S, double epsilon, bool verbose) {
    if (verbose) 
        cout << "Running b-Factor Auction" << endl;

    double start =  omp_get_wtime();

    // Initialize the auxilliary data structures
    PQCollection* P = new PQCollection[G->rVer];
    for (int i = G->rVer; i < G->nVer; i++) {
        for (int j = 0; j < S[i].b; j++) {
            EdgeE e = {-1-j, i, 0.0};
            P[i - G->lVer].pq.insert(make_pair(0.0, e));     // Min priority queue for objects with b(i) elements
        }
        if (verbose) {
            cout << "P[" << i << "].pq: ";
            for (auto it = P[i - G->lVer].pq.begin(); it != P[i - G->lVer].pq.end(); it++) {
                cout << it->first << " " << it->second.head << ", ";
            }
            cout << endl;
        }
    }
    
    MatchedCollection* M = new MatchedCollection[G->lVer];

    deque<int> random_idx_perm;
    for (int i = 0; i < G->lVer; i++) {
        random_idx_perm.push_back(i);
    }
    shuffle(random_idx_perm.begin(), random_idx_perm.end(), default_random_engine(time(0)));
    set<int> unsaturated_bidders(random_idx_perm.begin(), random_idx_perm.end());    // I

    double time_init =  omp_get_wtime();

    while(!unsaturated_bidders.empty()){
        int bidder = *(unsaturated_bidders.begin());
        
        vector<pair<float, Edge>> objs_to_look_at;
        for (int i = G->verPtr[bidder]; i < G->verPtr[bidder+1]; i++) {
            if (G->verInd[i].weight >= 0 && !M[bidder].matched.contains(G->verInd[i])) {
                float value = G->verInd[i].weight - P[G->verInd[i].id - G->lVer].pq.begin()->first;
                objs_to_look_at.push_back(make_pair(value, G->verInd[i]));
            }
        }
        
        // Get the k largest elements from objs_to_look_at
        vector<pair<float, Edge>> best_objs = kBestObject(objs_to_look_at, S[bidder].b + 1 - M[bidder].matched.size());
        pair<float, Edge> comparison_obj = best_objs.back();
        best_objs.pop_back();

        // print the elements of objs_to_look_at
        if (verbose) {
            cout << "Bidder " << bidder << " (b: " << S[bidder].b << ") " << "is matched to: (";
            for (auto& obj : M[bidder].matched) {
                cout << obj.id << ", ";
            }
            cout << ") " << endl;
            cout << "Bidder " << bidder << " is looking at objects: ";
            for (auto& obj : best_objs) {
                cout << "(" << obj.second.id << ") ";
            }
            cout << "Comparison object: (" << comparison_obj.second.id << ")";
            cout << endl << endl;
        }
        
        for (auto& edge : M[bidder].matched) {
            int obj_id = edge.id;
            float old_price;
            EdgeE e;

            auto it = find_if(P[obj_id - G->lVer].pq.begin(), P[obj_id - G->lVer].pq.end(), [bidder](const pair<float, EdgeE>& p ){ return p.second.head == bidder; });
            if (it != P[obj_id - G->lVer].pq.end())
                old_price = it->first;
                e = it->second;
                P[obj_id - G->lVer].pq.erase(it);

            float bid = edge.weight - old_price - comparison_obj.first + epsilon;
            P[obj_id - G->lVer].pq.insert(make_pair(old_price + bid, e));
        }

        for (auto& obj : best_objs) {
            Edge e = obj.second;
            int obj_id = e.id;
            float bid = obj.first - comparison_obj.first + epsilon;

            auto it = P[obj_id - G->lVer].pq.begin();
            EdgeE e_old = it->second;
            float old_price = it->first;
            int old_bidder = e_old.head;
            P[obj_id - G->lVer].pq.erase(it);

            EdgeE e_new = {bidder, obj_id, e.weight};
            P[obj_id - G->lVer].pq.insert(make_pair(old_price + bid, e_new));

            // Remove matched edge from old bidder
            if (old_bidder >= 0) {
                Edge e_old_edge = {e_old.id, e_old.weight};
                if (auto search = M[old_bidder].matched.find(e_old_edge); search != M[old_bidder].matched.end()) {
                    M[old_bidder].matched.erase(search);
                    unsaturated_bidders.insert(old_bidder);
                }
            }

            M[bidder].matched.insert(e);
        }   

        unsaturated_bidders.erase(bidder);
    }

    double end =  omp_get_wtime();
    cout << "Time: " << end - start << endl;
    cout << "Initialization Time: " << time_init - start << endl;
}


// Function to get k best objects in a given array
vector<pair<float, Edge>> kBestObject(vector<pair<float, Edge>> objs, int k) {
    priority_queue<pair<float, Edge>, vector<pair<float, Edge>>, greater<pair<float, Edge>>> pq;
    for (auto & obj : objs) {
        if (pq.size() < k) {
            pq.push(obj);
        }
        else if (pq.top().first < obj.first) {
            pq.pop();
            pq.push(obj);
        }
    }
    vector<pair<float, Edge>> best_objs(k);
    for (int i = 0; i < k; i++) {
        best_objs[k-i-1] = pq.top();
        pq.pop();
    }
    return best_objs;
}


void bFactorAuction2(CSR* G, Node* S, double epsilon, bool verbose) {
}