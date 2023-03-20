#include "include/auction.h"
#include <iostream>
#include <deque>
#include <queue>
#include <algorithm> 
#include <random>

AlgResult bMatchingAuction(CSR* G, Node* S, double epsilon, bool verbose) {
    if (verbose) 
        cout << "Running b-Matching Auction" << endl;

    double start = omp_get_wtime();

    // Initialize the auxilliary data structures
    Bidder* A = new Bidder[G->lVer];    // Array of bidders
    Object* B = new Object[G->rVer];    // Array of objects

    for (int i = G->rVer; i < G->nVer; i++) {
        B[i - G->lVer].object_copies.reserve(S[i].b);
        B[i - G->lVer].pq.SetCapacity(S[i].b);
        for (int j = 0; j < S[i].b; j++) {
            B[i - G->lVer].object_copies.push_back(ObjectCopy(0.0, i));
            B[i - G->lVer].pq.Add(&B[i - G->lVer].object_copies.back());   // Min adjustable priority queue of b(i) object copies
        }
        if (verbose) {
            cout << "B[" << i << "].pq: ";
            vector<ObjectCopy*> v = *B[i - G->lVer].pq.Raw();
            for (auto it = v.begin(); it != v.end(); it++) {
                cout << (*it)->price << ", " << (*it)->heap_index << " | ";
            }
            cout << endl;
        }
    }
    
    deque<int> I;   //Unsaturated bidders
    for (int i = 0; i < G->lVer; i++) {
        I.push_back(i);
    }
    //shuffle(I.begin(), I.end(), default_random_engine(time(0)));

    double time_init = omp_get_wtime();

    while(!I.empty()){
        int bidder = I.front();

        if (!A[bidder].is_strongly_eps_happy) {

            vector<pair<float, Edge>> objs_to_look_at;
            for (int i = G->verPtr[bidder]; i < G->verPtr[bidder+1]; i++) {
                //if (G->verInd[i].weight >= 0 && !A[bidder].matched.contains(G->verInd[i].id)) {
                if (G->verInd[i].weight >= 0 && A[bidder].matched.find(G->verInd[i].id) == A[bidder].matched.end()) {
                    float value = G->verInd[i].weight - B[G->verInd[i].id - G->lVer].pq.Top()->price;
                    if (value >= epsilon) {
                        objs_to_look_at.push_back(make_pair(value, G->verInd[i]));
                    }
                }
            }

            pair<float, Edge> comparison_obj;
            vector<pair<float, Edge>> best_objs = kBestObject(objs_to_look_at, S[bidder].b + 1 - A[bidder].matched.size());
            if (best_objs.size() < S[bidder].b + 1 - A[bidder].matched.size()) {
                comparison_obj = make_pair(epsilon, Edge(-1, 0));
                A[bidder].permanent = true;
            }
            else {
                comparison_obj = best_objs.back();
                best_objs.pop_back();
            }

            // print the elements of objs_to_look_at
            if (verbose) {
                cout << "Bidder " << bidder << " (b: " << S[bidder].b << ") " << "is matched to: (";
                for (const auto& [key, value] : A[bidder].matched ) {
                    cout << key << ", ";
                }
                cout << ") " << endl;
                cout << "Bidder " << bidder << " is looking at objects: ";
                for (auto& obj : best_objs) {
                    cout << "(" << obj.second.id << ") ";
                }
                cout << "Comparison object: (" << comparison_obj.second.id << ")";
                cout << endl << endl;
            }

            for(const auto& [j, c] : A[bidder].matched) {
                float bid = c->matched.weight - c->price - comparison_obj.first + epsilon;
                c->price += bid;
                B[c->object_id - G->lVer].pq.NoteChangedPriority(c);
            } 

            for (auto& obj : best_objs) {
                Edge e = obj.second;
                int obj_id = e.id;
                float bid = obj.first - comparison_obj.first + epsilon;

                ObjectCopy* c = B[obj_id - G->lVer].pq.Top();
                int old_bidder = c->matched.id;
                c->price += bid;
                c->matched = {bidder, e.weight};
                B[c->object_id - G->lVer].pq.NoteChangedPriority(c);
                A[bidder].matched.insert({obj_id, c});

                // Remove matched edge from old bidder
                if (old_bidder >= 0) {
                    A[old_bidder].matched.erase(obj_id);
                    if (!A[old_bidder].permanent) {
                        A[old_bidder].is_strongly_eps_happy = false;
                        I.push_back(old_bidder);
                    }
                }
            }

            A[bidder].is_strongly_eps_happy = true;
        }
        I.pop_front();
    }

    double end =  omp_get_wtime();

    double weight;
    for (int i = G->rVer; i < G->nVer; i++) {
        for (int j = 0; j < S[i].b; j++) {
            weight += B[i - G->lVer].object_copies[j].matched.weight;
        }
    }

    delete [] A;
    delete [] B;
    return AlgResult(end - start, time_init - start, weight);
}

