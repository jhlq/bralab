#ifndef BRAIN_H
#define BRAIN_H

#include <fstream>
#include "Neuron.h"

class Brain{
	protected:
		std::vector<Neuron> _inlayer;
		std::vector<Neuron> _utlayer;
		std::vector<Neuron> _nl;
		std::vector<double> _result;
		std::vector<Neuron> _buffer;
		
	public:
		Brain(int in, int O, int ut,bool fc=1);
		void addN(double threshold=0,bool bias=0);
		virtual std::vector<Neuron>& get_nl(){ return _nl; }
		std::vector<Neuron>& get_buf(){ return _buffer; }
		std::vector<Neuron>& get_inl(){ return _inlayer; }
		std::vector<Neuron>& get_utl(){ return _utlayer; }
		std::vector<double>& get_result(){ return _result; }
		virtual void Activate(std::vector<double> &data, bool verbose=false);
		void activate(int steps=1);
		virtual void print();
		void mutate();
		void mutate_dep(int la=50, bool min=1, bool mO=1,bool thresh=0);
		void save(std::string filename);
		
		std::vector<double> mvaxons;
		std::vector<double>& get_mvaxons(){ return mvaxons; }
		void set_mvaxons(int nn,double amount){ mvaxons[nn]=amount; }
		std::vector<double> mvthresh;
		std::vector<double>& get_mvthresh(){ return mvthresh; }
		void set_mvthresh(int nn,double amount){ mvthresh[nn]=amount; }
		std::vector<double> mvleak;
		std::vector<double>& get_mvleak(){ return mvleak; }
		void set_mvleak(int nn,double amount){ mvleak[nn]=amount; }
		std::vector<double> mvhab;
		std::vector<double>& get_mvhab(){ return mvhab; }
		void set_mvhab(int nn,double amount){ mvhab[nn]=amount; }
		
};


Brain::Brain(int in, int O, int ut,bool fc){ 
	for (int c=0;c<in;++c){ _inlayer.push_back(Neuron()); }
	for (int c=0;c<O;++c){ _nl.push_back(Neuron()); }
	for (int c=0;c<ut;++c){ _utlayer.push_back(Neuron()); }
	for (int c=0;c<O;++c){ 
		_buffer.push_back(Neuron()); 
		mvaxons.push_back(0.1);
		mvthresh.push_back(0);
		mvleak.push_back(0);
		mvhab.push_back(0);
	}
	for (int c=0;c<O;++c){ 
		_nl[c].bufferneuron=&_buffer[c];
	}
	
	if(fc){
		for (int c=0;c<in;++c){ (_inlayer.begin()+c)->connect2L(_nl,1); }
		for (int c=0;c<O;++c){ (_nl.begin()+c)->connect2L(_utlayer,1); }
	}
}
void Brain::addN(double threshold,bool bias){ 
	_nl.push_back(Neuron(threshold,bias)); 
}
void Brain::mutate(){
	int nn=_nl.size();
	double rm;
	double r;
	for (int ni=0;ni<nn;++ni){
		int ra=rand()%nn;
		r=((double)(rand()%200)-100)/100;
		rm=r*mvaxons[ni];
		_nl[ni].mutate_axon(ra,rm);
	}
	int rn=rand()%nn;
	r=((double)(rand()%200)-100)/100;
	rm=r*mvthresh[rn];
	_nl[rn].set_thresh(_nl[rn].get_thresh()+rm);
	
	rn=rand()%nn;
	r=((double)(rand()%200)-100)/100;
	rm=r*mvleak[rn];
	_nl[rn].set_leak(_nl[rn].get_leak()+rm);
	
	rn=rand()%nn;
	r=((double)(rand()%200)-100)/100;
	rm=r*mvhab[rn];
	_nl[rn].set_habituation(_nl[rn].get_habituation()+rm);
}
	
