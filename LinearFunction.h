#ifndef LINEARFUNCTION_H
#define LINEARFUNCTION_H
#include "Function.h"
#include <cmath>

class LinearFunction : public Function{
	protected:
		double tail;
		double amp;	
	public:
		LinearFunction(double t,double a){
			tail=t;
			amp=a;
		}
		virtual double transform(double x){
			return amp*x/tail;
		}
		void set_tail(double t){ tail=t; }
		double get_tail(){ return tail; }
};
#endif
