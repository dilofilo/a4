#ifndef TRAINER_H
#define TRAINER_H
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stack>
#include <algorithm>
#include <functional>
#include "network.h"
#include <cassert>
#include <time.h>

/* Each observation is just a sequence of */
using namespace std;
typedef  vector<string> Observation;


void remove_quotes(std::string& x) {
	x = x.substr(1 , x.length() - 2);
}

// forward declaration of fns used




void sort_incomplete_data(Network&, vector<Observation>&);
int incomplete_node(const Observation&);
vector<int> topological_ordering(Network&);
void DFS(Network&, stack<int>&);
void DFS_recur(Network&, vector<int>&, int, stack<int>&);


struct topo_ordering
{
	vector<int> order;
	topo_ordering(vector<int>& o): order(o){}
	bool operator()(const std::vector<string>& a,const std::vector<string>& b){
		int _a = incomplete_node(a);
		int _b = incomplete_node(b);
		if (_a != -1 && _b != -1) {
			for (int i = 0; i < a.size(); i++)
				if (a[i] != b[i]) return a[i] < b[i];
			return false;
		}
		if (_a != -1) return false;
		if (_b != -1) return true;

		return order[_a] < order[_b]; // order[a] < order[b] means a-> b
	}
};


void sort_incomplete_data(Network& n, vector<Observation>& data){
	vector<int> order = topological_ordering(n);
	topo_ordering sorter(order);
	sort(data.begin(), data.end(), sorter);
}

int incomplete_node(const Observation& obs){
	
	for (int i = 0; i < obs.size(); i++) {
		if (obs[i] == "\"?\"") {
			return i;
		}
	}
	return -1;
}

vector<int> topological_ordering(Network& N){
	stack<int> reverse_post_order;
	DFS(N, reverse_post_order);
	vector<int> order(reverse_post_order.size(), 0);
	int o = 0;
	while (!reverse_post_order.empty()){
		int i = reverse_post_order.top(); reverse_post_order.pop();
		order[i] = o;
		o++;

	}
	return order;
}



// DFS
void DFS(Network& N, stack<int>& reverse_post_order){
	vector<int> explored(N.Pres_Graph.size(), 0);   
	
	for (int i = 0; i < explored.size(); i++){
		if (!explored[i])
			DFS_recur(N, explored, i,  reverse_post_order);
	}
}

void DFS_recur(Network& N, vector<int>& explored, int i,  stack<int>& reverse_post_order){
	explored[i] = 1;
	#define node N.Pres_Graph[i]
	for (int i = 0; i < node.Children.size(); i++){
		if (!explored[node.Children[i]])
			DFS_recur(N, explored, node.Children[i], reverse_post_order);
	}
	
	reverse_post_order.push(i);

	#undef node	
}


/* I choose you! */
class Trainer {
public:
	Network network; /* contains the CPT. */
	Network gold_network;
	float error;
	vector<Observation> data; 			//All observations.
	std::vector<bool> complete_observation; //tells if the observation is complete or not.
	

	Trainer() {
		srand(time(NULL));
	}
	~Trainer() {}


	void read_data(std::string fname);
	int which_case(Observation& obs , Graph_Node& node);
	void updateAll(Observation& obs);
	void update_count(Graph_Node& n , Observation& obs);
	void convertToCPT(Graph_Node&);
	//void probability_given_evidence( Graph_Node node , vector<int>& counts ,double& p); //from data.
	void write_data();
	
	vector<vector<float> > CPT();     
	/* compute error. */
	float calc_error();

	void un_update_count(Graph_Node& n, Observation& obs){
		int cas = which_case(obs , n);
	// cout << "#brk2.75 , updatecount() after which case\n";
		if (cas >= 0) {
			n.counts[cas] -= 1;
			assert( n.counts[cas] >= 0);
		}
	}
};
#endif