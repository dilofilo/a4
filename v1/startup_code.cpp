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
	read_network(trainer.network); //reads alarm.bif
    trainer.read_data("records.dat"); //also does counting.
// Example: to do something
    for ( int i=0; i< trainer.network.Pres_Graph[0].Parents.size() ; ++i ) {
    	cout << trainer.network.Pres_Graph[0].Parents[i] << "\t";
    }
    cout << "\n";
    std::cout << trainer.which_case(trainer.data[3] , trainer.network.Pres_Graph[0] ) << "\n";
	cout<<"Perfect! Hurrah! \n";
	
}





