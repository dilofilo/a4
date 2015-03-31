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

int main()
{
	Trainer trainer;
	read_network(trainer.alarm); //reads alarm.bif
    
// Example: to do something
	cout<<"Perfect! Hurrah! \n";
	
}





