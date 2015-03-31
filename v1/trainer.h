#ifndef TRAINER_H
#define TRAINER_H
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "network.h"
/* Each observation is just a sequence of */
typedef  vector<std::string> Observation;

/* I choose you! */
class Trainer {
public:
	Network alarm; /* contains the CPT. */
	vector<Observation> data;
	std::vector<bool> incomplete_data;

	Trainer() {}
	~Trainer() {}
	/* I/O functions. */
	void read_data();
	void write_data();
};
#endif