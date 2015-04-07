#ifndef TRAINER_CPP
#define TRAINER_CPP

#include "trainer.h"

typedef std::vector<float> CPT;
void probability_table(Trainer& ,Network& n,int node_index, Observation& obs, std::vector<float>& cpt);
float probability(Trainer&,Network&,int node_index, std::string& val, Observation& obs);
void complete_data(Network&,Observation& obs,float val ,int node_index, vector<float>& table);
void EM(Trainer& t,Network& a, Observation&  obs);
//	complete_data(a,obs, val, incomplete_node_index ,table);


void Trainer::read_data(std::string filename) {
	fstream datfile; //included in the header file.
	datfile.open(filename.c_str() , ios::in);
	string obs_line;
	int lines_read=0;
	bool complete_value=true;
	while( !datfile.eof() ) {
		//Read one line, parse it.
		complete_value = true;
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
		if (sums[i % jp] == 0) {
			n.CPT[i] = 0.0;
		}
		else {
			n.CPT[i] = ((n.counts[i])/((double)(sums[i % jp])));
		}
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


float Trainer::calc_error() {
	error=0.0;
	for(int i=0; i< network.Pres_Graph.size(); ++i) {
		for(int c=0; c< network.Pres_Graph[i].CPT.size(); ++c) {
			error += abs( network.Pres_Graph[i].CPT[c] - gold_network.Pres_Graph[i].CPT[c]) ;
		}
	}
	return error;
}


/*-------------------------------------------EM-----------------------------------*/


#define ACC 100000000
float rng(int accuracy = ACC) {
	return ((rand()%accuracy)/(float)accuracy);
}


// Helper functions

void complete_data(Network& n,Observation& obs, float exp_outcome, int incomplete_node_index, vector<float>& table){
	#define node n.Pres_Graph[incomplete_node_index]
	int interval;
	float acc = 0.0;
	for (int i = 0; i < table.size(); i++){
		if (exp_outcome >= acc && exp_outcome < acc + table[i]) {
			interval = i;
			break;
		}
		acc += table[i];
	}

	obs[incomplete_node_index] = node.values[interval];
	return;
	#undef node
}


// for checking asserts
bool is_complete(Observation& obs){
	for (int i = 0; i < obs.size(); i++) {
		if (obs[i] == "\"?\""){
			return false;
		}
	}
	return true;
}



bool numerical_conv(float& a, float& b, float tolerance = 0.0001){
	return abs(a-b) < tolerance;
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
				//for (int j = 0; j < DATA[i].size(); j++) cout << DATA[i][j] << ",";
				// cout << '\n';
				t.complete_observation[i] = true;
			}

	}

	cout << "DONE!"<< '\n';
	#undef DATA	
	#undef a
}

void undo_obs(Trainer& t, Observation& obs, int node_index);
void recompute(Trainer&);
void EM2(Trainer& t,int n) {
	#define DATA t.data
	#define a t.network
	//sort_incomplete_data(a,DATA);
	 //depends on node's cpt being updated.
	std::vector<float> table;
	std::vector<pair<int, int> > incomplete_node_indices;
	for (int i = 0; i < DATA.size(); i++){
		if (!is_complete(DATA[i])){
			
			int incomplete_node_index = incomplete_node(DATA[i]);
			
			assert( incomplete_node_index != -1 );
			
			probability_table(t,a,incomplete_node_index,DATA[i], table); //bloats table
			
			float val = rng(); //belongs to 0,1. 
			complete_data(a,DATA[i], val, incomplete_node_index, table);
			
			incomplete_node_indices.push_back(make_pair(i,incomplete_node_index));
			
			table.clear(); //unbloats table.
		}
	}
	recompute(t);
	
	if (n == 0){return;}
	else{
		for (int i = 0; i < incomplete_node_indices.size(); i++) {
			undo_obs(t, DATA[incomplete_node_indices[i].first], incomplete_node_indices[i].second);
		}
		cout << "ERROR: " << t.calc_error() << '\n';
		EM2(t, n-1);
	}

	#undef DATA
	#undef a
}

