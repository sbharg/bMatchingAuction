#ifndef COMPARISON_H
#define COMPARISON_H

#include "graph.h"
#include <algorithm>
#include <lemon/smart_graph.h>
#include <lemon/network_simplex.h>
#include <lemon/cost_scaling.h>

using namespace lemon;
using namespace std;

AlgResult bFactorComparison_NS(CSR* G, Node* S) {
    double start = omp_get_wtime();

    SmartDigraph g;
    SmartDigraph::NodeMap<int> supply(g);
    SmartDigraph::ArcMap<float> cost(g);
    SmartDigraph::ArcMap<int> capacity(g);

    SmartDigraph::Node nodes[G->nVer];
    map<int, SmartDigraph::Node> nodeIndexMap;

    // Adding vertices
    for (int i = 0; i < G->nVer; i++) {
        nodes[i] = g.addNode();
        nodeIndexMap[i] = nodes[i];
        if (i < G->lVer)
            supply[nodes[i]] = S[i].b;
        else 
            supply[nodes[i]] = -1*S[i].b;
    }

    // Adding edges 
    for (int i = 0; i < G->lVer; i++) {   
        for (int j = G->verPtr[i]; j < G->verPtr[i+1]; j++) {
            if (G->verInd[j].weight >= 0) {
                SmartDigraph::Arc e = g.addArc(nodeIndexMap[i], nodeIndexMap[G->verInd[j].id]);
                cost[e] = -1*G->verInd[j].weight;
                capacity[e] = 1;
            }
        }
    }

    NetworkSimplex<SmartDigraph> network_simplex_solver(g);    
    network_simplex_solver.supplyMap(supply).costMap(cost).upperMap(capacity);
    
    double time_init = omp_get_wtime();

    //SmartDigraph::ArcMap<int> flow(g);

    network_simplex_solver.run();
    double end_ns =  omp_get_wtime();

    return AlgResult(end_ns - start, time_init - start, -1*network_simplex_solver.totalCost<double>());
}

AlgResult bFactorComparison_CS(CSR* G, Node* S) {
    double start = omp_get_wtime();

    SmartDigraph g;
    SmartDigraph::NodeMap<int> supply(g);
    SmartDigraph::ArcMap<float> cost(g);
    SmartDigraph::ArcMap<int> capacity(g);

    SmartDigraph::Node nodes[G->nVer];
    map<int, SmartDigraph::Node> nodeIndexMap;

    // Adding vertices
    for (int i = 0; i < G->nVer; i++) {
        nodes[i] = g.addNode();
        nodeIndexMap[i] = nodes[i];
        if (i < G->lVer)
            supply[nodes[i]] = S[i].b;
        else 
            supply[nodes[i]] = -1*S[i].b;
    }

    // Adding edges 
    for (int i = 0; i < G->lVer; i++) {   
        for (int j = G->verPtr[i]; j < G->verPtr[i+1]; j++) {
            if (G->verInd[j].weight >= 0) {
                SmartDigraph::Arc e = g.addArc(nodeIndexMap[i], nodeIndexMap[G->verInd[j].id]);
                cost[e] = -1*G->verInd[j].weight;
                capacity[e] = 1;
            }
        }
    }

    CostScaling<SmartDigraph> cost_scaling_solver(g);    
    cost_scaling_solver.supplyMap(supply).costMap(cost).upperMap(capacity);
    double time_init = omp_get_wtime();

    //SmartDigraph::ArcMap<int> flow(g);

    cost_scaling_solver.run();
    double end_cs =  omp_get_wtime();

    return AlgResult(end_cs - start, time_init - start, -1*cost_scaling_solver.totalCost<double>());
}

