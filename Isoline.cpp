/*
 * Isoline.cpp
 *
 *  Created on: Sep 26, 2014
 *      Author: bjw
 */

#include "Isoline.h"

Isoline::Isoline() {
	this->points = vector< pair<float, float> >();
}

Isoline::Isoline(int size) {
	this->points = vector< pair<float, float> >(size);
}

int Isoline::numPoints() {
	return this->points.size();
}

void Isoline::addPoint(float x, float y) {
	this->points.push_back(pair<float, float>(x, y));
}

void Isoline::addPoint(pair<float, float> p) {
	this->points.push_back(p);
}

/**
 * @require	pointIndex < numPoints();
 */
pair<float, float> Isoline::getPoint(int pointIndex) {
	return this->points.at(pointIndex);
}


