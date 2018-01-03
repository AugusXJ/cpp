#include "Node.h"
#include "ECgame.h"
#include "tool.h"
#include "20180102_method.h"
#include <map>

using namespace std;

//判断节点是否为已影响节点
bool ifInfluenced2(Node* n, map<Node*, bool> &ifchoosed)
{
    if(n->strategies == 1 || ifchoosed[n] == true)
        return true;
    for(auto i:n->nei())
    {
        if(i->strategies == 1 && ifchoosed[i] == false)
            return true;
    }
    return false;
}

//寻找节点n的未影响邻居数
int UnInfluNei2(Node* n, map<Node*, bool> &ifchoosed)
{
    int num = 0;
    for(auto i:n->nei())
    {
        if(ifInfluenced2(i, ifchoosed) == false)
            num++;
    }
    return num;
}

//邻居中未接种个数
int clusterNei(Node *n)
{
    int num = 0;
    for(auto i:n->nei())
    {
        if(i->strategies == 1)
            num++;
    }
    return num;
}

//得到节点n的分数
int Soc(Node* n, map<Node*, bool> &ifchoosed)
{
    int s = 0;
    for(auto i:n->nei())
    {
        if(ifInfluenced2(i, ifchoosed) == false)
            s += UnInfluNei2(i, ifchoosed);
    }
    return s;
}

double sigmoid(float frac)
{
    return 1/(1+exp(-10*(frac - 0.5)));
}

int method_20180102(NE &ne, double T)
{
    float frac = T/ne.lambda1;
    vector<Node*> nodes = ne.nodes;
    map<Node*, bool> ifchoosed;
    map<Node*, double> score;
    for(auto i:nodes)
    {
        i->strategies = 0;
        ifchoosed[i] = false;
        score[i] = 0;
    }
    for(int t = 0;t < ECgame::size;t++)
    {
        Node* target_node = nullptr;
        //计算每个个体的分数
        for(auto i:nodes)
        {
            if(ifchoosed[i] == false)
            {
                //double f = sigmoid(frac);
                double f = frac;
                score[i] = f * UnInfluNei2(i, ifchoosed) + (1-f) * clusterNei(i);
            }

        }
        double max_score = -1;
        for(auto i:nodes)
        {
            //cout<<i->getFlag()<<"  "<<score[i]<<endl;
            if(ifchoosed[i] == false && score[i] > max_score)
            {
                max_score = score[i];
            }
        }
        //选择相同分数的个体
        vector<Node*> candidate;
        for(auto i:nodes)
        {
            if( ifchoosed[i] == false && score[i] == max_score)
                candidate.push_back(i);
        }
        cout<<candidate.size()<<endl;
        if(candidate.size() == 1)
            target_node = candidate[0];
        else
        {
            max_score = -1;
            for(auto i:candidate)
            {
                int s = Soc(i, ifchoosed);
                if(s > max_score)
                {
                    max_score = s;
                    target_node = i;
                }
            }
        }
        cout<<"target_Node:   "<<target_node->getFlag()<<"  score:  "<<score[target_node];
        ifchoosed[target_node] = true;
        target_node->strategies = 1;
        double l = Tool::getMaxEigen(nodes);
        if(l > T)
        {
            target_node->strategies = 0;
            cout<<"  vacc  ";
        }
        else
        {
            cout<<"  unvacc  ";
        }
        cout<<endl;
        cout<<"lambda:   "<<l<<endl;
        cout<<"----------------------------------------------------"<<endl;
    }
    int num = count_if(nodes.begin(),nodes.end(),[](Node* n){return n->strategies == 0;});
        return num;
}

//---------------------------------------------------------------------------------
Node* UnInf_max_element(vector<Node*> nodes, map<Node*, bool> ifchoosed)
{
    Node* target_Node;
    int Uninf_max = -1;
    for(auto i:nodes)
    {
        if(ifchoosed[i] == false)
        {
            int num = UnInfluNei2(i, ifchoosed);
            if(num > Uninf_max)
            {
                target_Node = i;
                Uninf_max = num;
            }
        }
    }
    return target_Node;
}

bool ifchange_flag(vector<Node*> nodes, map<Node*, bool> ifchoosed)
{
    vector<Node*> unvacc;
    for(auto i:nodes)
    {
        if(i->strategies == 1)
        {
            for(auto j:i->nei())
            {
                if(ifchoosed[j] == false)
                    return false;
            }
        }
    }
    return true;
}
int method_20180103(NE &ne, double T)
{
    float frac = T/ne.lambda1;
    vector<Node*> nodes = ne.nodes;
    map<Node*, bool> ifchoosed;
    map<Node*, double> score;
    bool flag = false;              //用来标记是否有可选节点。如果没有选择影响节点最大的点
    for(auto i:nodes)
    {
        i->strategies = 0;
        ifchoosed[i] = false;
        score[i] = 0;
    }
    for(int t = 0;t < ECgame::size;t++)
    {
        Node* target_node = nullptr;
        if(flag == false)
        {
            target_node = UnInf_max_element(nodes, ifchoosed);
            flag = true;
        }
        else
        {
            vector<Node*> candi;
            //计算与簇相连的每个个体的分数
            for(auto i:nodes)
            {
                if(ifchoosed[i] == false)
                {
                    int n1 = clusterNei(i), n2 = UnInfluNei2(i, ifchoosed);
                    if(n1 != 0)
                    {
                        double f = sigmoid(frac);
                        score[i] = f * n2 + (1-f) * n1;
                        candi.push_back(i);
                    }
                }
            }
            if(candi.size() == 0)
                cout<<"error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
            double max_score = -1;
            for(auto i:candi)
            {
                //cout<<i->getFlag()<<"  "<<score[i]<<endl;
                if(ifchoosed[i] == false && score[i] > max_score)
                {
                    max_score = score[i];
                }
            }
            //选择相同分数的个体
            vector<Node*> candidate;
            for(auto i:candi)
            {
                if( ifchoosed[i] == false && score[i] == max_score)
                    candidate.push_back(i);
            }
            cout<<candidate.size()<<endl;
            if(candidate.size() == 1)
                target_node = candidate[0];
            else
            {
                max_score = -1;
                for(auto i:candidate)
                {
                    int s = Soc(i, ifchoosed);
                    if(s > max_score)
                    {
                        max_score = s;
                        target_node = i;
                    }
                }
            }
        }
        cout<<"target_Node:   "<<target_node->getFlag()<<"  score:  "<<score[target_node];
        ifchoosed[target_node] = true;
        target_node->strategies = 1;
        double l = Tool::getMaxEigen(nodes);
        if(l > T)
        {
            target_node->strategies = 0;
            cout<<"  vacc  ";
        }
        else
        {
            cout<<"  unvacc  ";
        }
        cout<<endl;
        cout<<"lambda:   "<<l<<endl;
        cout<<"----------------------------------------------------"<<endl;
        if(ifchange_flag(nodes, ifchoosed))
            flag = false;               //从true变为false
    }
    int num = count_if(nodes.begin(),nodes.end(),[](Node* n){return n->strategies == 0;});
        return num;
}
