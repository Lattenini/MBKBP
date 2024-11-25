#pragma once
#include<iostream>
#include <queue>
#include "FastBB.h"
#include <set>
using namespace std;

class Framework
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
    string version;
    int spenode;

    int *temp_index;
    int *temp_index2;
    int *temp_flag;
    int *temp_deg;
    int *degInG;
    int *re_temp_index;
    int *re_temp_index_spe;

    int temp_i,temp_j,temp_k, temp_node, temp_node2;


public:
    Framework(int **Graph, int *degree, int graph_size, int bi_index, int k, int theta_l, int theta_r, int delta, string version,int spenode);
    ~Framework();
    bool One_hop(int l, int r);
    void PBIE();
    int Heuristic_search(int **Graph, int *degree, int graph_size, int bi_index, int k, int theta_l, int theta_r, int delta);
};

Framework::Framework(int **Graph, int *degree, int graph_size, int bi_index, int k, int theta_l, int theta_r, int delta, string version,int spenode)
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
    this->version=version;
    this->spenode=spenode;

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

Framework::~Framework()
{
}


void Framework::PBIE(){
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
    cout << "maxRdeg+k = " << maxRdeg+k << endl;
    int last_v;

    // //问题三：
    // if(version == "spenode" || version == "speEdge"){
    //     //重新构图
    //     temp_flag[spenode] = 1;
    //     if(spenode<bi_index){
    //         //Cand_R第一次更新
    //         for(temp_i=degree[spenode]-1;temp_i>=0;--temp_i){
    //             temp_node=Graph[spenode][temp_i];//spenode的邻居
    //             if(degree[temp_node]>=theta_l-k){
    //                 temp_flag[temp_node]=1;
    //                 degInG[temp_node]++;
    //                 for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
    //                     temp_node2=Graph[temp_node][temp_j];
    //                     if(temp_flag[temp_node2] == 0){//二跳邻暂时全部加入
    //                         temp_flag[temp_node2]=1;
    //                         for(temp_k=degree[temp_node2]-1;temp_k>=0;--temp_k){
    //                             degInG[Graph[temp_node2][temp_k]]++;
    //                         }
    //                     }
                        
    //                 }
    //             }
    //         }
    //         for(temp_i=bi_index;temp_i<graph_size;++temp_i){
    //             if(degInG[temp_i]>=theta_l-k&&temp_flag[temp_i]==0){//新加入:之前只考虑了i的邻居，这里补齐符合条件的非邻居
    //                 temp_flag[temp_i]=1;
    //             }
    //             if(degInG[temp_i]<theta_l-k&&temp_flag[temp_i]==1){//不符合条件的去除(肯定是i的邻居，之前假设i后面所有顶点都在，其实并不是)
    //                 temp_flag[temp_i]=0;
    //             }
    //         }

    //         //根据temp_flag重新构图
    //         int count=0;
    //         int bi_count=0;
    //         for(int i=0;i<graph_size;++i){
    //             if(temp_flag[i]==1){
    //                 temp_index2[i]=count;
    //                 re_temp_index_spe[count] = i;//存储id的对应关系
    //                 if(i==spenode){
    //                     spenode = count;
    //                     // cout << "spenode最终更新为：" << count << endl;
    //                 }
    //                 count++;
    //                 if(i<bi_index){
    //                     bi_count++;
    //                 }
                    
    //             }else{
    //                 temp_index2[i]=-1;
    //             }
    //         }

            
    //         int new_bi=bi_count;
    //         int *new_degree=new int[count];
    //         int **new_graph=new int*[count];
    //         for(int i=0;i<graph_size;++i){
    //             if(temp_index2[i]>=0){
    //                 int temp_count=0;
    //                 for(int j=0;j<degree[i];++j){
    //                     if(temp_index2[Graph[i][j]]>=0){
    //                         temp_count++;
    //                     }
    //                 }
    //                 int *neg=new int[temp_count];
    //                 new_degree[temp_index2[i]]=temp_count;
    //                 temp_count=0;
                    
    //                 for(int j=0;j<degree[i];++j){
    //                     if(temp_index2[Graph[i][j]]>=0){
    //                         neg[temp_count]=temp_index2[Graph[i][j]];
    //                         temp_count++;
    //                     }
    //                 }
    //                 new_graph[temp_index2[i]]=neg;
    //             }
    //         }
    //         cout << "子图大小为： " << count << endl;
    //         //下一步需要把新图放入FastBB
    //         //10.19新增：根据core number计算上界
    //         CoreLocate core(new_graph, new_degree, count,new_bi,theta_l);
    //         core.Coredecompose(); //可以得到G_label：每个顶点的core number
    //         //根据core number计算上界 end
    //         FastBB bb(new_graph,count,new_bi,new_degree,delta,k,theta_l,theta_r,maxLdeg,version,spenode,core.G_label);
    //         if(version == "spenode"){
    //             bb.MaxNodeNum=0;
    //         }else if(version == "speEdge"){
    //             bb.MaxEdgeNum = 0;
    //         }
    //         bb.re_temp_index_spe = re_temp_index_spe;
    //         bb.BKmb();
    //     }else{//spenode来自于右侧
    //         for(temp_i=degree[spenode]-1;temp_i>=0;--temp_i){
    //             temp_node=Graph[spenode][temp_i];//i的邻居
    //             if(degree[temp_node]>=theta_r-k){
    //                 temp_flag[temp_node]=1;
    //                 degInG[temp_node]++;
    //                 for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
    //                     temp_node2=Graph[temp_node][temp_j];
    //                     if(temp_flag[temp_node2] == 0){//二跳邻暂时全部加入
    //                         temp_flag[temp_node2]=1;
    //                         for(temp_k=degree[temp_node2]-1;temp_k>=0;--temp_k){
    //                             degInG[Graph[temp_node2][temp_k]]++;
    //                         }
    //                     }
                        
    //                 }
    //             }
    //         }
    //         for(temp_i=0;temp_i<bi_index;++temp_i){
    //             if(degInG[temp_i]>=theta_r-k&&temp_flag[temp_i]==0){//新加入:之前只考虑了i的邻居，这里补齐符合条件的非邻居
    //                 temp_flag[temp_i]=1;
    //             }
    //             if(degInG[temp_i]<theta_r-k&&temp_flag[temp_i]==1){//不符合条件的去除(肯定是i的邻居，之前假设i后面所有顶点都在，其实并不是)
    //                 temp_flag[temp_i]=0;
    //             }
    //         }
    //         //根据temp_flag重新构图
    //         int count=0;
    //         int bi_count=0;
    //         for(int i=0;i<graph_size;++i){
                
    //             if(temp_flag[i]==1){
    //                 temp_index2[i]=count;
    //                 re_temp_index_spe[count] = i;//存储id的对应关系
    //                 if(i==spenode){
    //                     spenode = count;
    //                     // cout << "spenode最终更新为：" << count << endl;
    //                 }
    //                 count++;
    //                 if(i<bi_index){
    //                     bi_count++;
    //                 }
                    
    //             }else{
    //                 temp_index2[i]=-1;
    //             }
    //         }
            
    //         int new_bi=bi_count;
    //         int *new_degree=new int[count];
    //         int **new_graph=new int*[count];
    //         for(int i=0;i<graph_size;++i){
    //             if(temp_index2[i]>=0){
    //                 int temp_count=0;
    //                 for(int j=0;j<degree[i];++j){
    //                     if(temp_index2[Graph[i][j]]>=0){
    //                         temp_count++;
    //                     }
    //                 }
    //                 int *neg=new int[temp_count];
    //                 new_degree[temp_index2[i]]=temp_count;
    //                 temp_count=0;
                    
    //                 for(int j=0;j<degree[i];++j){
    //                     if(temp_index2[Graph[i][j]]>=0){
    //                         neg[temp_count]=temp_index2[Graph[i][j]];
    //                         temp_count++;
    //                     }
    //                 }
    //                 new_graph[temp_index2[i]]=neg;
    //             }
    //         }
    //         cout << "子图大小为： " << count << endl;
    //         //下一步需要把新图放入FastBB
    //         //10.19新增：根据core number计算上界
    //         CoreLocate core(new_graph, new_degree, count,new_bi,theta_l);
    //         core.Coredecompose(); //可以得到G_label：每个顶点的core number
    //         //根据core number计算上界 end
    //         FastBB bb(new_graph,count,new_bi,new_degree,delta,k,theta_l,theta_r,maxLdeg,version,spenode,core.G_label);
    //         if(version == "spenode"){
    //             bb.MaxNodeNum=0;
    //         }else if(version == "speEdge"){
    //             bb.MaxEdgeNum = 0;
    //         }
    //         bb.re_temp_index_spe = re_temp_index_spe;
    //         bb.BKmb();
    //     }
    //     return;
    // }
    // //问题三end
    int enuNum=0;
    while(low_l>theta_l){//这下面一块是PB框架
        if(version == "node"){
            last_v=low_l;
            low_l=low_l/2<theta_l?theta_l:low_l/2;
            // low_l=2*low_l/3<theta_l?2*theta_l:low_l/3;
            //P2
            int value = current-last_v>low_l-delta?current-last_v:low_l-delta;
            low_r=value<theta_r?theta_r:value;
            cout << "low_l = " << low_l << endl;
            cout << "low_r = " << low_r << endl;
        }else if(version == "edge"){
            last_v=low_l;
            low_r=current/low_l<theta_r?theta_r:current/low_l;
            low_l=low_l/2<theta_l?theta_l:low_l/2;
        }

        if(last_v-1+delta<low_r || 2*last_v+delta<current) //Edge需要注释掉
            break;
        One_hop(low_r-k, low_l-k);//one-hop Graph Reduction构成新图，注意这里输入的是low_r-k

        if(pgs!=0 ){
            cout << "pgs  = " << pgs << endl;
            //修改
            int alfa=(current-delta)/2-k;
            // cout << "alfa = " << alfa << endl;
            int **pG2=nullptr;
            int *pd2=nullptr;
            int pb2=0;
            int pgs2=0;
            if(alfa>0){
                CoreLocate core(pG, pd, pgs,pb,alfa);
                core.Coredecompose(); 
                if(!core.CorePrune(pG2,pd2,pb2,spenode,pgs2)) return;
                cout << "alfa prune后的大小：" << pgs2 << endl;
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
            // FastBB bb(pG2,pgs2,pb2,pd2,delta,k,low_l,low_r,last_v,version,spenode,core.G_label,enuNum); //Edge需要注释掉
            FastBB bb(pG,pgs,pb,pd,delta,k,low_l,low_r,last_v,version,spenode,core.G_label,enuNum);
            if(version == "node"){
                bb.MaxNodeNum = current;
            }else if(version == "edge"){
                bb.MaxEdgeNum = current;
            }

            bb.re_temp_index=re_temp_index;
            bb.BKmb();

            enuNum = bb.enuNum;

            if(version == "node"){
                current = bb.MaxNodeNum;
            }else if(version == "edge"){
                current = bb.MaxEdgeNum;
            }
        }
    }
    cout << "enum number = " << enuNum << endl;
}




bool Framework::One_hop(int l, int r){//先reduce图，在删减后又重新创建了新图。相当于删除了度数≤l以及≤r的顶点
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

//暂时只对version=node开heuristic_search
int Framework::Heuristic_search(int **Graph, int *H_degree, int graph_size, int bi_index, int k, int theta_l, int theta_r, int delta){
    int *degrees = H_degree;
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
    // while(left_size > 2*k && right_size > 2*k){//是不是应该改成2k+1？

    //     min_degreeL = right_size;
    //     min_degreeR = left_size;
    //     count = 0;
    //     //1.search for min degree in L and R
    //     for(int i=0;i<bi_index;i++){
    //         if(flag[i] && degrees[i]<min_degreeL){
    //             min_degreeL = degrees[i];
    //             lnode = i;
    //         }
    //     }
    //     for(int i=bi_index;i<graph_size;i++){
    //         if(flag[i] && degrees[i]<min_degreeR){
    //             min_degreeR = degrees[i];
    //             rnode = i;
    //         }
    //     }
    //     //check result
    //     if(min_degreeL+k >= right_size && min_degreeR+k >= left_size ){
            
    //         //check balance
    //         if(left_size+delta < right_size){
    //             //also update result
    //             cout  << "Res_L:" ;
    //             for(int i=0;i<bi_index;i++){
    //                 if(flag[i]){
    //                     cout << i << " ";
    //                 }
    //             }
    //             cout  << endl;
    //             cout  << "Res_R:" ;
    //             count=0;
    //             for(int i=bi_index;i<graph_size;i++){
    //                 if(flag[i] && count < left_size+delta){//为了保持平衡，R侧只提取left_size+delta个
    //                     cout << i << " ";
    //                     count++;
    //                 }
    //             }
    //             cout  << endl;
    //             return left_size*2+delta;
    //         }else if(right_size+delta < left_size){
    //             cout  << "Res_L:" ;
    //             count=0;
    //             for(int i=0;i<bi_index;i++){
    //                 if(flag[i] && count < right_size+delta){
    //                     cout << i << " ";
    //                     count++;
    //                 }
    //             }
    //             cout  << endl;
    //             cout  << "Res_R:" ;
    //             for(int i=bi_index;i<graph_size;i++){
    //                 if(flag[i] ){
    //                     cout << i << " ";
    //                 }
    //             }
    //             cout  << endl;
    //             return right_size*2+delta;
    //         }else{
    //             cout  << "Res_L:" ;
    //             for(int i=0;i<bi_index;i++){
    //                 if(flag[i]){
    //                     cout << i << " ";
    //                 }
    //             }
    //             cout  << endl;
    //             cout  << "Res_R:" ;
    //             for(int i=bi_index;i<graph_size;i++){
    //                 if(flag[i] ){
    //                     cout << i << " ";
    //                 }
    //             }
    //             cout  << endl;
    //             return right_size+left_size;
    //         }
    //     }
    //     //update degree info
    //     if(min_degreeL+k < right_size){
    //         if(left_size>theta_l+1){
    //             flag[lnode] = 0;
    //             for(int j=0;j<degrees[lnode];j++){
    //                 --degrees[Graph[lnode][j]];
    //             }
    //             left_size-=1;
    //         }else {
    //             int count1 = right_size-min_degreeL-k;//还需要删除掉这么多非邻居
    //             if(right_size-count1>theta_r){
    //                 set<int> neighbors;
    //                 for(int i=0;i<degrees[lnode];i++){
    //                     neighbors.insert(Graph[lnode][i]);
    //                 }
    //                 for(int i=bi_index;i<graph_size && count1 >0;i++){
    //                     if(neighbors.find(i)==neighbors.end() && flag[i]){
    //                         flag[i]=0;
    //                         for(int j=0;j<degrees[i];j++){
    //                             --degrees[Graph[i][j]];
    //                         }
    //                         right_size-=1;
    //                         count1-=1;
    //                     }
    //                 }
    //             }else{
    //                 return 0;
    //             }
    //         }
            
    //     }
    //     if(min_degreeR+k < left_size){
    //         if(right_size>theta_r+1){
    //             flag[rnode] = 0;
    //             for(int j=0;j<degrees[rnode];j++){
    //                 --degrees[Graph[rnode][j]];
    //             }
    //             right_size-=1;
    //         }else{
    //             int count2 = left_size-min_degreeR-k;
    //             if(left_size-count2>theta_l){
    //                 set<int> neighbors;
    //                 for(int i=0;i<degrees[rnode];i++){
    //                     neighbors.insert(Graph[rnode][i]);
    //                 }
    //                 for(int i=0;i<bi_index && count2 >0;i++){
    //                     if(neighbors.find(i)==neighbors.end() && flag[i]){
    //                         flag[i]=0;
    //                         for(int j=0;j<degrees[i];j++){
    //                             --degrees[Graph[i][j]];
    //                         }
    //                         left_size-=1;
    //                         count2-=1;
    //                     }
    //                 }
    //             }else{
    //                 return 0;
    //             }
    //         }
            
    //     }
        
    // }
   return 0;
}

