#ifndef AXON_H
#define AXON_H

class Neuron;
class Axon{
	friend class Neuron;
	public:
		Axon(double cd);
		void mutate(double rate);
		double get_cond();
		void set_cond(double);
		void add_conn(Neuron &);
		Neuron* get_conn();
		bool isconn();
	protected:
		double cond; //conductivity
		Neuron* connection;

};
Axon::Axon(double cd){
	cond=cd;
	connection=0;
}
void Axon::set_cond(double c){
	cond=c;
}
void Axon::mutate(double rate){
	cond+=rate;
}
double Axon::get_cond(){
	return cond;
}
void Axon::add_conn(Neuron &n){
	Neuron* np=&n;
	connection=np;
}
Neuron* Axon::get_conn(){
	return connection;
}
bool Axon::isconn(){
	return connection!=0;
}
#endif
