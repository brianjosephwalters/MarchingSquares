/*
 * Isoline.h
 *
 *  Created on: Sep 26, 2014
 *      Author: bjw
 */

#pragma once

#include <vector>

using namespace std;

class Isoline {
public:
	Isoline();
	Isoline(int size);
	int 					numPoints();
	void 					addPoint(float x, float y);
	void					addPoint(pair<float, float> p);
	pair<float, float> 		getPoint(int pointIndex);


private:
	vector< pair<float, float> >		points;
};
