#pragma once
#include<iostream>
#include<vector>
#include<time.h>
#include"RandList.h"
#include<queue>
#include <map>
#include <cstdint>
using namespace std;

class MBKBPSearch
{
private:
    /* data */ 
    int **Graph;
    int *degree;
    int graph_size;
    int bi_index;
    int k;
    int left, right;//PB框架中左右顶点数量的下界
    int delta;
    int high_l;
    vector<int> Res_L,Res_R;
    int temp_i,temp_j,temp_k, temp_value, temp_node, temp_node2, temp_size1, temp_size2;
    MBitSet *bit_G;
    map<int, int> ubMap;
    RandList P_L, P_R, Cand_L, Cand_R, Exc_L, Exc_R;
    int *degInP;
    int *degInG;
    int *G_index; 
    int *G_index2; 
    int count;
    vector<int> temp_vector;
    bool CanBranch();
    void enumall();
    void recordRes();
    void enumRecord();
    bool checkBalance();
    uint32_t biplexUpperBound(uint32_t u,int sizeP1,int sizeP2,RandList *P2,int sizeC1,RandList *C1);

public:
    MBKBPSearch(int **Graph, int Graph_size, int Bipartite_index,int *degree, int delta, int k,int threshold_l,int threshold_r,int high_l,int *G_label,int enuNum);
    ~MBKBPSearch();
    void search();
    int MaxNodeNum;
    int MaxEdgeNum;
    int *re_temp_index;
    int *re_temp_index_spe;
    int *G_label;
    int enuNum;
    int binarySearchUB(int low,int high,vector<int> numbers,int k,int delta);
    int binarySearchUB2(int low,int high,vector<int> numbers,int k,int delta);
    void greedyAdd(vector<int>Np, RandList *&Cand1, RandList *&Cand2,RandList *&P1,RandList *&P2,RandList *&Exc1,RandList *&Exc2,vector<int>&remove_C1, vector<int>&remove_E1, vector<int>&remove_C2, vector<int>&remove_E2,int temp_stand1,int temp_stand2);
};

MBKBPSearch::MBKBPSearch(int **Graph, int Graph_size, int Bipartite_index,int *degree, int delta, int k,int threshold_l,int threshold_r,int high_l,int *G_label,int enuNum)
{
    this->Graph=Graph;
    this->degree=degree;   
    this->graph_size=Graph_size;
    this->bi_index=Bipartite_index;
    this->k=k;
    this->left=threshold_l;
    this->right=threshold_r;
    this->high_l = high_l;
    this->delta = delta;
    this->degree=degree;   
    this->G_label = G_label;
    this->enuNum=enuNum;

    P_L.init(graph_size);
    P_R.init(graph_size);
    Cand_L.init(graph_size);
    Cand_R.init(graph_size);
    Exc_L.init(graph_size);
    Exc_R.init(graph_size);
    temp_vector.reserve(graph_size);

    degInG=new int[graph_size];
    degInP=new int[graph_size];
    G_index=new int[graph_size];
    G_index2=new int[graph_size];
    count=0;

    for(temp_i=graph_size-1;temp_i>=0;--temp_i){
        degInP[temp_i]=0;
        degInG[temp_i]=0;
        G_index[temp_i]=0;
        G_index2[temp_i]=0;
    }


    bit_G=new MBitSet[graph_size];;
    for(int i=0;i<graph_size;++i){
        bit_G[i].allocacte(graph_size+1);
        bit_G[i].reinit(graph_size+1);
        for(int j=0;j<degree[i];++j){
            bit_G[i].set(Graph[i][j]);
        }
    }
}


MBKBPSearch::~MBKBPSearch()
{
    delete [] degInG;
    delete [] degInP;
    delete [] G_index;
    //delete []bit_G;
    //delete &temp_vector;
    //delete &P_L, &P_R, &Cand_L, &Cand_R, &Exc_L, &Exc_R;
}


