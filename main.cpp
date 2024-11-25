#include "args.hxx"
#include <iostream>
#include "Util.h"
#include <string.h>
#include "Corepruning.h"
#include "Framework.h"
#include "GraphReduction.h"
#include "FastBB.h"

#define SIZE 100  


#define FILELEN 1024

using namespace std;

int Heuristic_search(int **Graph, int *H_degree, int graph_size, int bi_index, int k, int theta_l, int theta_r, int delta){
    int *degree = H_degree;
    int *flag = new int[graph_size];
    int lnode,left_size = bi_index;
    int rnode,right_size = graph_size-bi_index;
    int min_degreeL;
    int min_degreeR;
    int count;
    for(int i=0;i<graph_size;i++){
        flag[i]=1;
    }
    if(right_size<theta_r || left_size<theta_l) return 0;
    while(left_size > 2*k && right_size > 2*k){//是不是应该改成2k？

        min_degreeL = right_size;
        min_degreeR = left_size;
        count = 0;
        //1.search for min degree in L and R
        for(int i=0;i<bi_index;i++){
            if(flag[i] && degree[i]<min_degreeL){
                min_degreeL = degree[i];
                lnode = i;
            }
        }
        for(int i=bi_index;i<graph_size;i++){
            if(flag[i] && degree[i]<min_degreeR){
                min_degreeR = degree[i];
                rnode = i;
            }
        }
        //check result
        if(min_degreeL+k >= right_size && min_degreeR+k >= left_size ){
            
            //check balance
            if(left_size+delta < right_size){
                //also update result
                cout  << "Res_L:" ;
                for(int i=0;i<bi_index;i++){
                    if(flag[i]){
                        cout << i << " ";
                    }
                }
                cout  << endl;
                cout  << "Res_R:" ;
                count=0;
                for(int i=bi_index;i<graph_size;i++){
                    if(flag[i] && count < left_size+delta){//为了保持平衡，R侧只提取left_size+delta个
                        cout << i << " ";
                        count++;
                    }
                }
                cout  << endl;
                return left_size*2+delta;
            }else if(right_size+delta < left_size){
                cout  << "Res_L:" ;
                count=0;
                for(int i=0;i<bi_index;i++){
                    if(flag[i] && count < right_size+delta){
                        cout << i << " ";
                        count++;
                    }
                }
                cout  << endl;
                cout  << "Res_R:" ;
                for(int i=bi_index;i<graph_size;i++){
                    if(flag[i] ){
                        cout << i << " ";
                    }
                }
                cout  << endl;
                return right_size*2+delta;
            }else{
                cout  << "Res_L:" ;
                for(int i=0;i<bi_index;i++){
                    if(flag[i]){
                        cout << i << " ";
                    }
                }
                cout  << endl;
                cout  << "Res_R:" ;
                for(int i=bi_index;i<graph_size;i++){
                    if(flag[i] ){
                        cout << i << " ";
                    }
                }
                cout  << endl;
                return right_size+left_size;
            }
        }
        //update degree info
        if(min_degreeL+k < right_size){
            if(left_size>theta_l+1){
                flag[lnode] = 0;
                for(int j=0;j<degree[lnode];j++){
                    --degree[Graph[lnode][j]];
                }
                left_size-=1;
            }else {
                int count1 = right_size-min_degreeL-k;//还需要删除掉这么多非邻居
                if(right_size-count1>theta_r){
                    set<int> neighbors;
                    for(int i=0;i<degree[lnode];i++){
                        neighbors.insert(Graph[lnode][i]);
                    }
                    for(int i=bi_index;i<graph_size && count1 >0;i++){
                        if(neighbors.find(i)==neighbors.end() && flag[i]){
                            flag[i]=0;
                            for(int j=0;j<degree[i];j++){
                                --degree[Graph[i][j]];
                            }
                            right_size-=1;
                            count1-=1;
                        }
                    }
                }else{
                    return 0;
                }
            }
            
        }
        if(min_degreeR+k < left_size){
            if(right_size>theta_r+1){
                flag[rnode] = 0;
                for(int j=0;j<degree[rnode];j++){
                    --degree[Graph[rnode][j]];
                }
                right_size-=1;
            }else{
                int count2 = left_size-min_degreeR-k;
                if(left_size-count2>theta_l){
                    set<int> neighbors;
                    for(int i=0;i<degree[rnode];i++){
                        neighbors.insert(Graph[rnode][i]);
                    }
                    for(int i=0;i<bi_index && count2 >0;i++){
                        if(neighbors.find(i)==neighbors.end() && flag[i]){
                            flag[i]=0;
                            for(int j=0;j<degree[i];j++){
                                --degree[Graph[i][j]];
                            }
                            left_size-=1;
                            count2-=1;
                        }
                    }
                }else{
                    return 0;
                }
            }
            
        }
        
    }
   return 0;
}


