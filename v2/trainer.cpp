#ifndef TRAINER_CPP
#define TRAINER_CPP

#include "trainer.h"
#include <cassert>

void Trainer::calc_error() {
	for(int i=0; i< network.Pres_Graph.size(); ++i) {
		for(int c=0; c< network.Pres_Graph[i].CPT.size(); ++c) {
			error += abs( network.Pres_Graph[i].CPT[c] - gold_network.Pres_Graph[i].CPT[c]) ;
		}
	}
}

/* returns the index in the cpt to which this case(i.e, obs) belongs */
int Trainer::which_case( Observation& obs , Graph_Node& node) {
	int temp;
	if ( obs[node.idx_in_network] == "\"?\"") {
		return -1;
	} else {
		temp = node.valueToInt[ obs[node.idx_in_network] ];
	}
	int _case = 0;
	int acc=1;
	int digit;
	string r;
	for ( int i= node.Parents.size()-1; i >= 0; i--) {
		if( ( r = obs[ network.name_to_idx[node.Parents[i]] ] )== "?" ) {
			return -1;
		}
		Graph_Node* tempptr = &network.Pres_Graph[ network.name_to_idx[node.Parents[i]] ];
		digit = acc*(tempptr->valueToInt[ r ]);
		_case += digit;
		acc*= tempptr->nvalues;
		tempptr = NULL;
	}
	_case += acc*temp;
	return _case;
}
/*if obs[node] = 0th value of node, tells me the case. i.e , offset*/

// NOT-DEPRECATED FUNCTION.  USE
int Trainer::case_offset(Observation& obs, Graph_Node& node) {
	int _case = 0;
	int acc=1;
	int digit;
	string r;
	for ( int i= node.Parents.size()-1; i >= 0; i--) {
		if( ( r = obs[ network.name_to_idx[node.Parents[i]] ] )== "?" ) {
			return -1;
		}
		Graph_Node* tempptr = &network.Pres_Graph[ network.name_to_idx[node.Parents[i]] ];
		digit = acc*(tempptr->valueToInt[ r ]);
		_case += digit;
		acc*= tempptr->nvalues;
		tempptr = NULL;
	}
	return _case;
}

void Trainer::updateAll_CPT_using_observation(Observation& obs){
	for (int i = 0; i < network.Pres_Graph.size(); i++) {
		update_count(network.Pres_Graph[i], obs);
	}
}

void Trainer::update_count(Graph_Node& n, Observation& obs) {
	int cas = which_case(obs , n);
	if (cas >= 0) {
		n.counts[cas] += 1;
	}
}


void Trainer::convertToCPT(Graph_Node& n){ //converts counts to 
	int jp = n.CPT.size()/n.nvalues;
	vector<int> sums(jp, 0);
	for (int i = 0; i < jp; i++){
		int sum = 0;
		int t = i;
		while (t < n.counts.size()){
			sum += n.counts[t];
			t+= jp;
		}
		sums[i] = sum;
	}
	for (int i = 0; i < n.counts.size(); i++){
		if (sums[i % jp] == 0) {
			n.CPT[i] = 0.0;
		} else {
			n.CPT[i] = n.counts[i]/double(sums[i % jp]);
		}	
	}
}

/* IO FUNCTIONS FOLLOW. */

void Trainer::initialize_cpts() {
	(*cpt[0]).resize(network.Pres_Graph.size());
	(*cpt[1]).resize(network.Pres_Graph.size());
	(*cpt[2]).resize(network.Pres_Graph.size());
	for (int i=0; i<  network.Pres_Graph.size(); ++i) {
		for(int j=0; j<network.Pres_Graph[i].CPT.size(); ++j) {
			(*cpt[0])[i].push_back(network.Pres_Graph[i].CPT[j]);
			(*cpt[1])[i].push_back(network.Pres_Graph[i].CPT[j]);
			(*cpt[2])[i].push_back(network.Pres_Graph[i].CPT[j]);
		}
	}
}

/* reads raw data and puts it into member field called data. */
void Trainer::read_data(std::string filename) {
	fstream datfile; //included in the header file.
	datfile.open(filename.c_str() , ios::in);
	string obs_line;
	int lines_read=0;
	bool complete_datapoint=true;
	
	while( !datfile.eof() ) {
		complete_datapoint=true; /* reset it for each line. */
		getline(datfile , obs_line);
		++lines_read;
		stringstream temp(obs_line);
		
		data.push_back( Observation() );
		working_data.push_back( Observation() );

		string var_value;
		int tempctr=0;
		while(std::getline(temp, var_value , ' ')) {
			if ( var_value == "") {
				continue;
			}
			if (var_value=="\"?\"") {
				complete_datapoint = false;
				incomplete_node.push_back(tempctr);
				idx_incomplete_obs.push_back(lines_read-1);
			}
			++tempctr;

			data[lines_read-1].push_back(var_value);
			working_data[lines_read-1].push_back(var_value);
		
		}
		if ( complete_datapoint ) {
			incomplete_node.push_back(-1);
		}
		#define obs data[lines_read-1]
		updateAll_CPT_using_observation(obs);
		#undef obs
		obs_line = "";
		complete_observation.push_back(complete_datapoint);	
	}
	for(int i=0; i<network.netSize(); ++i) {
		convertToCPT(network.Pres_Graph[i]);
	}
	datfile.close();
	initialize_cpts();
	return;
}

double truncate(double x) {
	int ans = x*10000;
	double actans = ans/10000.0;
	return actans;
}

void Trainer::write_data(std::ostream& o, int cpt_source  /*= std::cout*/) {
	o << "network Alarm {\n}\n"; //first line.
	for(int i=0; i<network.Pres_Graph.size();++i) {
		o << "variable " << network.Pres_Graph[i].Node_Name << " {\n";
		o << "\ttype discrete[" << network.Pres_Graph[i].nvalues << "] {";
		for(int j=0; j<network.Pres_Graph[i].values.size(); ++j) {
			o << network.Pres_Graph[i].values[j] << " ";
		} o << "};\n}\n";
	}
	//For all the probabilities.
	for(int i=0; i<network.Pres_Graph.size(); ++i) {
		o << "probability ( " << network.Pres_Graph[i].Node_Name;
		for(int j=0; j<network.Pres_Graph[i].Parents.size(); ++j) {
			o << " " << network.Pres_Graph[i].Parents[j] << " ";
		} o << " ) {\n \t table ";
		for(int j=0; j<network.Pres_Graph[i].CPT.size(); ++j) {
			o << (*cpt[cpt_source])[i][j] << " "; //prints out the best cpt evah
		} o << ";\n}\n";
	}
}

void Trainer::fuckyouformatchecker() {
	fstream i,o;
}

#include "em.cpp"

#endif