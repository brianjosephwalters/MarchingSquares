#include "ScalarAttributes.h"
#include <algorithm>

using namespace std;



float ScalarAttributes::getMin()
{
	float minimum = values[0];
	for(int i=1;i<values.size();++i)
	   minimum = min(minimum,values[i]);
	   
	return minimum;
}

float ScalarAttributes::getMax()
{
	float maximum = values[0];
	for(int i=1;i<values.size();++i)
	   maximum = max(maximum,values[i]);
	   
	return maximum;
}
	   

