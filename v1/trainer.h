#ifndef TRAINER_H
#define TRAINER_H
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "network.h"
/* Each observation is just a sequence of */
typedef  vector<std::string> Observation;

void remove_quotes(std::string& x) {
	x = x.substr(1 , x.length() - 2);
}
/* I choose you! */
class Trainer {
public:
	Network network; /* contains the CPT. */
	vector<Observation> data; 			//All observations.
	std::vector<bool> complete_observation; //tells if the observation is complete or not.

	Trainer() {}
	~Trainer() {}
	/* I/O functions. */
	void read_data(std::string fname);
	int which_case(Observation& obs , Graph_Node& node);
	void update_counts(Graph_Node& n);
	//void probability_given_evidence( Graph_Node node , vector<int>& counts ,double& p); //from data.
	void write_data();
};
#endif