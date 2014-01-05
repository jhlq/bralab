#ifndef FUNCTION_H
#define FUNCTION_H

class Function{

public:
	Function();
	virtual double transform(double)=0;
	virtual void set_tail(double)=0;
	virtual double get_tail()=0;

	
};
Function::Function(){}
#endif