void Brain::mutate_dep(int la,bool min, bool mO, bool thresh){
	int r1;
	double r2;
	int r3;
	if (min) {
		
		r2=rand()%(2*la)-la;
		r2=r2/100;	
		r1=rand()%_inlayer.size();
		if (thresh){
			_inlayer[r1].set_thresh(_inlayer[r1].get_thresh()+r2);
		}else{
			r3=rand()%_inlayer[r1]._axl.size();
			_inlayer[r1].mutate_axon(r3,r2);
		}
	}
	if (mO) {
		r2=rand()%(2*la)-la;
		
		r2=r2/100;
		r1=rand()%_nl.size();
		if (thresh){
			_nl[r1].set_thresh(_nl[r1].get_thresh()+r2);
		}else{
			r3=rand()%_nl[r1]._axl.size();
			_nl[r1].mutate_axon(r3,r2);
		}
	}
	
}
void Brain::activate(int steps){
	for (int i=0;i<steps;++i){
		for (std::vector<Neuron>::iterator nit=_nl.begin();(nit)!=_nl.end();++nit){
			nit->vibrate();
		}
		for (unsigned int i=0;i<_nl.size();++i){
			_nl[i]._exc=_buffer[i]._exc;
			_buffer[i]._exc=0;
		}
	}
}
	
void Brain::Activate(std::vector<double> &data, bool verbose){ 
	_result.clear();
	std::vector<double>::iterator dit=data.begin();
	for (std::vector<Neuron>::iterator nit=_inlayer.begin();nit!=_inlayer.end();++nit){
		int position = nit - _inlayer.begin() ;
		nit->excite(*dit);
		++dit;

		if (verbose)
			std::cout<<" Inlayer, neuron "<<position<<": "<<nit->vibrate()<<" ";
		else
			nit->vibrate();
		
	}
	for (std::vector<Neuron>::iterator nit=_nl.begin();nit!=_nl.end();++nit){
		int position = nit - _nl.begin() ;
	
		if (verbose)
			std::cout<<" Olayer, neuron "<<position<<": "<<nit->vibrate()<<" ";
		else
			nit->vibrate();
		
	}
	for (std::vector<Neuron>::iterator nit=_utlayer.begin();nit!=_utlayer.end();++nit){
		int position = nit - _utlayer.begin() ;
		
		double re=nit->vibrate();
		_result.push_back(re);
		if (verbose)
			std::cout<<" Result "<<position<< ": "<<re<<" ";
	
	}
}

void Brain::print(){
	std::cout<<" Brain printing... "<<std::endl;
	std::cout<<" Inlayer: ";
	for (std::vector<Neuron>::iterator nit=_inlayer.begin();nit!=_inlayer.end();++nit){
		
		nit->print();
	}
	std::cout<<" Olayer: ";
	for (std::vector<Neuron>::iterator nit=_nl.begin();nit!=_nl.end();++nit){
		
		nit->print();
	}
	std::cout<<" Utlayer: ";
	for (std::vector<Neuron>::iterator nit=_utlayer.begin();nit!=_utlayer.end();++nit){
		
		nit->print();
	}
	std::cout<<std::endl;
}
void Brain::save(std::string filename){
	std::ofstream s;
	s.open(filename);
	s << "Connections matrix:\n";
	for (std::vector<Neuron>::iterator nit=_nl.begin();nit!=_nl.end();++nit){
		for (std::vector<Axon>::iterator ait=nit->get_axv().begin();ait!=nit->get_axv().end();++ait){
			s<<ait->get_cond()<<"\t";
		}
		s<<"\n";
	}
	s << "\nThresholds:\n";
	for (std::vector<Neuron>::iterator nit=_nl.begin();nit!=_nl.end();++nit){
		s<<nit->get_thresh()<<"\n";
	}
	s << "\nLeaks:\n";
	for (std::vector<Neuron>::iterator nit=_nl.begin();nit!=_nl.end();++nit){
		s<<nit->get_leak()<<"\n";
	}
	s.close();
}
#endif
