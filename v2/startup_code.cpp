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
    trainer.calc_error();
    trainer.network.toposort();
	cout<<"Perfect! Hurrah! \n";
	
}





