#include "include/graph.h"
#include "include/auction.h"
#include "include/comparison.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <float.h>
#include <ctime>
#include <cmath>
#include <algorithm> 
#include <random>
#include <limits>

typedef std::numeric_limits< double > dbl;

using namespace std;

struct auction_parameters {
    char* problem_name;
    bool verbose;
    bool abs_value;
    bool compare;
    double epsilon;
    int algorithm; // 1 for b-matching auction, 0 for b-factor auction

    auction_parameters();
    void usage();
    bool parse(int argc, char** argv);
};

auction_parameters::auction_parameters():problem_name(NULL),algorithm(1),abs_value(false),verbose(false),compare(false),epsilon(0.5){}

void auction_parameters::usage() {
    const char *params =
	"\n"
    "Usage: %s -f <problem_name> [-e <value>] [-p] [-a] [-v]\n\n"
	"   -f --filename problem_name  : File containing graph. Currently inputs .mtx files\n"
    "   -e --epsilon  value         : Value for epsilon. Default is ε=0.5\n"
    "   -p --perfect                : Use the perfect b-matching (b-factor) algorithm\n"
    "   -c --compare                : Perform a comparion against other algorithms\n"
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
        {"compare", no_argument, NULL, 'c'},
        {"perfect", no_argument, NULL, 'p'},
        
        // These do
        {"filename", required_argument, NULL, 'f'},
        {"epsilon", required_argument, NULL, 'e'},

        {NULL, no_argument, NULL, 0}
    };

    static const char *opt_string = "vhacpf:e:";
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

            case 'c':   compare = true;
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
    cout.precision(dbl::max_digits10);

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
    cout << "Input Processing Done: " << omp_get_wtime() - rt_start << endl;	
    cout << "(|A|, |B|, n, m) := (" << G.lVer << ", " << G.rVer << ", " << G.nVer << ", " << G.nEdge/2 << ")" << endl << endl;

    // Randomly assign b-values based on algorithm and run
    if (opts.algorithm == 1){
        // b-matching auction algorithm
        if (opts.verbose)
            cout << "Randomly generating b-values" << endl;

        // Assignment of b-values
        for (int i = 0; i < G.nVer; i++) {
            int deg = 0;
            for (int j = G.verPtr[i]; j < G.verPtr[i+1]; j++) {
                if (G.verInd[j].weight >= 0) {
                    deg++;
                }
            }
            S[i].deg = deg;

            // Taken from: https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
            std::random_device rd;
            std::mt19937 gen(rd()); 
            std::uniform_int_distribution<> distr(1, 10); // random integer in [1, deg]
            S[i].b = distr(gen);
        }
        /*
        for (int i = 0; i < G.nVer; i++) {
            cout << i << ": Degree is " << S[i].deg << ", b-value is " << S[i].b << endl;
        }
        */
        AlgResult auc_res = bMatchingAuction(&G, S, opts.epsilon, opts.verbose);

        cout << "\e[1mAuction (ε = " << opts.epsilon << ")\e[0m" << endl;
        cout << "Total Weight: " << auc_res.weight << endl;
        cout << "Initialization Time: " << auc_res.init_time << endl;
        cout << "Running Time: " << auc_res.total_time << endl << endl;

        if (opts.compare) {
            AlgResult greedy_res = bMatchingGreedy(&G, S);
            AlgResult ns_res = bMatchingComparison_NS(&G, S);
            AlgResult cs_res = bMatchingComparison_CS(&G, S);

            cout << "\e[1mGreedy\e[0m" << endl;
            cout << "Total Weight: " << greedy_res.weight << endl;
            cout << "Running Time: " << greedy_res.total_time << endl << endl;

            cout << "\e[1mNetwork Simplex\e[0m" << endl;
            cout << "Total Weight: " << ns_res.weight << endl;
            cout << "Total Time: " << ns_res.total_time << endl;
            cout << "Conversion Time: " << ns_res.init_time << endl;
            cout << "Running Time: " << ns_res.total_time - ns_res.init_time << endl << endl;

            cout << "\e[1mPush-Relabel\e[0m" << endl;
            cout << "Total Weight: " << cs_res.weight << endl;
            cout << "Total Time: " << cs_res.total_time  << endl;
            cout << "Conversion Time: " << cs_res.init_time << endl;
            cout << "Running Time: " << cs_res.total_time - cs_res.init_time << endl << endl;
        }
    }
    else {
        // b-factor auction algorithm
        if (opts.verbose)
            cout << "Randomly generating b-values" << endl;

        // Randomly generate a permutation of indices for the right side of the graph
        vector<int> random_idx_perm;
        random_idx_perm.reserve(G.rVer);
        for (int i = G.lVer; i < G.nVer; i++) {
            random_idx_perm.push_back(i);
        }
        shuffle(random_idx_perm.begin(), random_idx_perm.end(), default_random_engine(time(0)));

        // Assignment of b-values
        int cardF = 0;
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
                
                // Taken from: https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
                std::random_device rd; 
                std::mt19937 gen(rd()); 
                std::uniform_int_distribution<> distr(1, deg_half); // random integer in [1, deg/2]
                int b = distr(gen);
                cardF += b;
                S[i].b = S[random_idx].b = b;
            }
        }
        /*
        for (int i = 0; i < G.nVer; i++) {
            cout << i << ": Degree is " << S[i].deg << ", b-value is " << S[i].b << endl;
        }
        */
        cout << "Cardinality of F: " << cardF << endl << endl;
        float eps = 10000/cardF;

        AlgResult auc_res = bFactorAuction(&G, S, opts.epsilon, opts.verbose);
        cout << "\e[1mAuction (ε = " << opts.epsilon << ")\e[0m" << endl;
        cout << "Total Weight: " << auc_res.weight << endl;
        cout << "Initialization Time: " << auc_res.init_time << endl;
        cout << "Running Time: " << auc_res.total_time << endl << endl;

        if (opts.compare){
            AlgResult ns_res = bFactorComparison_NS(&G, S);
            AlgResult cs_res = bFactorComparison_CS(&G, S);

            cout << "\e[1mNetwork Simplex\e[0m" << endl;
            cout << "Total Weight: " << ns_res.weight << endl;
            cout << "Total Time: " << ns_res.total_time << endl;
            cout << "Conversion Time: " << ns_res.init_time << endl;
            cout << "Running Time: " << ns_res.total_time - ns_res.init_time << endl << endl;

            cout << "\e[1mPush-Relabel\e[0m" << endl;
            cout << "Total Weight: " << cs_res.weight << endl;
            cout << "Total Time: " << cs_res.total_time  << endl;
            cout << "Conversion Time: " << cs_res.init_time << endl;
            cout << "Running Time: " << cs_res.total_time - cs_res.init_time << endl << endl;
        }
    }
    
    delete[] S;
    
    return 0;
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