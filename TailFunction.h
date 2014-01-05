#ifndef TAILFUNCTION_H
#define TAILFUNCTION_H
#include "Function.h"
#include <cmath>

class TailFunction : public Function{
	protected:
		double tail;
		double amp;	
	public:
		TailFunction(double t,double a){
			tail=t;
			amp=a;
		}
		virtual double transform(double x){
			return amp*x/(std::abs(x)+tail);
		}
		void set_tail(double t){ tail=t; }
		double get_tail(){ return tail; }
};
#endif
