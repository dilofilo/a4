#ifndef EM_CPP
#define EM_CPP

const double NEGINF = -10000000000.0;
#define PRECISION 0.0001
const double logPrecision = log10(PRECISION);



bool is_complete(Trainer& t,Observation& obs){
	return (t.which_node_is_incomplete(obs) == -1);
}


/* a random number generator for convinience. */
#define ACC 1000000
double rng(int accuracy=ACC) {
	return ((rand()%accuracy)/(double)accuracy) ;
}
/* I'd want to change my tolerance. - its a helper, tells me if two numbers converge. */
bool numerical_convergence(double a, double b, double tolerance=0.00001) {
	return (abs(a-b) < tolerance );
}

/* returns antilog, base 10*/
double antilog10(double x) {
	return pow(10.0,x);
}



void Trainer::new_EM_loop(){ //doesnt use sub routines! sorry!
	
	vector<vector<double> > working_counts(network.Pres_Graph.size(), vector<double>() );
	
	double tolerance = 0.0001;
	double error = 0.0;
	do{
		error = 0.0;
		for (int i = 0; i < working_counts.size(); i++){
			// working_counts[i] is a vector<double> k;
			for (int j = 0; j < network.Pres_Graph[i].counts.size(); j++){
				working_counts[i].push_back(network.Pres_Graph[i].counts[j]);
			}
		}

		//non-sampling em.
		for(int i=0; i<idx_incomplete_obs.size(); ++i) {
			//ASSERT: idxIncompleteObs[i] is the idx of ith incomplete data point.
			int idxIncompleteNode = incomplete_node[idx_incomplete_obs[i]];

			//ASSERT : idxIncompleteNode != 0
			#define node network.Pres_Graph[idxIncompleteNode]

			int offset = case_offset( data[idx_incomplete_obs[i]] , network.Pres_Graph[idxIncompleteNode]);
			int jp = (node.CPT.size()/node.nvalues);
			for(int v=0; v<node.nvalues; ++v) {
				//For each value, update my counts by a bit.
				
				working_counts[idxIncompleteNode][v*jp + offset] += pow(10.0, probability( idxIncompleteNode , v , data[idx_incomplete_obs[i]]) );
				
			}	

			//ASSERT: Need to recompute CPT[0].
//			cout << "HALLO\n";
			compute_cpt0(working_counts);
			#undef node
//			cout << "HALLO2\n";
			for(int i=0; i<(*cpt[0]).size(); ++i) {
				for(int j=0; j<(*cpt[0])[i].size(); ++j) {
					error = max( error , abs( (*cpt[0])[i][j] - (*cpt[1])[i][j] ));
				}
			}
		}

		// restore working_counts to un initialized values. for all nodes
		for (int i = 0; i < working_counts.size(); i++)
			working_counts[i].clear();
		swap_cpts(0,1);
		print_stats();
	} while ( !numerical_convergence(error,0.0, tolerance));


	// perturb(working_counts);
}

void Trainer::compute_cpt0( std::vector<std::vector<double> >& vec) {
	for(int i=0; i<vec.size(); ++i) {
		//compute sums.
		#define node network.Pres_Graph[i]
		int jp = node.CPT.size()/node.nvalues;
		vector<double> sums(jp,0.0);
		for(int j=0; j<jp; ++j) {
			//sum it up.
			int t = j;
			double sum = 0.0;
			
			while( t < node.CPT.size()) {
				sum += vec[i][t];
				(*cpt[0])[i][t] = vec[i][t];
				t += jp;
			}
			sums[j] = sum;
		}
		//ASSERT : my sums is now ready , cpt0 now contains vec.

		//ASSERT : time to convert cpt0 into probabilities.
		for(int j=0; j<(*cpt[0])[i].size(); ++j) {
			(*cpt[0])[i][j] /= sums[j%jp];
		}
		#undef node
	}
	//ASSERT : cpt0 has been updated to contain new probabilities.
}






/* acc[i] = acc[i-1] + vec[i] forall i, and a[-1]=0. */
void accumulator(std::vector<double>& vec) {
	for(int i=1; i<vec.size(); ++i) {
		vec[i] = vec[i-1] + vec[i];
	}
}

void print(std::vector<double>& vec) {
	cout << "\t(";
	for (int i = 0; i < vec.size(); ++i)
	{
		cout << vec[i] << ",";
	}
	cout << ")\n";
}

