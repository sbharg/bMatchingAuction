#include "include/graph.h"
#include <cstring>
using namespace std;

bool CSR::readMtxB(char* filename, bool abs_value, bool verbose) {
    int count = 0, i, j;
    int inp, m1, sym, edgecnt_;
    int numRow, numCol, nonZeros, numEdges;
    double f;
    string s;
    ifstream inf;

    inf.open(filename, ios::in);

    if (inf.is_open()) {
        size_t found1, found2, found3;
        getline(inf,s);
        found1 = s.find("pattern");
        if (found1 != string::npos) {
            m1 = 2;
        }
        else {
            m1 = 3;
        }
        found1 = s.find("symmetric");
        found2 = s.find("hermitian");
        found3 = s.find("skew-symmetric");
        if (found1 != string::npos || found2 != string::npos || found3 != string::npos) {
            sym = 1;
        }
        else {
            sym = 0;
        }
        while (inf.peek() == '%') {
            getline(inf,s);
        }
        
        if (sym==0) {    
            cout << endl << "WARNING..!!" << endl;
            cout << "The mtx file contains full matrix" << endl;
            cout << "User has to make sure that input file has both (i,j) and (j,i) present." << endl;
            cout << "Matching is not defined if w(i,j) != w(j,i)" << endl << endl;
        }
 
        inf >> inp;
        numRow = inp;
        inf >> inp;
        numCol = inp;
        inf >> inp;
        nonZeros = inp;

        lVer = numRow;
        rVer = numCol;
        nVer = lVer+rVer;

        count = inp;
        
        vector<vector<int>> graphCRSIdx(nVer);
        vector<vector<double>> graphCRSVal(nVer);
        int diag = 0;
        
        while (count > 0) {     
            inf >> i; 
            inf >> j;

            j += lVer; //adjusting for the right hand vertices
            
            if (m1 == 3) {
                inf >> f; 
            }
            else {
                f = drand48()*1000000;
            }

            if (i == j) {
                diag++;
            }
            else {
                graphCRSIdx[i-1].push_back(j-1); 
                graphCRSVal[i-1].push_back(f);
                if (sym) {
                    graphCRSIdx[j-1].push_back(i-1); 
                    graphCRSVal[j-1].push_back(f); 
                }
            }   
            count--; 
        }     
        inf.close(); 
     
        numEdges = nonZeros;  
        if(sym == 1) //symmetric matrix
            numEdges = nonZeros*2 - 2*diag;

        nEdge = numEdges;
        
        verPtr = new int[nVer+1];
        verInd = new Edge[nEdge];

        verPtr[0] = 0;
        int max = 0, offset; 
        for (int i = 1; i <= nVer; i++) {
            offset = graphCRSIdx[i-1].size();
            verPtr[i] = verPtr[i-1] + offset;
            count = verPtr[i-1];
            for (int j = 0; j < offset; j++) {
                verInd[count].id = graphCRSIdx[i-1][j];
                if (abs_value) {
                    verInd[count].weight = abs(graphCRSVal[i-1][j]);
                }
                else {
                    verInd[count].weight = graphCRSVal[i-1][j];
                }
                count++;
            }
            if (offset>max)
                max = offset;
        }
        
        assert(count == nEdge);
        maxDeg = max;

        if (verbose)
            cout << "(|A|, |B|, n, m) := (" << lVer << ", " << rVer << ", " << nVer << ", " << nEdge << ")" << endl << endl;
    }
    else return false;

    int flag = 0;
    for (int i = 0; i < lVer; i++) {    
        for (j = verPtr[i]; j < verPtr[i+1]; j++) {
            if (verInd[j].id < lVer) {    
                flag = 1;
                break;
            }
        }
        if (flag == 1) {            
            cout << "Bad Graph left: " << i << endl;
            break;
        }
    }

    flag = 0;
    for (int i = lVer; i < nVer; i++) {    
        for (j = verPtr[i]; j< verPtr[i+1]; j++) {
            if (verInd[j].id >= lVer) {    
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            cout << "Bad Graph Right: " << i << endl;
            break;
        }
    }
   
   return true;
}