int main(int argc, char** argv) {

    // defalt value for params
    int delta = 1; 
    int k = 1;
    int spenode = -1;
    char version[10]="";//有三个选项，分别是node,edge,spenode
    char filepath[1024] = "./";


	args::ArgumentParser parser(
        "MBBP, an algorithm for finding the maximum δ-balanced k-biplex\n");

    args::HelpFlag help(parser, "help", "Display this help menu",
                        {'h', "help"});
    args::Group required(parser, "", args::Group::Validators::All);

    args::ValueFlag<string> benchmark_file(parser, "benchmark", "Path to benchmark", {'f', "file"},"");
    args::ValueFlag<int> Delta(parser, "para delta", "The parameter delta", {'d', "d"}, 1);
	args::ValueFlag<int> K(parser, "para k", "The parameter k", {'k', "k"}, 1);
    args::ValueFlag<string> Version(parser, "version", "The version of MBBP", {'v', "v"}, "");

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
	strncpy(version, args::get(Version).c_str(), 10);
    delta = args::get(Delta);
	k = args::get(K);
    spenode = args::get(Spenode);

	int threshold_l = args::get(Threshold_l);
    int threshold_r = args::get(Threshold_r);

	// if (k<=0||delta<=0) {
	// 	fprintf(stderr, "k and delta should be at least 1\n");
	// 	exit(-1);
	// }

    if(strlen(version) == 0){
		cout<<"please indicate the version" << endl;
        exit(-1);
	}

    if( (strcmp(version, "spenode") == 0 || strcmp(version, "spenode_edge") == 0 ) && spenode < 0){
		cout<<"spenode id should be larger than 0" << endl;
        exit(-1);
	}

	if(threshold_l<2*k+1 || threshold_r < 2*k+1){
		cout<<"size should be larger than "<< 2*k+1 << endl;
        exit(-1);
	}

    Util util;
	int Bipartite_index=0;
    int *degree=nullptr;
    int **Graph=nullptr;
    int graph_size=util.ReadGraph(filepath,Graph,degree,Bipartite_index);
    cout << "原始大小：" << graph_size << endl;
    // for(int i=0;i<5;i++){
    //     for(int j=0;j<degree[i];j++){
    //         cout << Graph[i][j] << ",";
    //     }
    //     cout << endl;
    // }


    //以下为111
	//限制是2k的原因：要求找到的结果两边size>=2k+1(2k+1-k=k+1)，而且需要保证k为任何值的时候，候选的所有顶点都能符合这个要求。
    //k最小为1，那么为了保证两边顶点的数量都>=2k+1，需要保证顶点的core number>=2k(因为要满足k=1和这个最苛刻的条件)
    time_t s1 = clock();
    // CoreLocate core(Graph, degree, graph_size,Bipartite_index,threshold_l-k);//threshold_l-k原本是2*k
    // core.Coredecompose(); 
    // int **pG=nullptr;
    // int *pd=nullptr;
    // int pb=0;
    // int pgs=0;
    // if(!core.CorePrune(pG,pd,pb,spenode,pgs)) return 0;
    // time_t s2 = clock();
    // // cout<<"Core pruning Time: "<<((double)(s2-s1)/CLOCKS_PER_SEC)<<" sec"<<endl;
    // cout << "单独core prune后的大小：" << pgs << endl;

    // time_t s3 = clock();
    // GraphReduction graphReduction(pG,pd,pgs,pb,k,threshold_l);
    // int **pG2=nullptr;
    // int *pd2=nullptr;
    // int pb2=0;
    // int pgs2=0;
    // if(!graphReduction.butterflyReduction(pG2,pb2,pd2,spenode,pgs2)) return 0;
    // time_t s4 = clock();
    // // cout<<"Butterfly pruning Time: "<<((double)(s4-s3)/CLOCKS_PER_SEC)<<" sec"<<endl;
    // cout << "Core + butterfly prune后的大小：" << pgs2 << endl;

    // cout << "Heuristic_search = " << Heuristic_search(pG2,pd2,pgs2,pb2,k,threshold_l,threshold_r,delta) << endl;
    // Heuristic_search(pG2,pd2,pgs2,pb2,k,threshold_l,threshold_r,delta);
    

    // 运用PB框架 
    // Framework bkmb(Graph,degree,graph_size,Bipartite_index,k,threshold_l,threshold_r,delta,version,spenode); 
    // Framework bkmb(pG,pd,pgs,pb,k,threshold_l,threshold_r,delta,version,spenode); 
    
    // bkmb.PBIE();

    // //baseline solution: no progressive bounding 222
    FastBB bb(Graph,graph_size,Bipartite_index,degree,delta,k,threshold_l,threshold_r,0,version,spenode);
    // FastBB bb(pG,pgs,pb,pd,delta,k,threshold_l,threshold_r,0,version,spenode);
    bb.enuNum = 0;
    if(strcmp(version, "node") == 0){
        bb.MaxNodeNum = 0;
    }else if(strcmp(version, "edge") == 0){ 
        bb.MaxEdgeNum = 0;
    }
    bb.BKmb();
    cout << "enum number = " << bb.enuNum << endl;


    //end baseline solution
    time_t s5 = clock();
    cout<<"Running Time: "<<((double)(s5-s1)/CLOCKS_PER_SEC)<<" sec"<<endl;




}    