AlgResult bMatchingComparison_NS(CSR* G, Node* S) {
    double start = omp_get_wtime();

    SmartDigraph g;
    SmartDigraph::NodeMap<int> supply(g);
    SmartDigraph::ArcMap<float> cost(g);
    SmartDigraph::ArcMap<int> capacity(g);

    SmartDigraph::Node nodes[G->nVer];
    map<int, SmartDigraph::Node> nodeIndexMap;

    // Adding vertices
    SmartDigraph::Node s = g.addNode();
    supply[s] = 0;
    for (int i = 0; i < G->nVer; i++) {
        nodes[i] = g.addNode();
        nodeIndexMap[i] = nodes[i];
        supply[nodes[i]] = 0;
        if (i < G->lVer) {
            SmartDigraph::Arc e = g.addArc(s, nodes[i]);
            cost[e] = 0;
            capacity[e] = S[i].b;
        }
        else {
            SmartDigraph::Arc e = g.addArc(nodes[i], s);
            cost[e] = 0;
            capacity[e] = S[i].b;
        }
    }

    // Adding edges 
    for (int i = 0; i < G->lVer; i++) {   
        for (int j = G->verPtr[i]; j < G->verPtr[i+1]; j++) {
            if (G->verInd[j].weight >= 0) {
                SmartDigraph::Arc e = g.addArc(nodeIndexMap[i], nodeIndexMap[G->verInd[j].id]);
                cost[e] = -1*G->verInd[j].weight;
                capacity[e] = 1;
            }
        }
    }

    NetworkSimplex<SmartDigraph> network_simplex_solver(g);    
    network_simplex_solver.supplyMap(supply).costMap(cost).upperMap(capacity);
    
    double time_init = omp_get_wtime();

    //SmartDigraph::ArcMap<int> flow(g);

    network_simplex_solver.run();
    double end_ns =  omp_get_wtime();

    return AlgResult(end_ns - start, time_init - start, -1*network_simplex_solver.totalCost<double>());
}

AlgResult bMatchingComparison_CS(CSR* G, Node* S) {
    double start = omp_get_wtime();

    SmartDigraph g;
    SmartDigraph::NodeMap<int> supply(g);
    SmartDigraph::ArcMap<float> cost(g);
    SmartDigraph::ArcMap<int> capacity(g);

    SmartDigraph::Node nodes[G->nVer];
    map<int, SmartDigraph::Node> nodeIndexMap;

    // Adding vertices
    SmartDigraph::Node s = g.addNode();
    supply[s] = 0;
    for (int i = 0; i < G->nVer; i++) {
        nodes[i] = g.addNode();
        nodeIndexMap[i] = nodes[i];
        supply[nodes[i]] = 0;
        if (i < G->lVer) {
            SmartDigraph::Arc e = g.addArc(s, nodes[i]);
            cost[e] = 0;
            capacity[e] = S[i].b;
        }
        else {
            SmartDigraph::Arc e = g.addArc(nodes[i], s);
            cost[e] = 0;
            capacity[e] = S[i].b;
        }
    }

    // Adding edges 
    for (int i = 0; i < G->lVer; i++) {   
        for (int j = G->verPtr[i]; j < G->verPtr[i+1]; j++) {
            if (G->verInd[j].weight >= 0) {
                SmartDigraph::Arc e = g.addArc(nodeIndexMap[i], nodeIndexMap[G->verInd[j].id]);
                cost[e] = -1*G->verInd[j].weight;
                capacity[e] = 1;
            }
        }
    }

    CostScaling<SmartDigraph> cost_scaling_solver(g);    
    cost_scaling_solver.supplyMap(supply).costMap(cost).upperMap(capacity);
    double time_init = omp_get_wtime();

    //SmartDigraph::ArcMap<int> flow(g);

    cost_scaling_solver.run();
    double end_cs =  omp_get_wtime();

    return AlgResult(end_cs - start, time_init - start, -1*cost_scaling_solver.totalCost<double>());
}

AlgResult bMatchingGreedy(CSR* G, Node* S) {
    double start = omp_get_wtime();

    int saturation[G->nVer] = {}; // Array to keep track of number of incident matched edges for each vertex

    // Populate and sort vector of edges by weight in ascending order  
    vector<EdgeE> edges;
    edges.reserve(G->nEdge);
    for (int i = 0; i < G->lVer; i++) {
        for (int j = G->verPtr[i]; j < G->verPtr[i+1]; j++) {
            if (G->verInd[j].weight >= 0) {
                edges.push_back(EdgeE(i, G->verInd[j].id, (double) G->verInd[j].weight));
            }
        }
    }
    sort(edges.begin(), edges.end());

    // Greedily match edges
    double weight = 0;
    while (!edges.empty()) {
        EdgeE e = edges.back();
        int u = e.head;
        int v = e.id;
        // Match edge if it doesn't oversaturate either of its vertices
        if (saturation[u] < S[u].b && saturation[v] < S[v].b) {
            saturation[u]++;
            saturation[v]++;
            weight += e.weight;
        }
        edges.pop_back();
    }

    double end =  omp_get_wtime();
    return AlgResult(end - start, 0, weight);
}

#endif  //COMPARISON_H
