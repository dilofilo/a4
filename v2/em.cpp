#ifndef EM_CPP
#define EM_CPP


/* a random number generator for convinience. */
#define ACC 1000000
float rng(int accuracy=ACC) {
	return ((rand()%accuracy)/(float)accuracy) ;
}
/* I'd want to change my tolerance. - its a helper, tells me if two numbers converge. */
bool numerical_convergence(float a, float b, float tolerance=0.0001) {
	return (abs(a-b) < tolerance );
}


/* --------------------------------------------------------------------------------------------- */


/* P(X = value{idxValue} | obs) */
float Trainer::probability(int idxNode , int idxValue , Observation& obs) {
	//calculate it from cpt[0].
	std::string original = obs[idxNode];
	obs[idxNode] = network.Pres_Graph[idxNode].values[idxValue];
	float ans= probability_given_parents(idxNode,idxValue,obs);
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
float Trainer::probability_given_parents(int idxNode , int idxValue , Observation& obs , int cpt_source/*=1*/) {
	#define node network.Pres_Graph[idxNode]
	int _case = case_offset(obs, node);
	int valsize = node.CPT.size()/node.nvalues; //size of each block-thingy.
	_case += idxValue*valsize;
	float x = (*cpt[cpt_source])[idxNode][_case];
	#undef node
	return log10(((x<EPSILON)?(EPSILON):(x)));
}
/* assigns value to idxNode of obs. */
void Trainer::assign_value(Observation& obs , int idxNode) {
	int offset = case_offset(obs, network.Pres_Graph[idxNode]);
	int case_size = network.Pres_Graph[idxNode].CPT.size()/network.Pres_Graph[idxNode].nvalues;
	int idx_cpt=offset;
	//I now know which case to use.
	float x = rng(); // range:=(0,1)
	float toincoss = log10(((x<EPSILON)?(EPSILON):(x)));
	//this here can be impoved.	
	vector<float> consideration;
	for(int i=0; i< network.Pres_Graph[idxNode].nvalues;  ++i) {
		consideration.push_back( consideration[consideration.size()-1] + probability( idxNode , i , obs ) );
	}
	for(int i=0; i< network.Pres_Graph[idxNode].nvalues;  ++i) {
		if (toincoss < consideration[i]) {
			obs[idxNode] = network.Pres_Graph[idxNode].values[i];
			return;
		} else {
			idx_cpt += offset;
		}
	}
}
#undef EPSILON

void Trainer::bulk_em_loop() {
	//The working data is not complete, hence, do em on that.
	do {//Assign values to each incomplete data point.
		// the sizes are ok.
		for(int i=0; i<idx_incomplete_obs.size(); ++i) {
			assign_value( working_data[idx_incomplete_obs[i]] , incomplete_node[idx_incomplete_obs[i]]);
		} //assign values to each node.
		bulk_recompute_cpt();
		swap_cpts();
	} while ( !convergence() );
	swap_cpts(1,2); //move the last cpt into the best cpt.
}

/* recompute the cpt using the dataset. put the new cpt into cpt[0] */
void Trainer::bulk_recompute_cpt() {
	//initialize the counts.
	std::vector<std::vector<int> > counts(network.Pres_Graph.size());
	for(int i=0; i<network.Pres_Graph.size(); ++i) {
		counts[i].resize(network.Pres_Graph[i].CPT.size(),-1);
	}
	/* count! */
	for(int i=0; i<working_data.size(); ++i) {
		for(int n=0; n<counts.size(); ++n) {
			int c = which_case(working_data[i] , network.Pres_Graph[n]);
			if (c >= 0) {
				counts[n][c]++;
			} else {
				// cout << " invalid data point found during em\n";	
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
				(*cpt[1])[n][i] = 0.0;
			} else {
				(*cpt[1])[n][i] = counts[n][i]/(float)sums[i%jp];
			}
		}
		//TODO : Smoothen?
	}
}


#define TIME_BUFFER 10
/* in corporate */
bool Trainer::convergence() {
	clocky = clock();
	if ( ((clocky/(float)(CLOCKS_PER_SEC)) > (maxtime - TIME_BUFFER))) {
		return true;
	} else {
		//check for convergence between cpt[0] and cpt[1]
		float inter_cpt_error=0.0;
		float gold_error=0.0;
		for(int n=0; n<network.Pres_Graph.size(); ++n) {
			for(int c=0; c < network.Pres_Graph[n].CPT.size(); ++c) {
				inter_cpt_error += abs( (*cpt[0])[n][c] - (*cpt[1])[n][c] );
				gold_error += abs( (*cpt[0])[n][c] - gold_network.Pres_Graph[n].CPT[c]);
			}
		}
		cout << " error between cpt[0],cpt[1]=" << inter_cpt_error << "\n";
		cout << "error between cpt[1] and golden network =" << gold_error << "\n";
		return false;
	}

}
#endif