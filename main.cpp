#include "args.hxx"
#include <iostream>
#include "Util.h"
#include <string.h>
#include "Corepruning.h"
#include "Zigzag.h"
#include "MBKBPSearch.h"

#define SIZE 100  


#define FILELEN 1024

using namespace std;


int main(int argc, char** argv) {

    // defalt value for params
    int delta = 1; 
    int k = 1;
    char filepath[1024] = "./";

	args::ArgumentParser parser(
        "MBKBP, an algorithm for finding the maximum δ-balanced k-biplex\n");

    args::HelpFlag help(parser, "help", "Display this help menu",
                        {'h', "help"});
    args::Group required(parser, "", args::Group::Validators::All);

    args::ValueFlag<string> benchmark_file(parser, "benchmark", "Path to benchmark", {'f', "file"},"");
    args::ValueFlag<int> Delta(parser, "para delta", "The parameter delta", {'d', "d"}, 1);
	args::ValueFlag<int> K(parser, "para k", "The parameter k", {'k', "k"}, 1);

	args::ValueFlag<int> Threshold_l(parser, "Lower Bound for L", "The lower bound of the left size of k-biplex", {'l', "l"}, 1);
    args::ValueFlag<int> Threshold_r(parser, "Lower Bound for R", "The lower bound of the right size of k-biplex", {'r', "r"}, 1);
    args::ValueFlag<int> Spenode(parser, "specific node", "maximum vertex k-biplex include a specific node", {'s', "s"}, -1);


    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        cout << parser;
        return 0;
    } catch (args::ParseError e) {
        cerr << e.what() << endl;
        cerr << parser;
        return 0;
    } catch (args::ValidationError e) {
        cerr << e.what() << endl;
        cerr << parser;
        return 0;
    }

	strncpy(filepath, args::get(benchmark_file).c_str(), FILELEN);
    delta = args::get(Delta);
	k = args::get(K);

	int threshold_l = args::get(Threshold_l);
    int threshold_r = args::get(Threshold_r);


	if(threshold_l<2*k+1 || threshold_r < 2*k+1){
		cout<<"size should be larger than "<< 2*k+1 << endl;
        exit(-1);
	}

    Util util;
	int Bipartite_index=0;
    int *degree=nullptr;
    int **Graph=nullptr;
    int graph_size=util.ReadGraph(filepath,Graph,degree,Bipartite_index);


    time_t s1 = clock();
    CoreLocate core(Graph, degree, graph_size,Bipartite_index,threshold_l-k);
    core.Coredecompose(); 
    int **pG=nullptr;
    int *pd=nullptr;
    int pb=0;
    int pgs=0;
    if(!core.CorePrune(pG,pd,pb,pgs)) return 0;
    time_t s2 = clock();

    Zigzag zigzag(pG,pd,pgs,pb,k,threshold_l,threshold_r,delta); 
    
    zigzag.ZigzagFramework();

    time_t s5 = clock();
    cout<<"Running Time: "<<((double)(s5-s1)/CLOCKS_PER_SEC)<<" sec"<<endl;




}    