void make_usable(std::vector<double>& vec) {
	//I'm given log probabilities.
	std::vector<double> antilogs(vec);
	double total = 0.0;
	for(int i=0; i<antilogs.size(); ++i) {
		antilogs[i] = pow(10.0 , vec[i]);
		total += antilogs[i];
	}
	//ASSERT : We have an unormalized thingy.
	for(int i=0; i<antilogs.size(); ++i) {
		antilogs[i] = antilogs[i]/total;
	}
	//Assert : antilogs[i] contains probabilities, and is not accumulated.
	for(int i=1; i<antilogs.size(); ++i) {
		antilogs[i] = antilogs[i-1] + antilogs[i];
	}
	// ASSERT : it has now been accumulated. Now to put it back into vec.

	for(int i=0; i<antilogs.size(); ++i) {
		vec[i] = antilogs[i]; //log10(antilogs[i]);
	}
	
	assert (numerical_convergence(antilogs[antilogs.size()-1], 1));
	
}
/* --------------------------------------------------------------------------------------------- */


/* alpha * P(X = value{idxValue} | obs) */
double Trainer::probability(int idxNode , int idxValue , Observation& obs) {
	//calculate it from cpt[1].
	std::string original = obs[idxNode];
	obs[idxNode] = network.Pres_Graph[idxNode].values[idxValue];
	
	double ans= probability_given_parents(idxNode,idxValue,obs);
	for(int i=0 ; i<network.Pres_Graph[idxNode].Children.size(); ++i) {
		//the hashdefine is, for the given node, for its child-node, get the int of its value.
		#define idxChildValue network.Pres_Graph[network.Pres_Graph[idxNode].Children[i]].valueToInt[obs[network.Pres_Graph[idxNode].Children[i]]]
		ans += probability_given_parents( network.Pres_Graph[idxNode].Children[i] , idxChildValue , obs);
		#undef idxChildValue
	}
	obs[idxNode] = original;
	return ans;
}

/* take values from cpt */
#define EPSILON 0.000001
double Trainer::probability_given_parents(int idxNode , int idxValue , Observation& obs , int cpt_source/*=1*/) {
	#define node network.Pres_Graph[idxNode]
	int _case = which_case(obs, node);
	double x = (*cpt[cpt_source])[idxNode][_case];
	if ( x==-1 ) {
		return log10(((1.0)/(node.nvalues)));
	}
	if (x>1) {
		cout << " i got this nagging feeling #node=" << idxNode << " #idxvalue=" << idxValue << "\n";
	}
	#undef node
	return ((x==0)?(NEGINF):(log10(x)));
}
/* assigns value to idxNode of obs. */



void Trainer::assign_value_recur(Observation& obs, int idxNode){
	int recur_depth = 0;
	assert (!is_complete(*this, obs));
	while (!is_complete(*this, obs)){
		assign_value(obs, idxNode);
		recur_depth++;
	}

	cout << recur_depth << " ";
	assert (is_complete(*this,obs));
}

void Trainer::assign_value(Observation& obs , int idxNode) {

	double x = rng(); 
	double toincoss = x; //(x==0)?(0):(log10(x));

	vector<double> consideration;
	for(int i=0; i< network.Pres_Graph[idxNode].nvalues; ++i) {
		consideration.push_back(probability( idxNode , i , obs ));
	}                  //consideration contains log probabilities.

	make_usable(consideration);
	

	assert (numerical_convergence(consideration[consideration.size()-1],01));
	

	for(int i=0; i< network.Pres_Graph[idxNode].nvalues;  ++i) {
		if (toincoss <= consideration[i]) {
			obs[idxNode] = network.Pres_Graph[idxNode].values[i];
			return;
		}
	}
	cout << " woopsy \n";
}
#undef EPSILON
double log_likelihood(Trainer& t, int cpt_source =1);


bool Trainer::CPT_invariant_okay(vector<vector<double> >* cpt){
	for (int i = 0; i < cpt->size(); i++) //row of cpt.
	{
		#define node network.Pres_Graph[i]
		int jump = node.CPT.size()/node.nvalues; //they all have the same size.
		vector<double> sums(jump,0);
		for(int j=0; j<node.CPT.size(); ++j) { //column of cpt[i].
			sums[j%jump] += (*cpt)[i][j];
		}
		for(int ki=0; ki<sums.size(); ++ki) {
			if( !numerical_convergence(sums[ki] ,1) ) {
				return false;
			}
		}
		#undef node
	}
	return true;
}

void Trainer::average_out_oscillations(){
	for (int i = 0; i < cpt[0]->size(); i++){
		
		for (int j = 0; j < (*cpt[0])[i].size(); j++){
			(*cpt[0])[i][j] = ((*cpt[0])[i][j] + (*cpt[1])[i][j])/2;
			
		}
		normalize(i);
	}


	assert (CPT_invariant_okay(cpt[0]));
}