void MBKBPSearch::search(){
    bool flag;

    for(int i=0;i<bi_index;++i){
        // 10.19新增S6：根据core number计算上界 BBB
        int ub_coreNum = 2*(G_label[i]+k)+delta;
        if(ub_coreNum <= MaxNodeNum){
            continue;
        }
        // S6 end
        P_L.add(i);//每一次，P_L都只有一个顶点。相当于是这一轮要包含i这个顶点,即IE框架        
        //获取Cand_R：（假设i及其后面的顶点都存在）先把可能符合规定的右侧邻居加入到Cand_R中
        for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
            temp_node=Graph[i][temp_i];//i的邻居
            count=0;
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                temp_node2=Graph[temp_node][temp_j];//temp_node2是i的邻居temp_node的邻居
                if(temp_node2>=i) count++;//要保证i的邻居的度数>left-k,否则不能加入当前k-biplex 
            }
            if(count<left-k) continue; 
            //更新度数信息
            degInG[temp_node]++;//因为i一定会加入，所以把temp_node度数+1
            degInP[temp_node]++;
            Cand_R.add(temp_node);
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){//temp_node此时加入了Cand_R，所以更新temp_node邻居的度数
                temp_node2=Graph[temp_node][temp_j];
                degInG[temp_node2]++;
            }
        }

        //获取Cand_L和Exc_L，左侧的候选节点应该是当前顶点i的二跳邻居 (如果左右两侧的顶点都大于2k，Luo的文章证明了：极大的k-biplex，同一侧的每个顶点之间至少会有一个共同邻居)
        for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
            temp_node=Graph[i][temp_i];
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                temp_node2=Graph[temp_node][temp_j];
                //第二个条件：因为上面只把i的符合条件的邻居加入了，没有考虑非邻居。边界情况是所有非邻居与当前这个temp_node2连接，所以判断条件是right-k-k
                //第三个判断条件：vpos这个列表的所有元素被初始化为graph_size，所以如果==graph_size，代表此节点没有被加入到Cand_L中
                if(temp_node2>i&&degInG[temp_node2]>=right-2*k&&Cand_L.vpos[temp_node2]==graph_size){
                    Cand_L.add(temp_node2);
                    for(temp_k=degree[temp_node2]-1;temp_k>=0;--temp_k){
                        degInG[Graph[temp_node2][temp_k]]++;//更新度数：此时temp_node2加入了Cand_L,更新其邻居的度数
                    }
                }
                if(temp_node2<i&&degInG[temp_node2]>=right-2*k&&Exc_L.vpos[temp_node2]==graph_size){
                    Exc_L.add(temp_node2);//这是之前遍历过的
                }
            }
        }

        //定下Cand_L后，遍历右侧顶点集合，重新获取Cand_R
        for(temp_i=bi_index;temp_i<graph_size;++temp_i){
            if(degInG[temp_i]>=left-k&&Cand_R.vpos[temp_i]==graph_size){//新加入:之前只考虑了i的邻居，这里补齐符合条件的非邻居
                Cand_R.add(temp_i);
                for(temp_j=degree[temp_i]-1;temp_j>=0;--temp_j){
                    degInG[Graph[temp_i][temp_j]]++;
                }
            }
            if(degInG[temp_i]<left-k&&Cand_R.vpos[temp_i]<graph_size){//不符合条件的去除(肯定是i的邻居，之前假设i后面所有顶点都在，其实并不是)
                Cand_R.remove(temp_i);
                //恢复一些度数
                for(temp_j=degree[temp_i]-1;temp_j>=0;--temp_j){
                    temp_node=Graph[temp_i][temp_j];
                    degInG[temp_node]--;
                    if(degInG[temp_node]<right-k&&Cand_L.vpos[temp_node]<graph_size){
                        //再次更新Cand_L
                        Cand_L.remove(temp_node);
                        for(temp_k=degree[temp_node]-1;temp_k>=0;--temp_k){
                            --degInG[Graph[temp_node][temp_k]];
                        }
                    }
                }
            }
        }

        flag=true;
        for(temp_i=Exc_L.vnum-1;temp_i>=0;--temp_i){
            temp_node=Exc_L.vlist[temp_i];
            if(degInG[temp_node]==Cand_R.vnum){
                flag=false;
                break;
            }
        }
        
        //新增上界剪枝S4：为候选集每个顶点计算了上界，并存储在Map结构中 BBB
        vector<int> NeighDeg;
        for(temp_i=Cand_L.vnum-1;temp_i>=0;--temp_i){
            temp_node=Cand_L.vlist[temp_i];
            NeighDeg.clear();
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                int neigh=Graph[temp_node][temp_j];
                NeighDeg.push_back(degree[neigh]+k+delta);
            }
            std::sort(NeighDeg.begin(), NeighDeg.end(), std::greater<int>());//降序排列
            int cnub = binarySearchUB2(0,NeighDeg.size()-1,NeighDeg,k,delta);
            ubMap[temp_node]=2*(cnub+k)+delta;
        }
        for(temp_i=Cand_R.vnum-1;temp_i>=0;--temp_i){
            temp_node=Cand_R.vlist[temp_i];
            NeighDeg.clear();
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                int neigh=Graph[temp_node][temp_j];
                NeighDeg.push_back(degree[neigh]+k+delta);
            }
            std::sort(NeighDeg.begin(), NeighDeg.end(), std::greater<int>());//降序排列
            int cnub = binarySearchUB2(0,NeighDeg.size()-1,NeighDeg,k,delta);
            ubMap[temp_node]=2*(cnub+k)+delta;
        }
        //end S4


        //10.17新增上界剪枝S5： BBB
        for(temp_j=degree[i]-1;temp_j>=0;--temp_j){
            int neigh=Graph[i][temp_j];
            G_index[neigh]=1;    
        }
        vector<int> commonNeighs;
        commonNeighs.clear();
        for(temp_i=Cand_L.vnum-1;temp_i>=0;--temp_i){
            temp_node=Cand_L.vlist[temp_i];
            count=0;
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                if(G_index[Graph[temp_node][temp_j]]){
                    count++;
                }
            }
            if(count>0)
                commonNeighs.push_back(count+2*k);
        }
        for(temp_i=P_L.vnum-1;temp_i>=0;--temp_i){
            temp_node=P_L.vlist[temp_i];
            count=0;
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                if(G_index[Graph[temp_node][temp_j]]){
                    count++;
                }
            }
            if(count>0) commonNeighs.push_back(count+2*k);
        }
        std::sort(commonNeighs.begin(), commonNeighs.end(), std::greater<int>());
        int cnub = 2*binarySearchUB(0,commonNeighs.size()-1,commonNeighs,k,delta)+delta;
        if(cnub<=MaxNodeNum){
            flag=false;
        }
        //end S5
        

        if(flag)
            enumall();

        //把Cand_R,Cand_L，Exc_L清空，P_L去除当前节点 ->都要处理度数信息
        for(temp_i=Cand_R.vnum-1;temp_i>=0;--temp_i){
            temp_node=Cand_R.vlist[temp_i];
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                --degInG[Graph[temp_node][temp_j]];
            }
        }
        for(temp_i=Cand_L.vnum-1;temp_i>=0;--temp_i){
            temp_node=Cand_L.vlist[temp_i];
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                --degInG[Graph[temp_node][temp_j]];
            }
        }
        
        for(temp_i=P_L.vnum-1;temp_i>=0;--temp_i){
            temp_node=P_L.vlist[temp_i];
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                --degInG[Graph[temp_node][temp_j]];
                --degInP[Graph[temp_node][temp_j]];
            }
        }
        
        Cand_R.clear();
        Cand_L.clear();
        P_L.clear();
        Exc_L.clear();      
    }
}

