#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <unordered_map>

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

#include "trainer.cpp"

int main(int argc, char** argv)
{
	//alarm.bif file and dataset file.
	Trainer trainer;
	read_network(trainer.network, "alarm.bif"); //reads alarm.bif
    read_network(trainer.gold_network, "gold_alarm.bif"); //reads alarm.bif
    trainer.read_data("records.dat"); //also does counting.
    
    for(int i=0; i< trainer.network.netSize(); ++i) {
        for(int j=0; j<trainer.network.Pres_Graph[i].CPT.size() ; ++j) {
            cout << (*trainer.cpt[2])[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "done somehow \n";
    trainer.bulk_em_loop();
    cout<<"Perfect! Hurrah! \n";
}





