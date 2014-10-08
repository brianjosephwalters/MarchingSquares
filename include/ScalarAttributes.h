#pragma once

#include <vector>

using namespace std;


class ScalarAttributes
{
public:
				ScalarAttributes(int size): values(size) { }
void			setC0Scalar(int i,float v)
					{  values[i] = v;  }
float			getC0Scalar(int i)
					{  return values[i];  }
float			getMin();
float			getMax();
				
protected:

vector<float>	values;
};