void MBKBPSearch::enumall(){
    // ||后面是P2 BBB
    if(P_L.vnum>=high_l||P_R.vnum>=high_l+delta){//high_l是左侧顶点的数量上界
        return;
    }
    
    //顶点数量没有达到下界
    if(P_L.vnum+Cand_L.vnum<left||P_R.vnum+Cand_R.vnum<right ) return;

    //以下为BBB
    //不是顶点最多
    if(P_L.vnum+Cand_L.vnum+P_R.vnum+Cand_R.vnum<=MaxNodeNum) return;
    //新增两个顶点数量的上界
    if((P_L.vnum+Cand_L.vnum)*2+delta<=MaxNodeNum) return;
    if((P_R.vnum+Cand_R.vnum)*2+delta<=MaxNodeNum) return;
    
    
    //非平衡剪枝 
    if(P_L.vnum>P_R.vnum+Cand_R.vnum+delta || P_R.vnum>P_L.vnum+Cand_L.vnum+delta){
        cout << "非平衡！" << endl;
        return;
    }

    int mindegPL = graph_size;
    int mindegPR = graph_size;
    //B1 BBB
    for(temp_i=P_L.vnum-1;temp_i>=0;--temp_i){
        temp_node=P_L.vlist[temp_i];
        if(degInG[temp_node]<mindegPL) mindegPL = degInG[temp_node];
    }
    for(temp_i=P_R.vnum-1;temp_i>=0;--temp_i){
        temp_node=P_R.vlist[temp_i];
        if(degInG[temp_node]<mindegPR) mindegPR = degInG[temp_node];
    }
    int num1 = mindegPL + mindegPR + 2*k;
    int num2 = mindegPL>mindegPR?mindegPL:mindegPR;
    if(num1<=MaxNodeNum||2*(num2+k)+delta<=MaxNodeNum){
        return;
    }
    //end B1
    
    if(Cand_R.empty()&&Cand_L.empty()){
        if(Exc_R.empty()&&Exc_L.empty() ){
            // enumRecord();//example print
            if(P_L.vnum>=left && P_R.vnum>=right){
                if(P_L.vnum+P_R.vnum>MaxNodeNum && checkBalance()){
                    MaxNodeNum = P_L.vnum+P_R.vnum;
                    recordRes();
                }
            }
        } 
        return;
    }

    //新增“根据顶点度数剪枝”S1 BBB
    for(temp_i=Cand_L.vnum-1;temp_i>=0;--temp_i){
        temp_node=Cand_L.vlist[temp_i];
        if(2*(degInG[temp_node]+k)+delta<=MaxNodeNum || ubMap[temp_node]<=MaxNodeNum){ //这里用上了10.15新增上界剪枝
            Cand_L.remove(temp_node);
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                temp_node2=Graph[temp_node][temp_j];
                --degInG[temp_node2];
            }
        }
    }
    for(temp_i=Cand_R.vnum-1;temp_i>=0;--temp_i){
        temp_node=Cand_R.vlist[temp_i];
        if(2*(degInG[temp_node]+k)+delta<=MaxNodeNum || ubMap[temp_node]<=MaxNodeNum){
            Cand_R.remove(temp_node);
            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                temp_node2=Graph[temp_node][temp_j];
                --degInG[temp_node2];
            }
        }
    }

    //end S1


    int povit=-1;
    temp_value=0;
    temp_node2=graph_size;
    temp_j=P_L.vnum+Cand_L.vnum;
    temp_k=P_R.vnum+Cand_R.vnum;
    //1. 寻找pivot顶点，第一个for从左边找，第二个for从右边找，哪个temp_value最大选哪个
    for(temp_i=P_L.vnum-1;temp_i>=0;--temp_i){
        temp_node=P_L.vlist[temp_i];
        if(temp_k-degInG[temp_node]>temp_value){//使用degInG去找，原因之一，是想寻找尽可能大的非邻居数量，使其>k,即pivot在P里面
            temp_value=temp_k-degInG[temp_node];//temp_value代表非邻居数量
            povit=temp_node;//pivot是非邻居数量最多的顶点，即度数最小的顶点
        }
        if(temp_node2>degInG[temp_node]){
            temp_node2=degInG[temp_node];
        }
    }
    if(temp_node2+k<right){//temp_node2代表P_L中顶点的最小度数(原代码附带的剪枝操作)
        return;
    }
    temp_size1=graph_size;
    for(temp_i=P_R.vnum-1;temp_i>=0;--temp_i){
        temp_node=P_R.vlist[temp_i];
        if(temp_j-degInG[temp_node]>temp_value){
            temp_value=temp_j-degInG[temp_node];
            povit=temp_node;
        }
        if(temp_size1>degInG[temp_node]){
            temp_size1=degInG[temp_node];
        }
    }
    if(temp_size1+k<left){
        return;
    }
    
    if(povit>=0&&temp_value>k){//找到了pivot,这个if代表pivot存在PL/PR中，而不是CandL/CandR中(要求pivot的非邻居数量>k)
        RandList *Cand1, *Cand2, *P1, *P2, *Exc1, *Exc2;
        int temp_stand1, temp_stand2,size1,size2;
        if(povit<bi_index){
            Cand1=&Cand_L;
            Cand2=&Cand_R;
            P1=&P_L;
            P2=&P_R;
            Exc1=&Exc_L;
            Exc2=&Exc_R;
            size1=bi_index;
            size2=graph_size-bi_index;
            temp_stand1=left;
            temp_stand2=right;
        }else{
            Cand1=&Cand_R;
            Cand2=&Cand_L;
            P1=&P_R;
            P2=&P_L;
            Exc1=&Exc_R;
            Exc2=&Exc_L;
            size1=graph_size-bi_index;
            size2=bi_index;
            temp_stand1=right;
            temp_stand2=left;
        }
        //2. 创建doing，计算分支数量
        vector<int> doing;
        doing.reserve(Cand2->vnum);//增加 vector 的容量，预留存储空间
        for(temp_i=Cand2->vnum-1;temp_i>=0;--temp_i){
            temp_node=Cand2->vlist[temp_i];
            if(!bit_G[povit].test(temp_node)){//如果当前的temp_node不存在位图中：含义就是，temp_node并不是pivot的邻居！
                doing.push_back(temp_node);
            }
        }
        int p=k-(P2->vnum-degInP[povit]), idx=1;//p代表还能加入多少个pivot的非邻居到P中去
        int rec_a, rec_b;
        rec_a=doing[0];//doing中存放的都是pivot的非邻居
        Cand2->remove(rec_a);
        Exc2->add(rec_a);
        
        for(temp_i=degree[rec_a]-1;temp_i>=0;--temp_i){
            --degInG[Graph[rec_a][temp_i]];
        }
        if(CanBranch()&&P2->vnum+Cand2->vnum>=temp_stand2)//这个应该是第一个分支，去掉了一个rec_a顶点
            enumall();

        Exc2->remove(rec_a);
        vector<int> remove_C1, remove_E1, remove_C2, remove_E2;
        remove_C1.reserve(Cand1->vnum);
        remove_C2.reserve(Cand2->vnum);
        remove_E1.reserve(Exc1->vnum);
        remove_E2.reserve(Exc2->vnum);
      
        while(idx<p+1){//同层的其他分支
            //P2中加入rec_a
            P2->add(rec_a);
            //BBB
            if(P2->vnum > P1->vnum+Cand1->vnum+delta) 
                break;
            for(temp_i=degree[rec_a]-1;temp_i>=0;--temp_i){
                temp_node=Graph[rec_a][temp_i];
                degInG[temp_node]++;//是因为前面第355行--了
                degInP[temp_node]++;
            }
            //更新Cand1
            if(degInP[rec_a]==P1->vnum-k){//rec_a的非邻居数量已达到上限，所以和Cand1中的顶点必须得是rec_a的邻居(即if的第一个条件)
                for(temp_i=Cand1->vnum-1;temp_i>=0;--temp_i){
                    temp_node=Cand1->vlist[temp_i];
                    if(!bit_G[rec_a].test(temp_node)||degInG[temp_node]<temp_stand2-k){//temp_stand2这里是最低下限
                        Cand1->remove(temp_node);
                        remove_C1.push_back(temp_node);
                        for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                            temp_node2=Graph[temp_node][temp_j];
                            --degInG[temp_node2];
                        }
                    }
                }

                for(temp_i=Exc1->vnum-1;temp_i>=0;--temp_i){
                    temp_node=Exc1->vlist[temp_i];
                    if(!bit_G[rec_a].test(temp_node)||degInG[temp_node]<temp_stand2-k){
                        Exc1->remove(temp_node);
                        remove_E1.push_back(temp_node);
                    }
                }
            }else{
                temp_size1=P2->vnum-k;
                for(temp_i=Cand1->vnum-1;temp_i>=0;--temp_i){
                    temp_node=Cand1->vlist[temp_i];
                    if(degInP[temp_node]<temp_size1||degInG[temp_node]<temp_stand2-k){
                        Cand1->remove(temp_node);
                        remove_C1.push_back(temp_node);
                        for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                            temp_node2=Graph[temp_node][temp_j];
                            --degInG[temp_node2];
                        }
                    }
                }

                for(temp_i=Exc1->vnum-1;temp_i>=0;--temp_i){
                    temp_node=Exc1->vlist[temp_i];
                    if(degInP[temp_node]<temp_size1||degInG[temp_node]<temp_stand2-k){
                        Exc1->remove(temp_node);
                        remove_E1.push_back(temp_node);
                    }
                }
            }
            //BBB
            if(Cand1->vnum+P1->vnum<temp_stand1 || Cand1->vnum+P1->vnum+delta<P2->vnum){
                break;
            }
            if((P1->vnum+Cand1->vnum)*2+delta<=MaxNodeNum || P1->vnum+P2->vnum+Cand1->vnum+Cand2->vnum<=MaxNodeNum) //新增数量上界剪枝
                break;


            temp_vector.clear();
            temp_size1=P2->vnum;
            temp_size2=P1->vnum;
            for(temp_i=P1->vnum-1;temp_i>=0;--temp_i){
                temp_node=P1->vlist[temp_i];
                if(degInP[temp_node]==temp_size1-k&&!bit_G[rec_a].test(temp_node)){
                    temp_vector.push_back(temp_node);//因为rec_a的加入，P1中某些顶点的非邻居数量到达了上限
                }
            }
            for(temp_i=degree[rec_a]-1;temp_i>=0;--temp_i){
                temp_node=Graph[rec_a][temp_i];
                if(Cand1->contains(temp_node)||P1->contains(temp_node))
                    G_index[temp_node]=1;//Cand1和P1中rec_a的邻居，被设置为1
            }
                    
            //更新Cand2
            if(!temp_vector.empty()){
                for(temp_i=Cand2->vnum-1;temp_i>=0;--temp_i){
                    temp_node=Cand2->vlist[temp_i];
                    if(degInP[temp_node]==temp_size2) continue;//跳过全连接的顶点
                    if(degInP[temp_node]<temp_size2-k||degInG[temp_node]<temp_stand1-k){//不符合条件的
                        Cand2->remove(temp_node);
                        remove_C2.push_back(temp_node);
                        for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                            temp_node2=Graph[temp_node][temp_j];
                            --degInG[temp_node2];
                        }
                    }else{
                        for(int i:temp_vector){
                            if(!bit_G[temp_node].test(i)){//temp_vector中的顶点非邻居数量到达上限，因此Cand2中所有顶点必须是其邻居
                                Cand2->remove(temp_node);
                                remove_C2.push_back(temp_node);
                                for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                                    temp_node2=Graph[temp_node][temp_j];
                                    --degInG[temp_node2];
                                }
                                break;
                            }
                        }
                        //以下为444
                        if(Cand2->contains(temp_node)){//如果通过上面这个for的条件，继续在这里看能否通过共同邻居的条件剪枝
                            count=0;
                            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                                if(G_index[Graph[temp_node][temp_j]]){
                                    count++;
                                }
                            }
                            //24.10.12日更新：MaxNodeNum本来应该只出现在node version中的，因为后续只做node的实验，所以暂时就这么写了更方便
                            if(count<temp_stand1-2*k ||count<=(MaxNodeNum-delta)/2-2*k){//用的是共同邻居+2k的剪枝条件S3 
                                Cand2->remove(temp_node);
                                remove_C2.push_back(temp_node);
                                for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                                    temp_node2=Graph[temp_node][temp_j];
                                    --degInG[temp_node2];
                                }
                            }
                        }   
                    }
                }

                for(temp_i=Exc2->vnum-1;temp_i>=0;--temp_i){
                    temp_node=Exc2->vlist[temp_i];
                    if(degInP[temp_node]==temp_size2) continue;
                    if(degInP[temp_node]<temp_size2-k||degInG[temp_node]<temp_stand1-k){
                        Exc2->remove(temp_node);
                        remove_E2.push_back(temp_node);
                    }else{
                        for(int i:temp_vector){
                            if(!bit_G[temp_node].test(i)){
                                Exc2->remove(temp_node);
                                remove_E2.push_back(temp_node);
                                break;
                            }
                        }
                    }
                }
            }
            //恢复
            for(temp_i=degree[rec_a]-1;temp_i>=0;--temp_i){
                temp_node=Graph[rec_a][temp_i];
                G_index[temp_node]=0;
            }
            
            //BBB
            if(Cand2->vnum+P2->vnum<temp_stand2 || Cand2->vnum+P2->vnum+delta<P1->vnum ){
                break;//因为Cand2会一直减少，要么转移到P2，要么被删除，所以如果此时大小已经不符合，后面的同层分支可以直接pass，所以从while中break
            }
            if((P2->vnum+Cand2->vnum)*2+delta<=MaxNodeNum || P1->vnum+P2->vnum+Cand1->vnum+Cand2->vnum<=MaxNodeNum) 
                break;
            

            // //S7加入更新代码 BBB
            // vector<int> Np;
            // Np.clear();
            // if(degInG[povit]==(MaxNodeNum-delta)/2-k){
            //     //把此顶点的所有邻居加入到Np中
            //     for(temp_i=degree[povit]-1;temp_i>=0;--temp_i){
            //         temp_node=Graph[povit][temp_i];
            //         Np.push_back(povit);
            //     }
            // //调用函数，把Np中的所有元素从C加入到P中(不知道是不是对所有数据集都有效，是否会拖慢速度？？)
            // greedyAdd(Np,Cand1,Cand2,P1,P2,Exc1,Exc2,remove_C1,remove_E1,remove_C2,remove_E2,temp_stand1,temp_stand2);
            // }
            // //endS7更新代码


            for(;idx<doing.size()&&!Cand2->contains(doing[idx]);++p,++idx);//doing一开始是从Cand2中生成的，doing中的某个顶点已经不存在于Cand2中，直接跳过
            if(idx==doing.size()) break;//doing没了
            rec_b=doing[idx];
            //删除下一个顶点，准备继续递归
            Cand2->remove(rec_b);
            Exc2->add(rec_b);
            for(temp_i=degree[rec_b]-1;temp_i>=0;--temp_i){
                temp_node=Graph[rec_b][temp_i];
                --degInG[temp_node];
            }
            if(CanBranch())
                enumall();

            Exc2->remove(rec_b);
            rec_a=rec_b;
            ++idx;
        }//while结束

        if(Cand1->vnum+P1->vnum>=temp_stand1&&Cand2->vnum+P2->vnum>=temp_stand2){//这里代码很奇怪，没啥用，ggg那里根本进不去
            
            count=0;
            for(;idx<doing.size();++idx){
                temp_node=doing[idx];
                if(Cand2->contains(temp_node)){
                    count++;
                }
            }
            
            if(Cand2->vnum+P2->vnum-count>=temp_stand2){
                for(;idx<doing.size();++idx){
                temp_node=doing[idx];
                if(Cand2->contains(temp_node)){
                    Cand2->remove(temp_node);
                    for(temp_i=degree[temp_node]-1;temp_i>=0;--temp_i){
                        --degInG[Graph[temp_node][temp_i]];
                    }
                }
                }
                if(CanBranch())
                    enumall();
            }
            
            
            
        }
        
        for(int i:doing){
            if(P2->vpos[i]<graph_size){
                P2->remove(i);
                Cand2->add(i);
                for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
                    temp_node=Graph[i][temp_i];
                    --degInP[temp_node];
                }
            }
            if(Cand2->vpos[i]>=graph_size){
                Cand2->add(i);
                for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
                    temp_node=Graph[i][temp_i];
                    degInG[temp_node]++;
                }

            }
        }

        for(int i:remove_C2){
            if(Cand2->vpos[i]==graph_size){
                Cand2->add(i);
                for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
                    temp_node=Graph[i][temp_i];
                    degInG[temp_node]++;
                }
            }
        }

        for(int i:remove_C1){
            Cand1->add(i);
            for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
                temp_node=Graph[i][temp_i];
                degInG[temp_node]++;
            }
        }

        for(int i:remove_E1){
            Exc1->add(i);
        }
        for(int i:remove_E2){
            Exc2->add(i);
        }

        

    }
    else{//pivot在Cand_L/R中，或者不存在
        for(temp_i=Cand_L.vnum-1;temp_i>=0;--temp_i){
            temp_node=Cand_L.vlist[temp_i];
            if(temp_k-degInG[temp_node]>temp_value){
                temp_value=temp_k-degInG[temp_node];
                povit=temp_node;
            }
        }

        for(temp_i=Cand_R.vnum-1;temp_i>=0;--temp_i){
            temp_node=Cand_R.vlist[temp_i];
            if(temp_j-degInG[temp_node]>temp_value){
                temp_value=temp_j-degInG[temp_node];
                povit=temp_node;
            }
        }

        if(temp_value<=k){//pivot的非邻居应该是不能<=k的;temp_value代表所有顶点的最大非邻数量，这时候如果所有顶点非邻居都<=k，就可以开始判断结果了
            if(Exc_L.empty()&&Exc_R.empty()){
                enumRecord();//example print
                int size = P_L.vnum+P_R.vnum+Cand_L.vnum+Cand_R.vnum;
                if( size>MaxNodeNum ){
                    if(checkBalance() && P_L.vnum+Cand_L.vnum>=left &&P_R.vnum+Cand_R.vnum>=right){//实验新增
                        MaxNodeNum = size;
                        recordRes();
                    }
                //     else{
                //     int num = P_L.vnum+Cand_L.vnum-P_R.vnum-Cand_R.vnum;//多出来的那几个
                //     if(num>0){//左边多
                //         for(int k=0;k<num-delta;k++){
                //             Cand_L.remove(Cand_L.vlist[Cand_L.vnum-1]);
                //         }
                //     }else{
                //         num = -num;
                //         for(int k=0;k<num-delta;k++){
                //             Cand_R.remove(Cand_R.vlist[Cand_R.vnum-1]);
                //         }
                //     }
                //     if(size-(num-delta)>MaxNodeNum){
                //         MaxNodeNum = size-(num-delta);
                //         recordRes();
                //     }
                // } 
                } 
                
                return; 
            }
            //Excl不为空，因此以下为非极大判断.(Excel不为空含义是：Excel中有顶点加入到P中，可以构成一个k-biplex，说明此时的P非极大)
            //这里相当于把Cand全部加入到P中，所以需要判断Excl中是否有顶点可以加入Cand+P中(函数开始处不需要判断excl是因为Cand为空，excl不会再因为Cand元素加入到P中，而导致excl元素减少)
            for(temp_i=Exc_R.vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc_R.vlist[temp_i];
                if(degInG[temp_node]==temp_j) return;
            }

            for(temp_i=Exc_L.vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc_L.vlist[temp_i];
                if(degInG[temp_node]==temp_k) return;
            }
            //看Exc_R/L中，是否能有顶点可以加入（我也写了这一条剪枝B0）
            temp_vector.clear();
            for(temp_i=P_L.vnum-1;temp_i>=0;--temp_i){
                temp_node=P_L.vlist[temp_i];
                if(degInG[temp_node]==temp_k-k){
                    temp_vector.push_back(temp_node);
                }
            }
            for(temp_i=Cand_L.vnum-1;temp_i>=0;--temp_i){
                temp_node=Cand_L.vlist[temp_i];
                if(degInG[temp_node]==temp_k-k){
                    temp_vector.push_back(temp_node);
                }
            } 
            for(temp_i=Exc_R.vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc_R.vlist[temp_i];//Excl中的任一顶点必须是temp_vector所有顶点的邻居
                if(degInG[temp_node]>=temp_j-k){
                    temp_value=1;
                    for(int i:temp_vector){
                        if(!bit_G[temp_node].test(i)){
                            temp_value=0;
                            break;
                        }
                    }
                    if(temp_value) return;
                }
            }

            temp_vector.clear();
            for(temp_i=P_R.vnum-1;temp_i>=0;--temp_i){
                temp_node=P_R.vlist[temp_i];
                if(degInG[temp_node]==temp_j-k){
                    temp_vector.push_back(temp_node);
                }
            }
            for(temp_i=Cand_R.vnum-1;temp_i>=0;--temp_i){
                temp_node=Cand_R.vlist[temp_i];
                if(degInG[temp_node]==temp_j-k){
                    temp_vector.push_back(temp_node);
                }
            }
            for(temp_i=Exc_L.vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc_L.vlist[temp_i];
                if(degInG[temp_node]>=temp_k-k){
                    temp_value=1;
                    for(int i:temp_vector){
                        if(!bit_G[temp_node].test(i)){
                            temp_value=0;
                            break;
                        }
                    }
                    if(temp_value) return;
                }
            }

            int size = P_L.vnum+P_R.vnum+Cand_L.vnum+Cand_R.vnum;
            enumRecord();//example print
            if( size>MaxNodeNum ){
                if(checkBalance() && P_L.vnum+Cand_L.vnum>=left &&P_R.vnum+Cand_R.vnum>=right){ //实验新增
                    MaxNodeNum = size;
                    recordRes();
                }
                // else{
                //     int num = P_L.vnum+Cand_L.vnum-P_R.vnum-Cand_R.vnum;//多出来的那几个
                //     if(num>0){//左边多
                //         for(int k=0;k<num-delta;k++){
                //             Cand_L.remove(Cand_L.vlist[Cand_L.vnum-1]);
                //         }
                //     }else{
                //         num = -num;
                //         for(int k=0;k<num-delta;k++){
                //             Cand_R.remove(Cand_R.vlist[Cand_R.vnum-1]);
                //         }
                //     }
                //     if(size-(num-delta)>MaxNodeNum){
                //         MaxNodeNum = size-(num-delta);
                //         recordRes();
                //     }
                // }
            } 
            
            return;
        }


        //pivot在Cand_L/R中：和上面类似的写法，只不过这里只有一个删除pivot的分支和加入pivot的分支，相当于先把pivot加入P中，在下一轮时就往上面的情况走了
        RandList *Cand1, *Cand2, *P1, *P2, *Exc1, *Exc2;
        int temp_stand1, temp_stand2;
        if(povit<bi_index){
            Cand1=&Cand_L;
            Cand2=&Cand_R;
            P1=&P_L;
            P2=&P_R;
            Exc1=&Exc_L;
            Exc2=&Exc_R;
            temp_stand1=left;
            temp_stand2=right;
        }else{
            Cand1=&Cand_R;
            Cand2=&Cand_L;
            P1=&P_R;
            P2=&P_L;
            Exc1=&Exc_R;
            Exc2=&Exc_L;
            temp_stand1=right;
            temp_stand2=left;
        }
        //第一个分支
        Cand1->remove(povit);
        Exc1->add(povit);
        for(temp_i=degree[povit]-1;temp_i>=0;--temp_i){
            temp_node=Graph[povit][temp_i];
            --degInG[temp_node];
        }

        if(CanBranch())
            enumall();
        
        //第二个分支
        Exc1->remove(povit);

        P1->add(povit);
        for(temp_i=degree[povit]-1;temp_i>=0;--temp_i){
            temp_node=Graph[povit][temp_i];
            degInG[temp_node]++;
            degInP[temp_node]++;
        }

        
        vector<int> remove_C1, remove_E1, remove_C2, remove_E2;
        remove_C1.reserve(Cand1->vnum);
        remove_C2.reserve(Cand2->vnum);
        remove_E1.reserve(Exc1->vnum);
        remove_E2.reserve(Exc2->vnum);

        if(degInP[povit]==P2->vnum-k){
            for(temp_i=Cand2->vnum-1;temp_i>=0;--temp_i){
                temp_node=Cand2->vlist[temp_i];
                if(!bit_G[povit].test(temp_node)||degInG[temp_node]<temp_stand1-k){
                    Cand2->remove(temp_node);
                    remove_C2.push_back(temp_node);
                    for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                        temp_node2=Graph[temp_node][temp_j];
                        --degInG[temp_node2];
                    }
                }
            }

            for(temp_i=Exc2->vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc2->vlist[temp_i];
                if(!bit_G[povit].test(temp_node)||degInG[temp_node]<temp_stand1-k){
                    Exc2->remove(temp_node);
                    remove_E2.push_back(temp_node);
                }
            }
        }else{
            temp_size1=P1->vnum-k;
            for(temp_i=Cand2->vnum-1;temp_i>=0;--temp_i){
                temp_node=Cand2->vlist[temp_i];
                if(degInP[temp_node]<temp_size1||degInG[temp_node]<temp_stand1-k){
                    Cand2->remove(temp_node);
                    remove_C2.push_back(temp_node);
                    for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                        temp_node2=Graph[temp_node][temp_j];
                        --degInG[temp_node2];
                    }
                }
            }

            for(temp_i=Exc2->vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc2->vlist[temp_i];
                if(degInP[temp_node]<temp_size1||degInG[temp_node]<temp_stand1-k){
                    Exc2->remove(temp_node);
                    remove_E2.push_back(temp_node);
                }
            }
        }

        
        for(temp_i=degree[povit]-1;temp_i>=0;--temp_i){
            temp_node=Graph[povit][temp_i];
            if(Cand2->contains(temp_node)||P2->contains(temp_node))
                G_index[temp_node]=1;
        }
        

        temp_vector.clear();
        temp_size1=P1->vnum;
        temp_size2=P2->vnum;
        for(temp_i=P2->vnum-1;temp_i>=0;--temp_i){
            temp_node=P2->vlist[temp_i];
            if(degInP[temp_node]==temp_size1-k&&!bit_G[povit].test(temp_node)){
                temp_vector.push_back(temp_node);
            }
        }

        if(!temp_vector.empty()&&Cand2->vnum+P2->vnum>=temp_stand2){
            for(temp_i=Cand1->vnum-1;temp_i>=0&&Cand1->vnum+P1->vnum>=temp_stand1;--temp_i){
                temp_node=Cand1->vlist[temp_i];
                if(degInP[temp_node]==temp_size2) continue;
                if(degInP[temp_node]<temp_size2-k||degInG[temp_node]<temp_stand2-k){
                    Cand1->remove(temp_node);
                    remove_C1.push_back(temp_node);
                    for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                        temp_node2=Graph[temp_node][temp_j];
                        --degInG[temp_node2];
                    }
                }else{
                    for(int i:temp_vector){
                        if(!bit_G[temp_node].test(i)){
                            Cand1->remove(temp_node);
                            remove_C1.push_back(temp_node);
                            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                                temp_node2=Graph[temp_node][temp_j];
                                --degInG[temp_node2];
                            }
                            break;
                        }
                    }
                    //444
                    if(Cand1->contains(temp_node)){
                        count=0;
                        for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                            if(G_index[Graph[temp_node][temp_j]]){
                                count++;
                            }
                        }
                        if(count<temp_stand2-2*k ){//S3 ||count<=(MaxNodeNum-delta)/2-2*k
                            Cand1->remove(temp_node);
                            remove_C1.push_back(temp_node);
                            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                                temp_node2=Graph[temp_node][temp_j];
                                --degInG[temp_node2];
                            }
                        }
                    }
                }
            }

            for(temp_i=Exc1->vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc1->vlist[temp_i];
                if(degInP[temp_node]==temp_size2) continue;
                if(degInP[temp_node]<temp_size2-k||degInG[temp_node]<temp_stand2-k){
                    Exc1->remove(temp_node);
                    remove_E1.push_back(temp_node);
                }else{
                    for(int i:temp_vector){
                        if(!bit_G[temp_node].test(i)){
                            Exc1->remove(temp_node);
                            remove_E1.push_back(temp_node);
                            break;
                        }
                    }
                }
                
            }
        }
        for(temp_i=degree[povit]-1;temp_i>=0;--temp_i){
            G_index[Graph[povit][temp_i]]=0;
        }
       
        if(CanBranch()&&P_R.vnum+Cand_R.vnum>=right&&P_L.vnum+Cand_L.vnum>=left)
            enumall();

        for(int i:remove_C1){
            Cand1->add(i);
            for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
                ++degInG[Graph[i][temp_i]];
            }
        }

        for(int i:remove_C2){
            Cand2->add(i);
            for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
                ++degInG[Graph[i][temp_i]];
            }
        }

        P1->remove(povit);
        Cand1->add(povit);
        for(temp_i=degree[povit]-1;temp_i>=0;--temp_i){
            --degInP[Graph[povit][temp_i]];
        }

        for(int i:remove_E1){
            Exc1->add(i);
        }
        for(int i:remove_E2){
            Exc2->add(i);
        }

        

    }

}


