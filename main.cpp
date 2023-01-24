#include "include/graph.h"
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <float.h>

using namespace std;

struct auction_parameters{
    char* problemname;
    bool verbose;
    double epsilon;

    auction_parameters();
    void usage();
    bool parse(int argc, char** argv);
};

auction_parameters::auction_parameters():problemname(NULL),verbose(false),epsilon(0.5){}

void auction_parameters::usage(){
    const char *params =
	"\n"
    "Usage: %s -f <problemname> [-e] [-v]\n\n"
	"   -f problemname  : file containing graph. Currently inputs .mtx files\n"
    "   -e epsilon      : value for epsilon. Default is e=0.5\n"
    "   -v              : verbose \n\n";
    fprintf(stderr, params);
}

bool auction_parameters::parse(int argc, char** argv){
    static struct option long_options[]={
        // These options don't take extra arguments
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        
        // These do
        {"problem", required_argument, NULL, 'f'},
        {"epsilon", required_argument, NULL, 'e'},
        {NULL, no_argument, NULL, 0}
    };

    static const char *opt_string="vhf:e:";
    int opt, longindex;
    opt = getopt_long(argc,argv,opt_string,long_options,&longindex);
    while(opt != -1){
        switch(opt){
            case 'v': verbose=true; 
                    break;

            case 'h': usage(); 
                    return false; 
                    break;

            case 'f': problemname = optarg; 
                    if(problemname == NULL){
                        cerr<<"Error: Problem file is not speficied"<<endl;
                        return false;  
                    }
                    break;
            case 'e': epsilon = atof(optarg);
                    if(epsilon < 0){
                        cerr<<"Error: epsilon can't be negative"<<endl;
                        return false;
                    }
                    break;
        }
        opt = getopt_long(argc,argv,opt_string,long_options,&longindex);
    }
    return true;
}

int main(int argc, char** argv){
    auction_parameters opts;
    if(!opts.parse(argc,argv)){
        return -1;
    }
    
    /********* Reading the input ******/
    double rt_start = omp_get_wtime();	
    CSR G;
    G.readMtxB(opts.problemname);

    double rt_end = omp_get_wtime();	
    cout<<"Graph (" << G.nVer << ", " << G.nEdge/2 << ") Reading Done....!! took " << rt_end - rt_start <<endl;
    
    /*********** Memory Allocation *************/
    //int *b = new int[G.nVer];
    //Node* S = new Node[G.nVer];      
    cout << "Input Processing Done: " << omp_get_wtime() - rt_end <<endl;	
    
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