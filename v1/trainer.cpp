#ifndef TRAINER_CPP
#define TRAINER_CPP

#include "trainer.h"


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
			if (var_value=="\"?\"") {
				complete_value = false;
			}
			data[lines_read-1].push_back(var_value);
		}
		#define obs data[lines_read-1]
		// cout << "#brk2\n";
		updateAll(obs);
		// cout << "#brk3\n";
		#undef obs

		obs_line = "";
		complete_observation.push_back(complete_value);
	
	}
	for(int i=0; i<network.netSize(); ++i) {
		convertToCPT(network.Pres_Graph[i]);
	}
	datfile.close();
	return;
}

/* returns the index in the cpt to which this case(i.e, obs) belongs */
int Trainer::which_case( Observation& obs , Graph_Node& node) {
	//current node index = idx_in_network.
	int temp;
	if ( obs[node.idx_in_network] == "\"?\"") {
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


void Trainer::updateAll(Observation& obs){
	// cout << "#brk2.5 , updateall() in\n";
	for (int i = 0; i < network.Pres_Graph.size(); i++) {
		// cout << "#brk2.5 , updateall() loop in\n";
		update_count(network.Pres_Graph[i], obs);
		// cout << "#brk2.5 , updateall() loop out\n";
	}
	// cout << "#brk2.5 , updateall() out\n";
}

void Trainer::convertToCPT(Graph_Node& n){
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
		if (sums[i % jp] == 0) 
			n.CPT[i] = 0.0;
		else
			n.CPT[i] = n.counts[i]/double(sums[i % jp]);
	}
}


vector<vector<float> > Trainer::CPT(){
	#define graph network.Pres_Graph
	vector<vector<float> > CPT_table;
	for (int i = 0; i < graph.size(); i++){
		CPT_table.push_back(graph[i].CPT);
	}


	#undef graph
	return CPT_table;
}

void Trainer::update_count(Graph_Node& n, Observation& obs) {
	// cout << "#brk2.75 , updatecount() in\n";
	int cas = which_case(obs , n);
	// cout << "#brk2.75 , updatecount() after which case\n";
	if (cas >= 0) {
		n.counts[cas] += 1;
	}
	// cout << "#brk2.75 , updatecount() out\n";
}

void Trainer::write_data() {
	std::cout << "total error=" << error << "\n";
	for (int i = 0; i < network.Pres_Graph.size(); i++)
		network.Pres_Graph[i].print(std::cout);

	
}


void Trainer::calc_error() {
	
	for(int i=0; i< network.Pres_Graph.size(); ++i) {
		for(int c=0; c< network.Pres_Graph[i].CPT.size(); ++c) {
			error += abs( network.Pres_Graph[i].CPT[c] - gold_network.Pres_Graph[i].CPT[c]) ;
		}
	}
}
#endif