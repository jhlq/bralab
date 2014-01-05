#ifndef NEURON_H
#define NEURON_H

#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include "Function.h"
#include "Axon.h"
#include "TanhFunction.h"
#include "TailFunction.h"
#include "LinearFunction.h"

class Neuron{
	friend class Brain;
	public:	
		Neuron(double threshold=0,bool bias=0);
		void excite(double);
		double vibrate();
		double get_exc();
		void set_exc(double);
		void connect(Neuron &,double cd=0);
		void grow_ax(double);
		void connect2L(std::vector<Neuron> &,double cd=0); 
		virtual void print();
		void scramble();
		void norm();
		void mutate_axon(int,double);
		std::vector<Axon>::iterator get_ax(int);
		std::vector<Axon>& get_axv(){ return _axl; }
		void set_thresh(double);
		double get_thresh();
		void reverseAxons(); //change signs
		void mutate(double mrate);
		void set_trfnc(Function* f){ _trfnc=f; }
		void set_tail(double t){ _trfnc->set_tail(t); }
		double get_tail(){ return _trfnc->get_tail(); }
		void set_bias(bool b){_bias=b;}
		void set_leak(double l){_leak=l;}
		double get_leak(){ return _leak;}
		void set_habituation(double h){_habituation=h;}
		double get_habituation(){return _habituation;}
		
		Neuron* bufferneuron;
		
	protected:
		double _exc;
		double pre_exc;
		Function *_trfnc;
		double _thresh;
		std::vector<Axon> _axl;
		bool _bias;
		double _leak;
		double _habituation;
		double _habfactor;
		
};

Neuron::Neuron(double threshold,bool bias) {
	_exc=0;
	pre_exc=0;
	_thresh=threshold;	
	_trfnc=new TailFunction(1,1);
	_bias=bias;
	_leak=0;
	_habituation=0;
	_habfactor=9;
}
void Neuron::connect2L(std::vector<Neuron> &nl,double cd){
	for (std::vector<Neuron>::iterator itn=nl.begin(); itn!=nl.end(); ++itn){
		connect(*itn,cd);
	}
}
void Neuron::connect(Neuron &n,double cd){
	grow_ax(cd);
	std::vector<Axon>::iterator ita;
	ita=_axl.end();
	--ita;
	ita->add_conn(n);
}
std::vector<Axon>::iterator Neuron::get_ax(int na){
	std::vector<Axon>::iterator ait;
	if (_axl.empty()){
		std::cout<<" No axons in neuron with leak: "<<_leak<<". ";
		return ait;
	}
	if (na==-1){
		ait=_axl.end();
		--ait;
	}else {
		
		ait=_axl.begin()+na;
	}
	return ait;
}
void Neuron::grow_ax(double cd){
	Axon nax=Axon(cd);
	_axl.push_back(nax);
}
void Neuron::excite(double ex){
	_exc+=ex;
}
double Neuron::vibrate(){
	if (_bias) { return 1; }

	double __exc;

	if (_exc==0){
		__exc=0;
	}else{
		__exc=_exc-_habituation*_exc/exp(_habfactor*(std::abs((_exc-pre_exc)/_exc)));
	}
	__exc=__exc-_leak*__exc;
	if (_thresh>=0){	
		if (std::abs(__exc)<=_thresh){
			bufferneuron->_exc+=__exc;
			return 0;
		}
	} else {
		if (std::abs(__exc)>-_thresh){
			bufferneuron->_exc+=__exc;
			return 0;
		}
	}
		
	
	pre_exc=_exc;
	_exc=0;
	std::vector<Axon>::iterator ait;
	double output=_trfnc->transform(__exc);
	for (ait=_axl.begin(); ait!=_axl.end(); ++ait){
		if (ait->connection==0){
			std::cout<<" No connection in axon with conductivity: "<<ait->cond<<". ";;
			continue;

		}
		Neuron* nP=ait->connection;
		nP->excite((ait->cond)*output);		
	}	
	return output;
} 
void Neuron::print(){
	std::vector<Axon>::iterator it;
	std::cout<<" Exc: "<<_exc;
	int c=1;
	for (it=_axl.begin(); it!=_axl.end(); it++){
		std::cout<<" axon "<<c<<": "<<it->get_cond()<<std::endl;//" connected: "<<it->isconn()<<endl;
		++c;
	}
	std::cout<<" Threshold: "<<_thresh;
	std::cout<<" Leak: "<<_leak;
	std::cout<<" Habituation: "<<_habituation;
}
void Neuron::scramble(){
	std::vector<Axon>::iterator it;
	for (it=_axl.begin(); it!=_axl.end(); it++){
		it->set_cond((double)(rand()%19-9)/10);
	}
}
void Neuron::mutate_axon(int na,double mrate){
	if (_axl.empty()){
		std::cout<<" No axons in neuron with leak: "<<_leak<<". ";
		return;
	}
	std::vector<Axon>::iterator ait;
	if (na==0){
		ait=_axl.end();
		--ait;
	}else {
		ait=_axl.begin();
		for (int c=1;c<na;++c){
			if (ait==_axl.end()){
				--ait;
				break;	
			}
			++ait;
		}
	}
	ait->mutate(mrate);
}
void Neuron::norm(){
	std::vector<Axon>::iterator it;
	double condsum=0;
	for (it=_axl.begin(); it!=_axl.end(); ++it){
		if (it->isconn())
			condsum+=std::abs(it->cond);
	}
	for (it=_axl.begin(); it!=_axl.end(); ++it){
		it->set_cond(it->get_cond()/condsum);
	}
}
double Neuron::get_exc(){
	return _exc;
}
void Neuron::set_exc(double ex){
	_exc=ex;
}
void Neuron::set_thresh(double t){
	_thresh=t;
}
double Neuron::get_thresh(){
	return _thresh;
}
void Neuron::reverseAxons(){
	for (std::vector<Axon>::iterator ita=_axl.begin(); ita!=_axl.end(); ++ita){
		ita->mutate(-2*(ita->get_cond()));
	}
}
void Neuron::mutate(double mrate){
	for (std::vector<Axon>::iterator ita=_axl.begin(); ita!=_axl.end(); ++ita){
		ita->mutate(mrate);
	}
}
#endif