void recompute(Trainer& t){
	#define DATA t.data
	#define NET t.network
	std::vector<std::vector<int> > counts(NET.Pres_Graph.size());
	for(int i=0; i<NET.Pres_Graph.size(); ++i) {
		counts[i].resize(NET.Pres_Graph[i].CPT.size(),0); 
	}
	for (int i = 0; i < DATA.size(); i++){
		assert (is_complete(DATA[i]));
		for (int j = 0; j < NET.Pres_Graph.size(); j++)
		{
			int _case = t.which_case(DATA[i],NET.Pres_Graph[j]);
			if (_case > 0)
				counts[j][_case] +=1;
		}
	}

	for (int n = 0; n < counts.size(); n++){
		#define node NET.Pres_Graph[n]
		int jp = node.CPT.size()/node.nvalues;
		vector<int> sums(jp, 0);
		for (int i = 0; i < jp; i++){
			int sum = 0;
			int t2 = i;
			while (t2 < counts[n].size()){
				sum+= counts[n][t2];
				t2+=jp;
			}
			sums[i] = sum;
		}

		for (int i = 0; i < counts[n].size(); i++){
			if (sums[i % jp] == 0) {
				node.CPT[i] = 0.0;
			}
			else {
				
				node.CPT[i] = ((counts[n][i])/((float)(sums[i % jp])));
				
			}

		}
		float aux_sum = 0;
		for (int i = 0; i < node.CPT.size(); i++) aux_sum += node.CPT[i];
		cout << aux_sum << '\n';
		

		#undef node
	}



	#undef DATA
	#undef network
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
		{
			cout << "Error so far:" << t.calc_error() << '\n';

			return;
		}
	else{
		undo_obs(t, obs, incomplete_node_index);
		EM(t, a, obs);
	}
}


void undo_obs(Trainer& t, Observation& obs, int node_index){
	
	
	obs[node_index] = "\"?\"";

}

void maximization(Trainer& t, Observation& obs, int node_index){
	#define node t.network.Pres_Graph[node_index]
	#define graph t.network.Pres_Graph
	
	assert( is_complete(obs) );

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





// takes a vector<float> and populates it with the probability distribution of node given by node_index, given all other nodes are as given by the observation

void probability_table(Trainer& t,Network& n, int node_index, Observation& obs, vector<float>& table){
	#define node n.Pres_Graph[node_index]

	for (int i = 0; i < node.nvalues; i++){
		string val = node.values[i];
		float probval = probability(t,n, node_index, val, obs);
		//ASSERT : probal belongs to (0,1).
		table.push_back(probval);
		assert(probval >= 0);
	}
	float normalizing_val=0.0;
	for(int i=0; i<table.size(); ++i) {
		normalizing_val += table[i];
	}

	assert (normalizing_val != 0);
	//ASSERT: normalizing val approximates to 1.
	for (int i = 0; i < table.size(); i++) {
		table[i] = (table[i]/normalizing_val);
		assert (table[i]<=1.0); 
		assert (table[i]>=0.0);
	}

	#undef node
}

// helper method
// returns probability that var takes on value val given the observation
// use the CPTs for calculating this

float probability(Trainer& t,Network& n, int node_index, std::string& val, Observation& obs){
	assert( obs[node_index] == "\"?\"");
	float _val = 0.0;
	#define node n.Pres_Graph[node_index]

	// compute the val using logs and antilogs
	typedef vector<int>::iterator itr;
	Observation augmented_obs = obs;
	augmented_obs[node_index] =  val;
	int _case =  t.which_case(augmented_obs,node);
	_val += log10(node.CPT[_case]);
	itr v_end = node.Children.end();
	for (itr i = node.Children.begin(); i != v_end; i++){ 
	// iterate through the positions of the children

		int child_case = t.which_case(augmented_obs, n.Pres_Graph[*i]);
		if (n.Pres_Graph[*i].CPT[child_case] == 0) {
			return 0;
		} else {
			_val += log10(n.Pres_Graph[*i].CPT[child_case]);	
		}
		assert(n.Pres_Graph[*i].CPT[child_case] > 0);
	}
	#undef node
	//cout << pow(10.0, _val) << '\n';
	//assert( pow(10.0,_val) >= 0 );
	return pow(10.0,_val);
}




/**/

#endif