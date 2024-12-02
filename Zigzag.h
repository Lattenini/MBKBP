#pragma once
#include<iostream>
#include <queue>
#include "MBKBPSearch.h"
#include <set>
#include "Corepruning.h"
using namespace std;

class Zigzag
{
private:
    int **Graph;
    int *degree;
    int bi_index;
    int graph_size;
    int k;

    int **pG;
    int *pd;
    int pb, pgs;
    int theta_l,theta_r;
    int delta;

    int *temp_index;
    int *temp_index2;
    int *temp_flag;
    int *temp_deg;
    int *degInG;
    int *re_temp_index;
    int *re_temp_index_spe;

    int temp_i,temp_j,temp_k, temp_node, temp_node2;


public:
    Zigzag(int **Graph, int *degree, int graph_size, int bi_index, int k, int theta_l, int theta_r, int delta);
    ~Zigzag();
    bool One_hop(int l, int r);
    void ZigzagFramework();
};

Zigzag::Zigzag(int **Graph, int *degree, int graph_size, int bi_index, int k, int theta_l, int theta_r, int delta)
{
    this->Graph=Graph;
    this->degree=degree;
    this->graph_size=graph_size;
    this->bi_index=bi_index;
    this->k=k;
    this->theta_l=theta_l;
    this->theta_r=theta_r;
    this->delta=delta;
    this->pgs=0;

    temp_index=new int[graph_size];
    temp_index2=new int[graph_size];
    temp_flag=new int[graph_size];
    temp_deg=new int[graph_size];
    degInG=new int[graph_size];
    re_temp_index=new int[graph_size];
    re_temp_index_spe=new int[graph_size];

    for(int i=graph_size-1;i>=0;--i){
        temp_index[i]=0;
        temp_index2[i]=-1;
        temp_flag[i]=0;
        temp_deg[i]=0;
        degInG[i]=0;
        re_temp_index[i]=0;
        re_temp_index_spe[i]=0;
    }
    
}

Zigzag::~Zigzag()
{
}


void Zigzag::ZigzagFramework(){
    int current=0;
    int maxLdeg=0, maxRdeg=0;
    //寻找左侧顶点的最大degree
    for(temp_i=bi_index-1;temp_i>=0;--temp_i){
        if(degree[temp_i]>maxLdeg){
            maxLdeg=degree[temp_i];
        }
    }
    //寻找右侧顶点的最大degree
    for(temp_i=graph_size-1;temp_i>=bi_index;--temp_i){
        if(degree[temp_i]>maxRdeg){
            maxRdeg=degree[temp_i];
        }
    }
    int low_l, low_r;
    low_l=maxRdeg+k;
    int last_v;

    int enuNum=0;
    while(low_l>theta_l){
        last_v=low_l;
        low_l=low_l/2<theta_l?theta_l:low_l/2;
        int value = current-last_v>low_l-delta?current-last_v:low_l-delta;
        low_r=value<theta_r?theta_r:value;
        if(last_v-1+delta<low_r || 2*last_v+delta<current) //Edge需要注释掉
            break;
        One_hop(low_r-k, low_l-k);//one-hop Graph Reduction构成新图，注意这里输入的是low_r-k

        if(pgs!=0 ){
            // cout << "pgs  = " << pgs << endl;
            //修改
            int alfa=(current-delta)/2-k;
            int **pG2=nullptr;
            int *pd2=nullptr;
            int pb2=0;
            int pgs2=0;
            if(alfa>0){
                CoreLocate core(pG, pd, pgs,pb,alfa);
                core.Coredecompose(); 
                if(!core.CorePrune(pG2,pd2,pb2,pgs2)) return;
                // cout << "alfa prune后的大小：" << pgs2 << endl;
            }else{
                pG2=pG;
                pd2=pd;
                pb2=pb;
                pgs2=pgs;
            }
            //修改end
            //10.19新增：根据core number计算上界
            CoreLocate core(pG2, pd2, pgs2,pb2,alfa);
            core.Coredecompose(); //可以得到G_label：每个顶点的core number
            //根据core number计算上界 end
            MBKBPSearch ms(pG2,pgs2,pb2,pd2,delta,k,low_l,low_r,last_v,core.G_label,enuNum);
            
            ms.MaxNodeNum = current;

            ms.re_temp_index=re_temp_index;
            ms.search();

            enuNum = ms.enuNum;

            current = ms.MaxNodeNum;

        }
    }
    // cout << "enum number = " << enuNum << endl;
}




bool Zigzag::One_hop(int l, int r){//先reduce图，在删减后又重新创建了新图。相当于删除了度数≤l以及≤r的顶点
    for(temp_i=graph_size-1;temp_i>=0;--temp_i){
        temp_index[temp_i]=1;
        temp_deg[temp_i]=degree[temp_i];
    }

    bool flag=true;
    int countL=0, countR=0;
    while(flag){
        flag=false;
        for(temp_i=bi_index-1;temp_i>=0;--temp_i){
            if(temp_index[temp_i]==0||temp_deg[temp_i]>=l) continue;//temp_index[temp_i]==0的条件防止重复删除
            temp_index[temp_i]=0;
            flag=true;
            countL++;
            for(temp_j=degree[temp_i]-1;temp_j>=0;--temp_j){
                if(temp_index[Graph[temp_i][temp_j]]){
                    --temp_deg[Graph[temp_i][temp_j]];
                }
            }
        }
        for(temp_i=bi_index;temp_i<graph_size;++temp_i){
            if(temp_index[temp_i]==0||temp_deg[temp_i]>=r) continue;
            flag=true;
            countR++;
            temp_index[temp_i]=0;

            for(temp_j=degree[temp_i]-1;temp_j>=0;--temp_j){
                if(temp_index[Graph[temp_i][temp_j]]){
                    --temp_deg[Graph[temp_i][temp_j]];
                }
            }
        }
    }

    temp_i=1;

    for(temp_j=0;temp_j<graph_size;++temp_j){//为重新构图编号，这里的temp_index都+1了，可能是想利用>0为真，=0为假的特性
        if(temp_index[temp_j]){
            temp_index[temp_j]=temp_i;
            ++temp_i;
        }
    }

    if(pgs!=0){
        for(temp_i=pgs-1;temp_i>=0;--temp_i){
            delete [] pG[temp_i];
        }
    
        delete []pd;
        delete []pG;
    }
    
    pgs=graph_size-countL-countR;
    if(pgs==0) return false;
    pd=new int[pgs];

    for(temp_j=0;temp_j<graph_size;++temp_j){//为重新构图编号
        if(temp_index[temp_j]){
            temp_node=temp_index[temp_j]-1;//temp_node代表temp_j这个顶点在新图中的编号
            re_temp_index[temp_node]=temp_j;//通过re_temp_index可以找到原本的图顶点号
        }
        // cout << "新id:" << temp_node << "/旧id:" << temp_j << endl;
    }


    pb=bi_index-countL;
    pG=new int*[pgs];
    for(temp_i=0;temp_i<graph_size;++temp_i){//构图中...
        temp_node=temp_index[temp_i]-1;
        if(temp_node>=0){
            pd[temp_node]=temp_deg[temp_i];
            pG[temp_node]=new int[temp_deg[temp_i]];
            
            temp_k=0;
            for(temp_j=0;temp_j<degree[temp_i];++temp_j){
                temp_node2=Graph[temp_i][temp_j];;
                if(temp_index[temp_node2]){
                    pG[temp_node][temp_k]=temp_index[temp_node2]-1;
                    temp_k++;
                }
            }
        }
    }  
    return true;
}

