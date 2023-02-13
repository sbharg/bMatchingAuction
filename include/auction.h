#ifndef AUCTION_H
#define AUCTION_H

#include "graph.h"

void bMatchingAuction(CSR* G, Node* S, double epsilon, bool verbose);

void bFactorAuction(CSR* G, Node* S, double epsilon, bool verbose);

#endif //AUCTION_H
