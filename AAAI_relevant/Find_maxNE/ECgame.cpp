﻿#include "ECgame.h"
#include <stdexcept>
#include <iterator>

using namespace std;

class Node;

const double ECgame::C = 1;             //接种花费
const double ECgame::L = 0;             //未接种未染病花费
const double ECgame::Le = 2;            //染病花费
const int ECgame::size = 15;          //节点个数
const float ECgame::learning_rate = 0.2;	//RLA算法学习率
double ECgame::T = 0;


void ECgame::readData(const std::string &filename)
{
	nodes = inputData(filename);
	lambda1 = Tool::getMaxEigen(nodes);
	//cout << "lambda1:  " << lambda1 << endl;
}



//备份，原来的状态
double NE::iterative_secure(double t,vector<Node*> &pai, ofstream &outfile)
{
	T = t; size_t size = nodes.size();


	double l = Tool::getMaxEigen(pai);
	cout<<"After subsidy MaxEigen: "<<l<<endl;
	size_t num = 0;
	double lastl = l;
	int index = pai.size()-1;
	cout<<"---------stage 1-----------"<<endl;
	while (l > T || l==-1)
	{
	    if(pai[index]->strategies == 1)
            pai[index]->strategies = 0;
        else
        {
            {
                index--;
                continue;
            }
        }
		l = Tool::getMaxEigen(pai);
		if(l==-1)
            l = lastl;
        lastl = l;
		num++;
		cout<<"add flag:  "<<pai[index]->getFlag()<<endl;
		cout<<"<<stage 1>>   num:  "<<num<<"\tmaxeigenV:  "<<l<<endl;
		index --;
	}
	cout<<"---------stage 2-----------"<<endl;
	index += 2;
    if(index >= pai.size())
    {
        int vacc_num = count_if(pai.begin(), pai.end(), [](Node* n){return n->strategies == 0;});
//		restrain_flag();                //重置编号
		for (auto i : pai)				//重置策略
			i->strategies = 1;
        return C*vacc_num*1.0;
    }

    while(index < pai.size())
    {
        num--;
        pai[index]->strategies = 1;
        cout<<"delete flag:  "<<pai[index]->getFlag()<<endl;

        l = Tool::getMaxEigen(pai);
        if(l==-1)
            l = lastl;
        lastl = l;
        //若去接种节点之后，特征值大于T，则重新接种该节点
        if(l > T)
        {
            cout<<"add flag:  "<<pai[index]->getFlag()<<endl;
            num++;
            pai[index]->strategies = 0;
        }
        cout<<"<<stage 2>>   num:  "<<num<<"\tmaxeigenV:  "<<l<<endl;
        index++;
    }
	int vacc_num = count_if(pai.begin(), pai.end(), [](Node* n){return n->strategies == 0;});
    outfile<<vacc_num<<endl;
	for (auto i : pai)
	{
	    if(i->strategies ==0)
        {
            vacc_info.push_back(i->getFlag());
            outfile<<i->getFlag()<<" ";
        }
	}

	return C*vacc_num*1.0;
}

//迭代求解，不好用，舍弃掉
//double NE::iterative_secure(double t,vector<Node*> &pai, ofstream &outfile)
//{
//	T = t; size_t size = nodes.size();
//	double l = Tool::getMaxEigen(pai);
//	cout<<"After subsidy MaxEigen: "<<l<<endl;
//	size_t num = 0;
//	double lastl = l;
//	vector<Node*> vacc;
//	vector<Node*> unvacc = pai;
//	int times = 0;
//	int max_vacc_num = 0;
//	while(times < 100)
//    {
//        times++;
//        if(!vacc.empty())
//        {
//            default_random_engine engine(time(nullptr));
//            uniform_int_distribution<> dis(0, vacc.size()-1);
//            int index = dis(engine);
//            cout<<"index:    -  - - -- - -"<<index<<endl;
//            auto it = vacc.begin()+index;
//            (*it)->strategies = 1;
//            vacc.erase(it);unvacc.insert(unvacc.begin(),*it);
//            l = Tool::getMaxEigen(unvacc);
//        }
//        cout<<"---------stage 1-----------"<<endl;
//        while ((l > T || l==-1) && !unvacc.empty())
//        {
//            Node* n = *unvacc.rbegin();
//            if(n->strategies == 1)
//            {
//                n->strategies = 0;
//                cout<<"add flag:  "<<n->getFlag()<<endl;
//                vacc.push_back(n);
//                unvacc.pop_back();
//            }
//            else
//            {
//                cout<<"something wrong"<<endl;
//                continue;
//            }
//            l = Tool::getMaxEigen(unvacc);
//            if(l==-1)
//                l = lastl;
//            lastl = l;
//            num++;
//            cout<<"<<stage 1>>   num:  "<<num<<"\tmaxeigenV:  "<<l<<endl;
//        }
//        cout<<"---------stage 2-----------"<<endl;
//        for(auto it = vacc.rbegin();it!=vacc.rend();)
//        {
//            Node *n = *it;
//            num--;
//            n->strategies = 1;unvacc.push_back(n);
//            cout<<"delete flag:  "<<n->getFlag()<<endl;
//            l = Tool::getMaxEigen(unvacc);
//            if(l==-1)
//                l = lastl;
//            lastl = l;
//            //若去接种节点之后，特征值大于T，则重新接种该节点
//            if(l > T)
//            {
//                cout<<"add flag:  "<<n->getFlag()<<endl;
//                n->strategies = 0;unvacc.pop_back();
//                it++;num++;
//            }
//            else
//            {
//                auto it2 = vacc.erase((++it).base());
//                vector<Node*>::reverse_iterator it3(it2);
//                it = it3;
//            }
//            cout<<"<<stage 2>>   num:  "<<num<<"\tmaxeigenV:  "<<l<<endl;
//        }
//        int vacc_num = count_if(vacc.begin(), vacc.end(), [](Node* n){return n->strategies == 0;});
//        if(vacc_num != vacc.size())
//            cout<<"error!!!"<<endl;
//        if(vacc_num > max_vacc_num)
//            max_vacc_num = vacc_num;
//    }
//    outfile<<max_vacc_num<<endl;
////	for (auto i : vacc)
////	{
////        vacc_info.push_back(i->getFlag());
////        outfile<<i->getFlag()<<" ";
////	}
//	return C*max_vacc_num*1.0;
//}


