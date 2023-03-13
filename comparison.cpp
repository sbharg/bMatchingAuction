#include "include/comparison.h"

using namespace lemon;
using namespace std;

void bFactorComparison(CSR* G, Node* S, bool verbose) {
    double start = omp_get_wtime();

    SmartDigraph g;
    SmartDigraph::NodeMap<int> supply(g);
    SmartDigraph::ArcMap<long> cost(g);
    SmartDigraph::ArcMap<int> capacity(g);

    for (int i = 0; i < G->lVer; i++) {        
        SmartDigraph::Node u = g.addNode();
        supply[u] = S[i].b;
        for (int j = G->verPtr[i]; j < G->verPtr[i+1]; j++) {
            if (G->verInd[j].weight >= 0) {
                SmartDigraph::Node v = g.addNode();
                supply[v] = -1*S[i].b;

                SmartDigraph::Arc e = g.addArc(u, v);
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

    cout << "\e[1mNetwork Simplex\e[0m" << endl;

    cout << "Total Cost: " << -1*network_simplex_solver.totalCost() << endl;
    cout << "Time: " << end_ns - start << endl;

    cout << "Initialization Time: " << time_init - start << endl << endl;
}

void bFactorComparison_cs(CSR* G, Node* S, bool verbose) {
    double start = omp_get_wtime();

    SmartDigraph g;
    SmartDigraph::NodeMap<int> supply(g);
    SmartDigraph::ArcMap<float> cost(g);
    SmartDigraph::ArcMap<int> capacity(g);

    for (int i = 0; i < G->lVer; i++) {        
        SmartDigraph::Node u = g.addNode();
        supply[u] = S[i].b;
        for (int j = G->verPtr[i]; j < G->verPtr[i+1]; j++) {
            if (G->verInd[j].weight >= 0) {
                SmartDigraph::Node v = g.addNode();
                supply[v] = -1*S[i].b;

                SmartDigraph::Arc e = g.addArc(u, v);
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

    cout << "\e[1mPush-Relabel\e[0m" << endl;

    cout << "Total Cost: " << -1*cost_scaling_solver.totalCost() << endl;
    cout << "Time: " << end_cs - start << endl;

    cout << "Initialization Time: " << time_init - start << endl;
}