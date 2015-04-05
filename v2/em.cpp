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


void Trainer::assign_value(Observation& obs , int idxNode) {
	int offset = case_offset(obs, network.Pres_Graph[idxNode]);
	int case_size = network.Pres_Graph[idxNode].CPT.size()/network.Pres_Graph[idxNode].nvalues;
	int idx_cpt=offset;
	//I now know which case to use.
	float toincoss = rng();

	for(int i=0; i< network.Pres_Graph[idxNode].nvalues;  ++i) {
		if (toincoss < probability( idxNode , i , obs )) {
			obs[idxNode] = network.Pres_Graph[idxNode].values[i];
			return;
		} else {
			idx_cpt += offset;
		}
	}
}

void Trainer::bulk_em_loop() {
	//The working data is not complete.
	do {//Assign values to each incomplete data point.
		for(int i=0; i<idx_incomplete_obs.size(); ++i) {
			assign_value( working_data[idx_incomplete_obs[i]] , incomplete_node[idx_incomplete_obs[i]]);
		}
		bulk_recompute_cpt();
		swap_cpts();
	} while ( !convergence() );
}
/* P(X = w{i} | obs) */
float Trainer::probability(int idxNode , int idxValue , Observation& obs) {
	//calculate it from cpt[0].
}

/* recompute the cpt using the dataset. */
void Trainer::bulk_recompute_cpt() {

}

/* in corporate */
bool Trainer::convergence() {
	return false;
}
#endif