double NE::HDG(double t, ofstream &outfile)
{
    cout<<"--------------------------HDG------------------------"<<endl;
    vector<Node*> pai = nodes;
    for(auto it=pai.begin();it!=pai.end();)
    {
        if((*it)->ifGetSubsidy == true)
            it = pai.erase(it);
        else
            it++;
    }
	sort(pai.begin(), pai.end(), [](Node *n1, Node *n2) {return n1->degree() < n2->degree(); });//from min to max
    return iterative_secure(t, pai, outfile);
}

double NE::LDG(double t, ofstream &outfile)
{
    cout<<"--------------------------LDG------------------------"<<endl;
    vector<Node*> pai = nodes;
    for(auto it=pai.begin();it!=pai.end();)
    {
        if((*it)->ifGetSubsidy == true)
            it = pai.erase(it);
        else
            it++;
    }
	sort(pai.begin(), pai.end(), [](Node *n1, Node *n2) {return n1->degree() > n2->degree(); });//from min to max
    return iterative_secure(t, pai, outfile);
}

double NE::get_NEcost(double t, const string &fileName, const string &flag, ofstream &outfile)
{
    cout<<"--------------------------"<<fileName<<"------------------------"<<endl;
    if(flag!="High" && flag!="Low")
        throw runtime_error("get_NEcost need a High or Low flag");
    ifstream infile(fileName);
    vector<Node*> pai;int a;
    while(infile>>a)
    {
        if(nodes[a]->ifGetSubsidy == false)
            pai.push_back(nodes[a]);
    }
    if(flag == "High")
        reverse(pai.begin(),pai.end());
    return iterative_secure(t, pai, outfile);
}

void NE::RLA_algorithm(double T)
{
	//1) Set an initial probability pi(0), for each user i =1,...,N.
	size_t size = nodes.size();
	vector<float> pi(size, 0.5);
	vector<double> payoff(size);
	int t = 0;
	default_random_engine engine(time(nullptr));		//随机数种子
	uniform_real_distribution<double> u(0, 1);
	while (true)
	{
		for (int i = 0; i < size; i++)
		{
			// 2) At every time step t, each node i invests with probability
			// pi(t).This determines his pure action σi(t).
			if (u(engine) > pi[i])
				nodes[i]->strategies = 0;
			else
				nodes[i]->strategies = 1;
		}
		// 3) Each player i obtains his payoff Si(σi(t); σ ¯(t))
		double l = Tool::getMaxEigen(nodes);
		for (int i = 0; i < size; i++)
		{
			if (nodes[i]->strategies == 0)
				payoff[i] = L;
			else
				payoff[i] = l < T ? 0 : Le;
			payoff[i] /= L + Le;
		}
		// 4) Each node i updates its probability according to the following rule :
		for (int i = 0; i < size; i++)
		{
			pi[i] = pi[i] + learning_rate * (1-payoff[i]) * (1 - nodes[i]->strategies - pi[i]);
		}
	}
}



//重置nodes的编号
//void NE::restrain_flag()
//{
//    for(int i=0;i<nodes.size();i++)
//        nodes[i]->flag = nodes[i]->fixflag;
//}

//补助
void NE::subsidy(vector<Node*> pai)
{
    for(auto i:pai)
    {
        i->ifGetSubsidy = true;
        i->strategies = 1;
    }
}

void NE::init()
{
    vacc_info.clear();
	for (auto i : nodes)
	{
		i->flag = i->fixflag;
		i->strategies = 1;
		i->ifGetSubsidy = false;
	}
}

