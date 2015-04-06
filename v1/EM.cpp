#ifndef EM_CPP
#define EM_CPP
#include "trainer.h"
#include <iostream>
#include <algorithm>
#include "network.h"
#include <cassert>
#include <math.h>
#include <random>
#include <numeric>

typedef std::vector<float> CPT;


#define ACC 100000000
float rng(int accuracy = ACC) {
	return ((rand()%accuracy)/(float)accuracy);
}


void probability_table(Trainer& ,Network& n,int node_index, Observation& obs, std::vector<float>& cpt);
float probability(Trainer&,Network&,int node_index, std::string& val, Observation& obs);
void complete_data(Network&,Observation& obs,float val ,int node_index, vector<float>& table);
void EM(Trainer& t,Network& a, Observation&  obs);
void EM2(Trainer& t, Network& a);

bool numerical_conv(float& a, float& b, float tolerance = 0.0001){
	return abs(a-b) <= tolerance;
}

bool converges(std::vector<CPT>& a, std::vector<CPT> & b){

	for (int i = 0; i < a.size(); i++){
		for (int j = 0; j < a[i].size(); j++){
			if (!numerical_conv(a[i][j], b[i][j]))
				return false;

		}
	}

	return true;
}	


bool is_complete(Observation& obs);


void maximization(Trainer& t, Observation& obs, int node_index);
void EM_complete_data(Trainer& t){
	#define a t.network
	#define DATA t.data
	sort_incomplete_data(a, DATA);
	for (int i = 0; i  < DATA.size(); i++){
		if (!is_complete(DATA[i]))
			{
				EM(t, a, DATA[i]);
				t.complete_observation[i] = true;
			}

	}

	cout << "DONE!"<< '\n';
	#undef DATA	
	#undef a
}

void EM(Trainer& t,Network& a, Observation&  obs){
	assert (!is_complete(obs));
	vector<CPT > init_dist = t.CPT();
	int incomplete_node_index = incomplete_node(obs);
	std::vector<float> table; 
	probability_table(t,a,incomplete_node_index, obs, table);

	float val = rng();

	complete_data(a,obs, val, incomplete_node_index ,table);
	
	maximization(t,obs, incomplete_node_index);
	vector<CPT> final_cpt = t.CPT();

	if (converges(init_dist , final_cpt) ) 
		return;

	else{
		obs[incomplete_node_index] = "\"?\"";
		EM(t, a, obs);
	}
}


void maximization(Trainer& t, Observation& obs, int node_index){
	#define node t.network.Pres_Graph[node_index]
	#define graph t.network.Pres_Graph
	t.update_count(node, obs);
	t.convertToCPT(node);
	for (int i = 0; i < node.Children.size(); i++){
		t.update_count(graph[node.Children[i]] , obs);
		t.convertToCPT(graph[node.Children[i]]);
	}

	#undef node
	#undef graph
	return;


}

// Helper functions

void complete_data(Network& n,Observation& obs, float exp_outcome, int incomplete_node_index, vector<float> table){
	#define node n.Pres_Graph[incomplete_node_index]
	int interval;
	float acc = 0.0;
	for (int i = 0; i < table.size(); i++){
		if (exp_outcome >= acc && exp_outcome < acc + table[i])
			interval = i;
	}

	obs[incomplete_node_index] = node.values[interval];
	return;
	#undef node
}


// for checking asserts
bool is_complete(Observation& obs){
	for (int i = 0; i < obs.size(); i++)
		if (obs[i] == "\"?\"")
			return false;
	return true;
}




// takes a vector<float> and populates it with the probability distribution of node given by node_index, given all other nodes are as given by the observation

void probability_table(Trainer& t,Network& n, int node_index, Observation& obs, vector<float>& table){
	#define node n.Pres_Graph[node_index]

	for (int i = 0; i < node.nvalues; i++){
		string val = node.values[i];
		float logVal = probability(t,n, node_index, val, obs);
		table.push_back(logVal);
	}
	float normalizing_val = -std::accumulate(table.begin(), table.end(), 0);

	for (int i = 0; i < table.size(); i++){
		table[i] = pow(10, normalizing_val + table[i]);
	}

	#undef node
}

// helper method
// returns probability that var takes on value val given the observation
// use the CPTs for calculating this

float probability(Trainer& t,Network& n, int node_index, std::string& val, Observation& obs){
	float _val = 0.0;
	#define node n.Pres_Graph[node_index]


	// compute the val using logs and antilogs
	typedef vector<int>::iterator itr;
	Observation augmented_obs = obs;
	augmented_obs[node_index] =  val;
	int _case =  t.which_case(augmented_obs,node);
	_val += log(node.CPT[_case]);
	itr v_end = node.Children.end();
	for (itr i = node.Children.begin(); i != v_end; i++){ 
	// iterate through the positions of the children

		int child_case = t.which_case(augmented_obs, n.Pres_Graph[*i]);
		_val += log(n.Pres_Graph[*i].CPT[child_case]);
	}


	#undef node
	return _val;
}





/*int main(){
	Trainer trainer;
	read_network(trainer.network, "alarm.bif"); 
	
	trainer.read_data("records.dat");
	EM_complete_data(trainer);
	trainer.write_data();
	trainer.calc_error();

}
*/
#endif
