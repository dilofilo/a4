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
     string netfilename(argv[1]);
     string datfilename(argv[2]);
     trainer.outfilename = "solved_" + netfilename;
     trainer.inputfilename = netfilename;
	   read_network(trainer.network, netfilename); //reads alarm.bif

    //read_network(trainer.gold_network, "gold_alarm.bif"); //reads alarm.bif
    trainer.read_data(datfilename); //also does counting.
    cout << "data is in order" << '\n';
    trainer.bulk_em_loop();
    //trainer.new_EM_loop();
    cout<<"Perfect! Hurrah! \n";
    cout << "TIME TAKEN: " ;
    cout << ((trainer.clocky - clock())/(double)CLOCKS_PER_SEC) << '\n';
}





