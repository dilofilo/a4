#ifndef TRAINER_H
#define TRAINER_H
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stack>
#include <algorithm>
#include <functional>
#include "network.h"
/* Each observation is just a sequence of */
typedef  vector<std::string> Observation;

using namespace std;

void remove_quotes(std::string& x) {
	x = x.substr(1 , x.length() - 2);
}

/* I choose you! */
class Trainer {
public:
	Network network; /* contains the CPT. */
	Network gold_network;
	double error;
	vector<Observation> data; 			//All observations.
	std::vector<bool> complete_observation; //tells if the observation is complete or not.
	
	std::vector<std::vector<float> >* cpt[2]; //old and new
	
	std::vector<Observation> working_data;
	std::vector<int> idx_incomplete_obs;
	std::vector<int> incomplete_node;

	Trainer() {
		cpt[0] = new std::vector<std::vector<float> >();
		cpt[1] = new std::vector<std::vector<float> >();
	}
	~Trainer() {
		delete cpt[0];
		delete cpt[1];
	}

	//tells me which data-coordinate in the observation is incomplete.
	int which_node_is_incomplete(Observation& obs) {
		for(int i=0; i<network.netSize(); ++i) {
			if ( obs[i] == "\"?\"") return i;
		} return -1;
	}
	
	void swap_cpts() {
		std::vector<std::vector<float> >* temp = cpt[0];
		cpt[0] = cpt[1];
		cpt[1] = temp;
	}


	void read_data(std::string fname);
	int which_case(Observation& obs , Graph_Node& node);
	int case_offset(Observation& obs, Graph_Node& node); // if obs[node] = 0th value of node, tells me the case. i.e , offset
	void updateAll_CPT_using_observation(Observation& obs);
	void update_count(Graph_Node& n , Observation& obs);
	
	void convertToCPT(Graph_Node&);
	//void probability_given_evidence( Graph_Node node , vector<int>& counts ,double& p); //from data.
	void write_data(std::ostream& o = std::cout);

	/* compute error. */
	void calc_error();


	/* now, for em.	*/

	float probability(int idxNode, int idxValue , Observation& obs); /* this returns P( node{idxNode} = */

	void bulk_em_loop(); //this essentially calls assign incomplete and then generate_newcpt until convergence.
	void bulk_recompute_cpt();
	void assign_value(Observation& obs, int idxNode);
	bool convergence();

};
#endif