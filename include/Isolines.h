/*
 * Isolines.h
 *
 *  Created on: Sep 26, 2014
 *      Author: bjw
 */

#pragma once

#include <vector>
#include "Isoline.h"

using namespace std;

class Isolines {
public:
	Isolines();
	Isolines(vector<float>);
	Isolines(float[], int numValues);
	vector<float>					getIsolineValues();
	float 							getIsolineValue(int lineIndex);
	void							addIsolineValue(float isoValue);
	int 							numLines();
	Isoline	*						getLine(int lineIndex);
	void 							setLine(int lineIndex, Isoline * line);
	pair<float, float>  			getPoint(int lineIndex, int pointIndex);


private:
	vector<float> 					isoValues;
	vector<Isoline*>				isolines;

};
