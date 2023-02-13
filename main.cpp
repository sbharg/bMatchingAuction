#include "include/graph.h"
#include "include/auction.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <float.h>
#include <ctime>
#include <cmath>
#include <algorithm> 
#include <random>

using namespace std;

struct auction_parameters {
    char* problem_name;
    bool verbose;
    bool abs_value;
    double epsilon;
    int algorithm; // 1 for b-matching auction, 0 for b-factor auction

    auction_parameters();
    void usage();
    bool parse(int argc, char** argv);
};

auction_parameters::auction_parameters():problem_name(NULL),algorithm(1),abs_value(false),verbose(false),epsilon(0.5){}

void auction_parameters::usage() {
    const char *params =
	"\n"
    "Usage: %s -f <problem_name> [-e <value>] [-p] [-a] [-v]\n\n"
	"   -f --filename problem_name  : File containing graph. Currently inputs .mtx files\n"
    "   -e --epsilon  value         : Value for epsilon. Default is e=0.5\n"
    "   -p --perfect                : Use the perfect b-matching (b-factor) algorithm\n"
    "   -a --absvalue               : Take the absolute value of edge weights\n"
    "   -v --verbose                : Verbose \n\n"
    "By default this runs the b-matching auction algorithm. Use -p to run the b-factor auction algorihtm.\n\n";
    fprintf(stderr, params);
}

bool auction_parameters::parse(int argc, char** argv) {
    static struct option long_options[]={
        // These options don't take extra arguments
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {"absvalue", no_argument, NULL, 'a'},
        {"perfect", no_argument, NULL, 'p'},
        
        // These do
        {"filename", required_argument, NULL, 'f'},
        {"epsilon", required_argument, NULL, 'e'},

        {NULL, no_argument, NULL, 0}
    };

    static const char *opt_string = "vhapf:e:";
    int opt, longindex;
    opt = getopt_long(argc,argv,opt_string,long_options,&longindex);
    while (opt != -1) {
        switch (opt) {
            case 'v':   verbose = true; 
                        break;

            case 'h':   usage(); 
                        return false; 
                        break;

            case 'a':   abs_value = false;
                        break;

            case 'p':   algorithm = 0;
                        break;

            case 'f':   problem_name = optarg; 
                        cout << "Problem file: " << problem_name << endl;
                        if (problem_name == NULL || problem_name[0] == '\0' || *problem_name == 0) {
                            cerr << "Error: Problem file is not speficied" << endl;
                            return false;  
                        }
                        break;

            case 'e':   epsilon = atof(optarg);
                        if (epsilon < 0) {
                            cerr << "Error: epsilon can't be negative" << endl;
                            return false;
                        }
                        break;
        }
        opt = getopt_long(argc,argv,opt_string,long_options,&longindex);
    }
    return true;
}

int main(int argc, char** argv){
    srand(time(0));
    auction_parameters opts;
    if (!opts.parse(argc,argv)) {
        return -1;
    }
    
    // Reading the input 
    double rt_start = omp_get_wtime();	
    CSR G;
    G.readMtxB(opts.problem_name, opts.abs_value, opts.verbose);
    
    // Memory Allocation
    Node* S = new Node[G.nVer];      
    cout << "Input Processing Done: " << omp_get_wtime() - rt_start << endl << endl;	

    // Randomly assign b-values based on algorithm and run
    if (opts.algorithm == 1){
        // b-matching auction algorithm
        if (opts.verbose)
            cout << "Randomly generating b-values" << endl;

        for (int i = 0; i < G.nVer; i++) {
            int deg = 0;
            for (int j = G.verPtr[i]; j < G.verPtr[i+1]; j++) {
                if (G.verInd[j].weight >= 0) {
                    deg++;
                }
            }
            S[i].deg = deg;
            S[i].b = rand() % deg + 1; // b-value in range [1, deg]
        }
        /*
        for (int i = 0; i < G.nVer; i++) {
            cout << i << ": Degree is " << S[i].deg << ", b-value is " << S[i].b << endl;
        }
        */
        bMatchingAuction(&G, S, opts.epsilon, opts.verbose);
    }
    else {
        // b-factor auction algorithm
        if (opts.verbose)
            cout << "Randomly generating b-values" << endl;

        // Randomly generate a permutation of indices for the right side of the graph
        vector<int> random_idx_perm;
        for (int i = G.lVer; i < G.nVer; ++i) {
            random_idx_perm.push_back(i);
        }
        shuffle(random_idx_perm.begin(), random_idx_perm.end(), default_random_engine(time(0)));

        for (int i = 0; i < G.nVer; i++) {
            int deg = 0;
            for (int j = G.verPtr[i]; j < G.verPtr[i+1]; j++) {
                if (G.verInd[j].weight >= 0) {
                    deg++;
                }
            }
            S[i].deg = deg;

            if (i < G.lVer) {
                int deg_half = floor(deg/2);
                int random_idx = random_idx_perm.back();
                random_idx_perm.pop_back();
                S[i].b = S[random_idx].b = rand() % deg_half + 1; // b-value in range [1, floor(deg/2)]
            }

        }
        /*
        for (int i = 0; i < G.nVer; i++) {
            cout << i << ": Degree is " << S[i].deg << ", b-value is " << S[i].b << endl;
        }
        */
        bFactorAuction(&G, S, opts.epsilon, opts.verbose);
    }
    
    /*
	if(opts.verbose)
	{
		ofstream fout;
		fout.open("matching.txt",ios::out);
		if(fout.is_open())
		{
			fout<<"*Vertices "<<G.nVer<<endl;
			fout<<"*arcs"<<endl;
			for(int i=0;i<G.nVer;i++)
				for(int j=0;j<S[i].curSize;j++)
					if(i>S[i].heap[j].id)
						fout<<i+1<<" "<<S[i].heap[j].id+1<<" "<<S[i].heap[j].weight<<endl;
		}
		fout.close();
	}
    for(int i=0;i<G.nVer;i++)
        delete S[i].heap;
    
    delete S;
    
    return 0;
    */
}