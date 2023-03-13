#ifndef COMPARISON_H
#define COMPARISON_H

#include "graph.h"
#include <lemon/smart_graph.h>
#include <lemon/network_simplex.h>
#include <lemon/cost_scaling.h>

void bFactorComparison(CSR* G, Node* S, bool verbose);

void bFactorComparison_cs(CSR* G, Node* S, bool verbose);


#endif  //COMPARISON_H
