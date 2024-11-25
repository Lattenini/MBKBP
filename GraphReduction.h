#include "hash.h"


class GraphReduction
{
    public:
    GraphReduction(int **Graph, int *degree, int graph_size, int bipartite, int k, int threshold);
	bool butterflyReduction(int **&new_graph,int &new_bi,int *&new_degree,int &spenode,int &new_size);

    int k;
	int **Graph;
	int *degree;
	int graph_size;
	int bipartite;
	int threshold;
	int *G_index;
};

GraphReduction::GraphReduction(int **Graph, int *degree, int graph_size, int bipartite, int k, int threshold){
    this->k=k;
	this->Graph=Graph;
    this->degree=degree;
    this->graph_size=graph_size;
	this->bipartite=bipartite;
	this->threshold=threshold;
	G_index=new int[graph_size];
	for(int i=0;i<graph_size;++i){
        G_index[i]=-1;
    }
}


//adaption version

bool GraphReduction::butterflyReduction(int **&new_graph,int &new_bi,int *&new_degree,int &spenode,int &new_size) {
	std::vector<uint32_t>cn(graph_size);
	std::vector<uint32_t>deg(graph_size);
	deg.assign(degree,degree+graph_size);
	std::vector<bool> rmv(graph_size);//定义了一个存储布尔值的大小为2的向量数组 rmv 用来标记节点是否被移除。
	std::vector<CuckooHash> rme(bipartite);
	uint32_t temp_i,temp_j,temp_k;

	for (temp_i = 0; temp_i < graph_size; ++temp_i) {
		uint32_t u = temp_i, cntv = 0;

		if (rmv[u]) continue;

		for(temp_j=0;temp_j<degree[u];++temp_j){
			uint32_t v = Graph[u][temp_j];
			if(!rmv[v]){
				for(temp_k=0;temp_k<degree[v];++temp_k){
					uint32_t w = Graph[v][temp_k];
					if(!rmv[w]){
						cn[w]=0;
					}
				}
			}
		}
		for(temp_j=0;temp_j<degree[u];++temp_j){
			uint32_t v = Graph[u][temp_j];
			if(!rmv[v]){
				for(temp_k=0;temp_k<degree[v];++temp_k){
					uint32_t w = Graph[v][temp_k];
					if(!rmv[w]){
						++cn[w];
					}
				}
			}
		}

		for(temp_j=0;temp_j<degree[u];++temp_j){
			uint32_t v = Graph[u][temp_j];
			if(!rmv[v]){
				uint32_t cntw = 0;
				for(temp_k=0;temp_k<degree[v];++temp_k){
					uint32_t w = Graph[v][temp_k];
					if(!rmv[w] && cn[w] >= threshold-2*k){
						++cntw;
					}
				}
				if (cntw >= threshold-k) {++cntv;}
				else if (u < bipartite) {
					rme[u].insert(v);}
				else {
					rme[v].insert(u);}
			}
		}
		if (cntv < threshold-k) {
			rmv[u] = true;

			for(temp_j=0;temp_j<degree[u];++temp_j){
				uint32_t v = Graph[u][temp_j];
				if (--deg[v] < threshold-k){ 
					rmv[v] = true;}
			}
		}
	}
	//构建新图
	int count=0;
    int bi_count=0;
	//重新编号
	for(int i=0;i<graph_size;++i){
        if(!rmv[i]){
            G_index[i]=count;
            count++;
            if(i<bipartite){
                bi_count++;
            }
        }else{
            G_index[i]=-1;
        }
    }
	new_bi=bi_count;
	new_degree=new int[count];
    new_graph=new int*[count];
	new_size=count;
    for(int i=0;i<graph_size;++i){
        if(i == spenode){
            if(G_index[i]<0){
                cout << "bfprune: 您指定的顶点不可存在于连通的k-biplex内，请重新指定新的顶点" << endl;
				return false;
            }else{
                cout << "spenode原本为：" << spenode << " ——> 经过bfPrune更新为："<< G_index[spenode] << endl;
                spenode = G_index[spenode];
            }
        }
        if(G_index[i]>=0){
            int temp_count=0;
            for(int j=0;j<degree[i];++j){
				int neigh = Graph[i][j];
				bool edgeExit;
				if(i<bipartite)
					edgeExit = !rme[i].find(neigh);
				else edgeExit = !rme[neigh].find(i);
                if(G_index[neigh]>=0 && edgeExit){
                    temp_count++;
                }
            }
            int *neg=new int[temp_count];
            new_degree[G_index[i]]=temp_count;
            temp_count=0;
            
            for(int j=0;j<degree[i];++j){
				int neigh = Graph[i][j];
				bool edgeExit;
				if(i<bipartite)
					edgeExit = !rme[i].find(neigh);
				else edgeExit = !rme[neigh].find(i);
                if(G_index[neigh]>=0 && edgeExit){
                    neg[temp_count]=G_index[neigh];
                    temp_count++;
                }
            }
            new_graph[G_index[i]]=neg;
        }
    }

    return true;

}