bool Trainer::convergence_sums(){
	double error = 0.0;
	for (int i = 0; i < (*cpt[0]).size(); i++){
		for (int j = 0; j < (*cpt[0])[i].size(); j++)
			error += abs((*cpt[0])[i][j] - (*cpt[1])[i][j]);
	}	
	print_stats();
	return numerical_convergence(error,0, 0.05);

}



void Trainer::normalize(int node_index){
	#define node network.Pres_Graph[node_index]
	//int jp = (*cpt[0])[node_index].size()/ node.nvalues; //this is equal to the next.
	int jp = node.CPT.size()/node.nvalues;
	vector<double> sums(jp, 0);
	for (int i = 0; i < jp; i++){
		double sum = 0;
		int t = i;
		while (t < node.CPT.size()){
			sum += (*cpt[0])[node_index][t];
			t += jp;
		}
		sums[i] = sum;
	}

	// sums[i] = \sigma_K cpt[i][k] | K = k/jp
	for(int i=0; i< node.CPT.size(); ++i) {
		(*cpt[0])[node_index][i] /= sums[i%jp];
	}
	
	#undef node
}

void Trainer::print_point(std::vector<string>& v) {
	for(int i=0; i<v.size(); ++i) {
		cout << v[i]<< "," ;
	}
	cout << "\n";
}

void Trainer::bulk_em_loop(bool debug) {
	//The working data is not complete, hence, do em on that.
	int iter=0;
	int num_iterations = 0;
	// cout << "#brk1\n";
	//log_cached[0] = log_cached[1] = log_cached[2] = log_likelihood(*this, 0);
	//double log_likelihood_current;
	// cout << "#brk2\n";
	do {//Assign values to each incomplete data point.
	
		for(int i=0; i<idx_incomplete_obs.size(); ++i) {
			//print_point( working_data[idx_incomplete_obs[i]] );
			assign_value( working_data[idx_incomplete_obs[i]] , incomplete_node[idx_incomplete_obs[i]]);
		}

		bulk_recompute_cpt();

		//average_out_oscillations();

		//log_cached[0] = log_likelihood_current = log_likelihood(*this, 0) ; // log_likelihood_current is the most recent

		//if (log_likelihood_current > log_cached[1]) {}

		swap_cpts(0,1); swap_logs(0,1);
		

		// log_cached[1] is the most recent and the best

		++iter;
		++num_iterations;
		if (iter == 10){
			if (debug)	write_data(std::cout);
			iter = 0;
		}
		// cout << "#brk4\n";
	} while ( !convergence() );
	swap_cpts(1,2); //move the last cpt into the best cpt.
	// cout << "\t-----------------" << '\n';
	// cout << "ITERATIONS: " << num_iterations << '\n';
	fuckyouformatchecker();
}

/* recompute the cpt using the dataset. put the new cpt into cpt[0] */
void Trainer::bulk_recompute_cpt() {
	//initialize the counts.
	std::vector<std::vector<int> > counts(network.Pres_Graph.size());
	for(int i=0; i<network.Pres_Graph.size(); ++i) {
		counts[i].resize(network.Pres_Graph[i].CPT.size(),0); //TODO : Decide b/w 0 and 1.
	}
	/* count! */
	for(int i=0; i<working_data.size(); ++i) {
		for(int n=0; n<counts.size(); ++n) {
			assert (is_complete(*this, working_data[i]));
			int c = which_case(working_data[i] , network.Pres_Graph[n]);
			if (c >= 0) {
				counts[n][c]++;
			}
		}
	}
	for(int n=0; n<counts.size(); ++n) {
		//for each node, normalize.
		#define node network.Pres_Graph[n]
		int jp = node.CPT.size()/node.nvalues;
		vector<int> sums(jp, 0);
		for (int i = 0; i < jp; i++){
			int sum = 0;
			int t = i;
			while (t < counts[n].size()){
				sum += counts[n][t];
				t += jp;
			}
			sums[i] = sum;
		}
		#undef node
		for(int i=0; i<counts[n].size(); ++i) {
			if ( sums[i%jp] == 0 ) {
				(*cpt[0])[n][i] = 0;
			} else {
				(*cpt[0])[n][i] = (counts[n][i]/(double)sums[i%jp]);
			}
		}
	}
}