bool MBKBPSearch::checkBalance() {
    int left_size = P_L.vnum + Cand_L.vnum;
    int right_size = P_R.vnum + Cand_R.vnum;
    int size_diff = abs(left_size - right_size);

    return size_diff <= delta;
}


bool MBKBPSearch::CanBranch(){//检查当前分支是否是极大的，他的下一部分是在B0处
    temp_j=P_L.vnum+Cand_L.vnum;
    temp_k=P_R.vnum+Cand_R.vnum;
    for(temp_i=Exc_L.vnum-1;temp_i>=0;--temp_i){
        temp_node=Exc_L.vlist[temp_i];
        if(degInG[temp_node]==temp_k){
            return false;
        }
    }

    for(temp_i=Exc_R.vnum-1;temp_i>=0;--temp_i){
        temp_node=Exc_R.vlist[temp_i];
        if(degInG[temp_node]==temp_j){
            return false;
        }
    }

    return true;
}

void MBKBPSearch::recordRes(){
    Res_L.clear();
    Res_R.clear();
    
    //left side
    for(int i=P_L.vnum-1;i>=0;--i){
        Res_L.push_back(re_temp_index[P_L.vlist[i]]);
    }    

    for(int i=Cand_L.vnum-1;i>=0;--i){
        Res_L.push_back(re_temp_index[Cand_L.vlist[i]]); //BBB
    }    
    //right side
    for(int i=P_R.vnum-1;i>=0;--i){
        Res_R.push_back(re_temp_index[P_R.vlist[i]]);//BBB
    }
    for(int i=Cand_R.vnum-1;i>=0;--i){
        Res_R.push_back(re_temp_index[Cand_R.vlist[i]]);//BBB
    }

    cout  << "Update MBKBP: " << MaxNodeNum << endl;
    cout  << "|L|: " << Res_L.size() << endl;
    cout  << "|R|: " << Res_R.size() << endl;

    cout  << "Res_L:" ;
    for(vector<int>:: iterator iter=Res_L.begin();iter!=Res_L.end();iter++){
        cout  << *iter << " ";
    }
    cout  << endl;
    cout  << "Res_R:" ;
    for(vector<int>:: iterator iter=Res_R.begin();iter!=Res_R.end();iter++){
        cout  << *iter << " ";
    }
    cout  << endl;
}

