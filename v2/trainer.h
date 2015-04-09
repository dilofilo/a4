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
#include <stdio.h>
#include <time.h>
#include <cmath>
//YO!
#define ATLEAST_ONE_COUNT 1

#include "network.h"

/* Each observation is just a sequence of strings */
typedef  vector<std::string> Observation;
#define TEN_MINUTES 600
#define FIVE_MINUTES 300
#define TWO_MINUTES 120
#define SIXTY_SECONDS 60
using namespace std;

void remove_quotes(std::string& x) {
	x = x.substr(1 , x.length() - 2);
}

/* I choose you! */
class Trainer {
public:
	/* for timing purposes. */
	clock_t clocky;
	double maxtime;
	string outfilename , inputfilename ;
	/* input network. */
	Network network;
	/*for debugging purposes*/
	Network gold_network; 
	double error;


	/* TO SPEED UP LOG LIKELIHOOD CALCULATIONS, CACHE THE MOST RECENT COMPUTATION*/
	double log_cached[3]; // log_cached[0] is the most recent, log_cached[1] is the last, log_cached[2] is the best ever
	/* input data*/
	vector<Observation> data; 			//All observations.
	std::vector<bool> complete_observation; //tells if the observation is complete or not.
	
	/* cpt tables to be constructed...*/
	std::vector<std::vector<double> >* cpt[3];//the one construct in em, and the last one (where i derive my assigns from) and best yet(if restarts)
	//NOTE : To access cpt, use (*cpt[0,1,2])[nodeIDX][case] its an array of pointers.


	/* data that EM will work on. */
	std::vector<Observation> working_data;
	std::vector<int> idx_incomplete_obs; //Which data-points' are incomplete? saved by their index.
	std::vector<int> incomplete_node; //In each datapoint, which node is the incomplete node? -1 if 

	bool data_okay(){
		for (int i = 0; i < idx_incomplete_obs.size(); i++){
			int idx = idx_incomplete_obs[i];
			if (incomplete_node[i] == -1)
				return false;
		}

		return true;
	}

	Trainer() { 		/* set seed. start the time, prevent segfaults, and go. */
		srand(time(NULL));
		outfilename = "";
		inputfilename = "";
		clocky = clock();
		maxtime = TWO_MINUTES; //10 minutes.
		cpt[0] = new std::vector<std::vector<double> >();
		cpt[1] = new std::vector<std::vector<double> >();
		cpt[2] = new std::vector<std::vector<double> >(); //stores the best so far.
	}
	~Trainer() {/* cleanup behind ourselves. we aint dogs bruh. */
		delete cpt[0];
		delete cpt[1];
		delete cpt[2];
	}
	/* now for miscellaneous functions. */
	void calc_error();
	int which_case(Observation& obs , Graph_Node& node); //tells me which thing of the nodes's cpt the observation counts for.
	int case_offset(Observation& obs, Graph_Node& node); // if obs[node] = 0th value of node, tells me the case. i.e , offset
	int which_node_is_incomplete(Observation& obs) {
		for(int i=0; i<network.netSize(); ++i) {
			if ( obs[i] == "\"?\"") return i;
		} return -1;
	}
	void swap_cpts(int i=0, int j=1) { //swaps the ith and jth cpts.
		std::vector<std::vector<double> >* temp = cpt[i];
		cpt[i] = cpt[j];
		cpt[j] = temp;
	}
	bool CPT_invariant_okay(std::vector<std::vector<double> >* cptptr);


	/* the next three functions are used in the very first time to count things. */
	void updateAll_CPT_using_observation(Observation& obs); //register the obs as a count in all the nodes.
	void update_count(Graph_Node& n , Observation& obs); //register the obs as a count in the node n.
	void convertToCPT(Graph_Node&); //convert the counts to CPT for each node. Also sets cpt[2]

	void assign_value_recur(Observation& obs, int idxNode); // outermost recursive call to assign_value. 
	// assert that obs is complete before returning from the function

	/* I/O Functions */
	void initialize_cpts();
	void read_data(std::string fname); //cpt[0,1,2] are initialized here.
	void write_data(std::ostream& o = std::cout, int cpt_source = 2);


	/* now, for em.	*/
	double probability(int idxNode, int idxValue , Observation& obs); /* this returns log10(P( node{idxNode} ) */
	double probability_given_parents(int idxNode, int idxValue , Observation& obs , int cpt_source=1);
	/* bulk functions */
	void bulk_em_loop(bool debug = false); //this essentially calls assign incomplete and then generate_newcpt until convergence.
	void bulk_recompute_cpt();
	/**/
	void assign_value(Observation& obs, int idxNode);
	bool convergence(); //between cpt[0] and cpt[1]

	void average_out_oscillations();
	void normalize(int node_index);
	void swap_logs(int i, int j){
		double temp = log_cached[i];
		log_cached[i] = log_cached[j];
		log_cached[j] = temp;
	} 

	bool convergence_log(); 
	bool convergence_sums();
	bool convergence_max();
	void print_stats();
	void print_point(std::vector<string>& v);
	/* another em... */
	void new_EM_loop();
	void compute_cpt0(std::vector<std::vector<double> >& v);
	void fuckyouformatchecker()
};
#endif