void Trainer::print_stats(){
	double inter_cpt_error=0.0;
	double gold_error=0.0;
	for(int n=0; n<network.Pres_Graph.size(); ++n) {
		for(int c=0; c < network.Pres_Graph[n].CPT.size(); ++c) {
			inter_cpt_error +=  abs((*cpt[0])[n][c] - (*cpt[1])[n][c]) ;
			gold_error += abs( (*cpt[0])[n][c] - gold_network.Pres_Graph[n].CPT[c]);
		}
	}
	cout << " error between cpt[0],cpt[1]=" << inter_cpt_error << "\n";
	cout << "error between cpt[1] and golden network =" << gold_error << "\n";
}
bool Trainer::convergence_log(){
	double _a = log_cached[1];	
	double _b = log_cached[0]; 
	cout << _a  <<'\n';
	cout << _b << '\n';

	print_stats();
	if (numerical_convergence(_a, _b))
		return true;
	else
		return false;
}

bool Trainer::convergence_max() {
	double error = 0.0; //not -infinitity because positive numbers.
	pair<int,int> which_two = make_pair(0,0);
	for(int r=0; r< (*cpt[0]).size(); ++r) {
		for (int c = 0; c < (*cpt[1])[r].size(); ++c) {
			//error = max( error , abs((*cpt[0])[r][c] - (*cpt[1])[r][c]) );
			double curr_error = abs((*cpt[0])[r][c] - (*cpt[1])[r][c]);
			if (error < curr_error)
				{which_two = make_pair(r,c); error = curr_error;}
		}
	}
	print_stats();
	cout << " max.error=" << error << "\n";
	if (error >= 0.5) cout << which_two.first << " " << which_two.second << '\n';
	return numerical_convergence(error, 0 , 0.05);
}
/* * * * * * * * * * * * * * LOG LIKELIHOOD * * * * * * * * * * * * * * * * */







double probability_data_point(Trainer& t, Observation& obs, int cpt_source = 1){
	assert (is_complete(t,obs));
	double ans = 0.0;
	for (int i = 0; i < obs.size(); i++){
		#define node t.network.Pres_Graph[i]
		int _case = t.which_case(obs, node);
		//(*cpt[cpt_source])[idxNode][_case]
		double _ans = (*t.cpt[cpt_source])[i][_case];
		if (_ans == 0) return 0;
		//assert (_ans != 0);
		if (_ans == -1) ans +=log10(((1.0)/(node.nvalues)));
		 else ans += log10( (_ans)) ;
		#undef node
	}

	return ans;
}

void print(Observation& obs){
	
	for (int i = 0; i < obs.size(); i++)
		cout << obs[i] << " " ;
	cout << '\n';
}	

double probability_data_point_incomplete(Trainer& t, Observation& obs, int incomplete_node_index, int cpt_source = 1){
	
	assert (!is_complete(t,obs));
	double ans = 0.0;
	#define node t.network.Pres_Graph[incomplete_node_index]
	for (int i = 0; i < node.nvalues; i++){
		Observation aux_obs = obs;
		aux_obs[incomplete_node_index] = node.values[i];
		ans += pow(10.0,probability_data_point(t, aux_obs, cpt_source) );
	}

	return log10(ans);
	#undef node
}

double log_likelihood(Trainer& t, int cpt_source){
	#define DATA t.data
	double val = 0.0;
	for (int i = 0; i < DATA.size(); i++){
		if (!is_complete(t,DATA[i]))
		{

			int incomplete_node_index  = t.which_node_is_incomplete(DATA[i]);
			val += probability_data_point_incomplete(t,DATA[i], incomplete_node_index, cpt_source);
			//cout << val << '\n';
		}

		else{
			val += probability_data_point(t, DATA[i], cpt_source);
		}
	}

	#undef DATA

	return val;
}






//////////////////////////////////////////////////////////////////////////////



#define TIME_BUFFER 10
/* in corporate */
bool Trainer::convergence() {
	clocky = clock();
	if ( ((clocky/(double)(CLOCKS_PER_SEC)) > (maxtime - TIME_BUFFER))) {
		return true;
	} else if ( true ) {
		//print_stats();
		return false;
	} else {
		//check for convergence between cpt[0] and cpt[1]
		double inter_cpt_error=0.0;
		double gold_error=0.0;
		for(int n=0; n<network.Pres_Graph.size(); ++n) {
			for(int c=0; c < network.Pres_Graph[n].CPT.size(); ++c) {
				inter_cpt_error +=  abs((*cpt[0])[n][c] - (*cpt[1])[n][c]) ;
				gold_error += abs( (*cpt[0])[n][c] - gold_network.Pres_Graph[n].CPT[c]);
			}
		}
		cout << " error between cpt[0],cpt[1]=" << inter_cpt_error << "\n";
		cout << "error between cpt[1] and golden network =" << gold_error << "\n";
		return false;
	}
}
#endif