void MBKBPSearch::enumRecord(){
    enuNum++;
    vector<int> resL,resR;
    resL.clear();
    resR.clear();
    
    //left side
    for(int i=P_L.vnum-1;i>=0;--i){
        resL.push_back(re_temp_index[P_L.vlist[i]]); //BBB
        // resL.push_back(P_L.vlist[i]); 
        
    }    

    for(int i=Cand_L.vnum-1;i>=0;--i){
        resL.push_back(re_temp_index[Cand_L.vlist[i]]); //BBB
        // resL.push_back(Cand_L.vlist[i]); 
    }    
    //right side
    for(int i=P_R.vnum-1;i>=0;--i){
        resR.push_back(re_temp_index[P_R.vlist[i]]);//BBB
        // resR.push_back(P_R.vlist[i]);
        
    }
    for(int i=Cand_R.vnum-1;i>=0;--i){
        resR.push_back(re_temp_index[Cand_R.vlist[i]]);//BBB
        // resR.push_back(Cand_R.vlist[i]);
    }
    // cout << "-----------------------new-----------------------" << endl;
    // cout  << "resL:" ;
    // for(vector<int>:: iterator iter=resL.begin();iter!=resL.end();iter++){
    //     cout  << *iter << " ";
    // }
    // cout  << endl;
    // cout  << "resR:" ;
    // for(vector<int>:: iterator iter=resR.begin();iter!=resR.end();iter++){
    //     cout  << *iter << " ";
    // }
    // cout  << endl;
    // cout << "-----------------------EndNew-----------------------" << endl;
}


