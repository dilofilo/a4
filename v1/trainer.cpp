#ifndef TRAINER_CPP
#define TRAINER_CPP

#include "trainer.h"

//typedef  vector<std::string> Observation;

void Trainer::read_data(std::string filename) {
	fstream datfile; //included in the header file.
	datfile.open(filename.c_str() , ios::in);
	string obs_line;
	int lines_read=0;
	bool complete_value=true;
	while( !datfile.eof() ) {
		//Read one line, parse it.

		getline(datfile , obs_line);
		++lines_read;
		stringstream temp(obs_line);
		data.push_back( Observation() );

		string var_value;
		while(std::getline(temp, var_value , ' ')) {
			if (var_value=="?") {
				complete_value = false;
			}
			data[lines_read-1].push_back(var_value);
		}
		obs_line = "";
		complete_observation.push_back(complete_value);
	
	}
	datfile.close();
	return;
}

/* returns the index in the cpt to which this case(i.e, obs) belongs */
int Trainer::which_case( Observation& obs , Graph_Node& node) {// , vector< vector<int> >& counts ,vector<double>& p)
	//current node index = idx_in_network.
	int temp;
	if ( obs[node.idx_in_network] == "?") {
		return -1;
	} else {
		temp = node.valueToInt[ obs[node.idx_in_network] ];
	}

	// _case *= (node.valueToInt[obs[node.idx_in_network]]*node.nvalues);
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

void Trainer::update_counts(Graph_Node& n) {
	for(int i =0; i<data.size(); ++i) {
		int cas = which_case(data[i] , n);
		if (cas >= 0) {
			n.counts[cas] += 1;	
		}		
	}
}

void Trainer::write_data() {
	return;
}
#endif