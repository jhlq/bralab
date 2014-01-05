#ifndef TANHFUNCTION_H
#define TANHFUNCTION_H
#include "Function.h"
#include <cmath>


class TanhFunction : public Function{
	protected:
		double _c0;
		double _c1;
	
	public:
		TanhFunction(double c1,double c0){
			_c0=c0;
			_c1=c1;
		}
		virtual double transform(double x){
			return _c1*tanh(_c0*x);
		}
		void set_tail(double t){_c0=t;}
		double get_tail(){return _c0;}
};


#endif