uint32_t MBKBPSearch::biplexUpperBound(uint32_t u,int sizeP1,int sizeP2,RandList *P2,int sizeC1,RandList *C1) //u将加入P2
{
	std::vector<uint32_t>ptrB(k+1);//ptrB是一个大小为k+1的数组，记录了非邻居数为分别为0~k的顶点的数量
    std::vector<uint32_t>sup(graph_size);
    std::vector<std::vector<uint32_t>>B(k+1, std::vector<uint32_t>(graph_size));
	uint32_t supSum = 0;

    for(temp_i=P2->vnum-1;temp_i>=0;--temp_i){
        int v = P2->vlist[temp_i];
        sup[v] = k-(sizeP1-degInP[v]);
        supSum += sup[v];
    }

    if(degInG[u] < sizeC1){
        for(temp_i=0;temp_i<degree[u];temp_i++){
            temp_node = Graph[u][temp_i];
            if(!C1->contains(temp_node)) continue;
            uint32_t s = sizeP2-degInP[temp_node];
			B[s][ptrB[s]++] = temp_node; 
        }
    }else{
        for(temp_i=C1->vnum-1;temp_i>=0;--temp_i){
            temp_node = C1->vlist[temp_i];
            if(!bit_G[u].test(temp_node)) continue;
            uint32_t s = sizeP2-degInP[temp_node];
			B[s][ptrB[s]++] = temp_node; 
        }
    }

	uint32_t ub = sizeP1 + k - (sizeP1-degInP[u]);//还差“可加入的u的邻居”部分

	for (uint32_t i = 0; i <= k && i <= supSum; ++i) {
		for (uint32_t j = 0; j < ptrB[i] && i <= supSum; ++j) {

			uint32_t v = B[i][j], x = -1;//让邻居按照非邻居数量从小到大往里面加，能够在supSum的限制下，尽可能地加入更多的u的邻居

            for(temp_i=P2->vnum-1;temp_i>=0;--temp_i){
                int w = P2->vlist[temp_i];
                if(bit_G[v].test(w)) continue;
                if (x == -1u || sup[w] < sup[x])//x == -1u是为了让x有一个初值
					x = w;//找最小的sup
            }

			if (x == -1u || sup[x] > 0) {//sup[x] > 0可判断当前顶点是否可以加入。这里的x==-1u代表v没有非邻居。
				// if (x != -1u) --sup[x];//因为v是x的非邻居，要加进来(只操作x的原因难道是为了上界???是不是应该把w都操作一下)
				if (x != -1u){
                    for(temp_i=P2->vnum-1;temp_i>=0;--temp_i){
                        int w = P2->vlist[temp_i];
                        if(bit_G[v].test(w)) continue;
                        --sup[w];
                    }
                }
				supSum -= i;
				++ub;
			}
		}
	}

	return ub;
}


