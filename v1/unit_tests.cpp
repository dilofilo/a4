#include "trainer.cpp"

int main(){
	Trainer trainer;
	read_network(trainer.network, "alarm.bif"); 
	read_network(trainer.gold_network, "gold_alarm.bif"); 

	trainer.read_data("records.dat");
	// cout << trainer.calc_error() << '\n' ;
	EM2(trainer,1<<30);
	//trainer.write_data();
	cout << trainer.calc_error();

}