AlgResult bFactorAuction(CSR* G, Node* S, double epsilon, bool verbose) {
    if (verbose) 
        cout << "Running b-Factor Auction" << endl;

    double start = omp_get_wtime();

    // Initialize the auxilliary data structures
    Bidder* A = new Bidder[G->lVer];    // Array of bidders
    Object* B = new Object[G->rVer];    // Array of objects

    //vector<Bidder> A;
    //vector<Object> B;
    //A.reserve(G->lVer);
    //B.reserve(G->rVer);

    for (int i = G->rVer; i < G->nVer; i++) {
        B[i - G->lVer].object_copies.reserve(S[i].b);
        B[i - G->lVer].pq.SetCapacity(S[i].b);
        for (int j = 0; j < S[i].b; j++) {
            B[i - G->lVer].object_copies.push_back(ObjectCopy(0.0, i));
            B[i - G->lVer].pq.Add(&B[i - G->lVer].object_copies.back());   // Min adjustable priority queue of b(i) object copies
        }
        if (verbose) {
            cout << "B[" << i << "].pq: ";
            vector<ObjectCopy*> v = *B[i - G->lVer].pq.Raw();
            for (auto it = v.begin(); it != v.end(); it++) {
                cout << (*it)->price << ", " << (*it)->heap_index << " | ";
            }
            cout << endl;
        }
    }
    
    deque<int> I;   //Unsaturated bidders
    for (int i = 0; i < G->lVer; i++) {
        I.push_back(i);
    }
    //shuffle(I.begin(), I.end(), default_random_engine(time(0)));

    double time_init = omp_get_wtime();

    while(!I.empty()){
        int bidder = I.front();
        
        if (!A[bidder].is_strongly_eps_happy) {

            vector<pair<float, Edge>> objs_to_look_at;
            for (int i = G->verPtr[bidder]; i < G->verPtr[bidder+1]; i++) {
                //if (G->verInd[i].weight >= 0 && !A[bidder].matched.contains(G->verInd[i].id)) {
                if (G->verInd[i].weight >= 0 && A[bidder].matched.find(G->verInd[i].id) == A[bidder].matched.end()) {
                    float value = G->verInd[i].weight - B[G->verInd[i].id - G->lVer].pq.Top()->price;
                    objs_to_look_at.push_back(make_pair(value, G->verInd[i]));
                }
            }

            // Get the k largest elements from objs_to_look_at
            vector<pair<float, Edge>> best_objs = kBestObject(objs_to_look_at, S[bidder].b + 1 - A[bidder].matched.size());
            pair<float, Edge> comparison_obj = best_objs.back();
            best_objs.pop_back();

            // print the elements of objs_to_look_at
            if (verbose) {
                cout << "Bidder " << bidder << " (b: " << S[bidder].b << ") " << "is matched to: (";
                for (const auto& [key, value] : A[bidder].matched ) {
                    cout << key << ", ";
                }
                cout << ") " << endl;
                cout << "Bidder " << bidder << " is looking at objects: ";
                for (auto& obj : best_objs) {
                    cout << "(" << obj.second.id << ") ";
                }
                cout << "Comparison object: (" << comparison_obj.second.id << ")";
                cout << endl << endl;
            }

            for(const auto& [j, c] : A[bidder].matched) {
                float bid = c->matched.weight - c->price - comparison_obj.first + epsilon;
                c->price += bid;
                B[c->object_id - G->lVer].pq.NoteChangedPriority(c);
            } 

            for (auto& obj : best_objs) {
                Edge e = obj.second;
                int obj_id = e.id;
                float bid = obj.first - comparison_obj.first + epsilon;

                ObjectCopy* c = B[obj_id - G->lVer].pq.Top();
                int old_bidder = c->matched.id;
                c->price += bid;
                c->matched = {bidder, e.weight};
                B[c->object_id - G->lVer].pq.NoteChangedPriority(c);
                A[bidder].matched.insert({obj_id, c});

                // Remove matched edge from old bidder
                if (old_bidder >= 0) {
                    A[old_bidder].matched.erase(obj_id);
                    A[old_bidder].is_strongly_eps_happy = false;
                    I.push_back(old_bidder);
                }
            }

            A[bidder].is_strongly_eps_happy = true;
        }
        I.pop_front();
    }

    double end =  omp_get_wtime();

    double weight;
    for (int i = G->rVer; i < G->nVer; i++) {
        for (int j = 0; j < S[i].b; j++) {
            weight += B[i - G->lVer].object_copies[j].matched.weight;
        }
    }

    /*
    cout << "\e[1mAuction (ε = " << epsilon << ")\e[0m" << endl;
    cout << "Weight: " << weight << endl;
    cout << "Initialization Time: " << time_init - start << endl;
    cout << "Running Time: " << end - start << endl << endl;
    */

    delete [] A;
    delete [] B;
    return AlgResult(end - start, time_init - start, weight);
}

// Function to get k best objects in a given array
vector<pair<float, Edge>> kBestObject(vector<pair<float, Edge>>& objs, int k) {
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
    int size = pq.size();
    vector<pair<float, Edge>> best_objs(size);
    for (int i = 0; i < size; i++) {
        best_objs[size-i-1] = pq.top();
        pq.pop();
    }
    return best_objs;
}