int MBKBPSearch::binarySearchUB(int low,int high,vector<int> numbers,int k,int delta){
    int max=0;
    while(low<=high){
        int mid ;
        mid = (high+low)/2;
        if(numbers[mid]-delta<mid+1){
            if(max < numbers[mid]) max=numbers[mid];
            high=mid-1;
        }else{
            if(max < mid+1+delta) max=mid+1+delta;
            low=mid+1;
        }
    }
    return max;
}

int MBKBPSearch::binarySearchUB2(int low,int high,vector<int> numbers,int k,int delta){
    int max=0;
    while(low<=high){
        int mid ;
        mid = (high+low)/2;
        if(numbers[mid]<mid+1){
            if(max < numbers[mid]) max=numbers[mid];
            high=mid-1;
        }else{
            if(max < mid+1) max=mid+1;
            low=mid+1;
        }
    }
    return max;
}


void MBKBPSearch::greedyAdd(vector<int>Np, RandList *&Cand1, RandList *&Cand2,RandList *&P1,RandList *&P2,RandList *&Exc1,RandList *&Exc2,vector<int>&remove_C1, vector<int>&remove_E1, vector<int>&remove_C2, vector<int>&remove_E2,int temp_stand1,int temp_stand2){
    for(int i: Np){
        //P中加入i
        P1->add(i);
        Cand1->remove(i);
        Exc1->add(i);
        //BBB
        if(P1->vnum > P2->vnum+Cand2->vnum+delta) 
            break;
        for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
            temp_node=Graph[i][temp_i];
            degInP[temp_node]++;
        }
        //更新Cand2
        if(degInP[i]==P2->vnum-k){//rec_a的非邻居数量已达到上限，所以和Cand1中的顶点必须得是rec_a的邻居(即if的第一个条件)
            for(temp_i=Cand2->vnum-1;temp_i>=0;--temp_i){
                temp_node=Cand2->vlist[temp_i];
                if(!bit_G[i].test(temp_node)||degInG[temp_node]<temp_stand1-k){//temp_stand2这里是最低下限
                    Cand2->remove(temp_node);
                    remove_C2.push_back(temp_node);
                    for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                        temp_node2=Graph[temp_node][temp_j];
                        --degInG[temp_node2];
                    }
                }
            }

            for(temp_i=Exc2->vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc2->vlist[temp_i];
                if(!bit_G[i].test(temp_node)||degInG[temp_node]<temp_stand1-k){
                    Exc2->remove(temp_node);
                    remove_E2.push_back(temp_node);
                }
            }
        }else{
            temp_size1=P1->vnum-k;
            for(temp_i=Cand2->vnum-1;temp_i>=0;--temp_i){
                temp_node=Cand2->vlist[temp_i];
                if(degInP[temp_node]<temp_size1||degInG[temp_node]<temp_stand1-k){
                    Cand2->remove(temp_node);
                    remove_C2.push_back(temp_node);
                    for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                        temp_node2=Graph[temp_node][temp_j];
                        --degInG[temp_node2];
                    }
                }
            }

            for(temp_i=Exc2->vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc2->vlist[temp_i];
                if(degInP[temp_node]<temp_size1||degInG[temp_node]<temp_stand1-k){
                    Exc2->remove(temp_node);
                    remove_E2.push_back(temp_node);
                }
            }
        }
        //BBB
        if(Cand2->vnum+P2->vnum<temp_stand2 || Cand2->vnum+P2->vnum+delta<P1->vnum){
            break;
        }
        // 这里只考虑顶点的剪枝，因为原代码中对于边最多在这里也没有任何剪枝操作（后面可以测试一下，剪枝到底会不会更快？）
        if((P2->vnum+Cand2->vnum)*2+delta<=MaxNodeNum || P1->vnum+P2->vnum+Cand1->vnum+Cand2->vnum<=MaxNodeNum) //新增数量上界剪枝
            break;

        temp_vector.clear();
        temp_size1=P1->vnum;
        temp_size2=P2->vnum;
        for(temp_i=P2->vnum-1;temp_i>=0;--temp_i){
            temp_node=P2->vlist[temp_i];
            if(degInP[temp_node]==temp_size1-k&&!bit_G[i].test(temp_node)){
                temp_vector.push_back(temp_node);//因为i的加入，P2中某些顶点的非邻居数量到达了上限
            }
        }
        for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
            temp_node=Graph[i][temp_i];
            if(Cand2->contains(temp_node)||P2->contains(temp_node))
                G_index[temp_node]=1;//Cand1和P1中i的邻居，被设置为1
        }
                
        //更新Cand1
        if(!temp_vector.empty()){
            for(temp_i=Cand1->vnum-1;temp_i>=0;--temp_i){
                temp_node=Cand1->vlist[temp_i];
                if(degInP[temp_node]==temp_size2) continue;//跳过全连接的顶点
                if(degInP[temp_node]<temp_size2-k||degInG[temp_node]<temp_stand2-k){//不符合条件的
                    Cand1->remove(temp_node);
                    remove_C1.push_back(temp_node);
                    for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                        temp_node2=Graph[temp_node][temp_j];
                        --degInG[temp_node2];
                    }
                }else{
                    for(int j:temp_vector){
                        if(!bit_G[temp_node].test(j)){//temp_vector中的顶点非邻居数量到达上限，因此Cand1中所有顶点必须是其邻居
                            Cand1->remove(temp_node);
                            remove_C1.push_back(temp_node);
                            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                                temp_node2=Graph[temp_node][temp_j];
                                --degInG[temp_node2];
                            }
                            break;
                        }
                    }
                    //以下为444
                    if(Cand1->contains(temp_node)){//如果通过上面这个for的条件，继续在这里看能否通过共同邻居的条件剪枝
                        count=0;
                        for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                            if(G_index[Graph[temp_node][temp_j]]){
                                count++;
                            }
                        }
                        //24.10.12日更新：MaxNodeNum本来应该只出现在node version中的，因为后续只做node的实验，所以暂时就这么写了更方便
                        if(count<temp_stand2-2*k||count<=(MaxNodeNum-delta)/2-2*k){//用的是共同邻居+2k的剪枝条件S3  
                            Cand1->remove(temp_node);
                            remove_C1.push_back(temp_node);
                            for(temp_j=degree[temp_node]-1;temp_j>=0;--temp_j){
                                temp_node2=Graph[temp_node][temp_j];
                                --degInG[temp_node2];
                            }
                        }
                    }   
                }
            }

            for(temp_i=Exc1->vnum-1;temp_i>=0;--temp_i){
                temp_node=Exc1->vlist[temp_i];
                if(degInP[temp_node]==temp_size2) continue;
                if(degInP[temp_node]<temp_size2-k||degInG[temp_node]<temp_stand2-k){
                    Exc1->remove(temp_node);
                    remove_E1.push_back(temp_node);
                }else{
                    for(int j:temp_vector){
                        if(!bit_G[temp_node].test(j)){
                            Exc1->remove(temp_node);
                            remove_E1.push_back(temp_node);
                            break;
                        }
                    }
                }
            }
        }
        //恢复
        for(temp_i=degree[i]-1;temp_i>=0;--temp_i){
            temp_node=Graph[i][temp_i];
            G_index[temp_node]=0;
        }
        
        //BBB
        if(Cand1->vnum+P1->vnum<temp_stand1 || Cand1->vnum+P1->vnum+delta<P2->vnum ){
            break;//因为Cand1会一直减少，要么转移到P1，要么被删除，所以如果此时大小已经不符合，后面的同层分支可以直接pass，所以从while中break
        }
        if((P1->vnum+Cand1->vnum)*2+delta<=MaxNodeNum || P1->vnum+P2->vnum+Cand1->vnum+Cand2->vnum<=MaxNodeNum) 
            break;

    }
}