/*
 * Isolines.cpp
 *
 *  Created on: Sep 26, 2014
 *      Author: bjw
 */

#include "Isolines.h"
#include <iostream>

Isolines::Isolines () {
	this->isoValues = vector<float>();
	this->isolines = vector<Isoline*>();
}

Isolines::Isolines (vector<float> isoValues)
{
	this->isoValues = isoValues;
	this->isolines = vector<Isoline*>();
	//Initialize Lines
	for (int i = 0; i < isoValues.size(); i++)
	{
		this->isolines.push_back(new Isoline());
	}
}

Isolines::Isolines(float values[], int numValues) {
	this->isoValues = vector<float>();
	this->isolines = vector<Isoline*>();
	for (int i = 0; i < numValues; i++) {
		this->isoValues.push_back(values[i]);
		this->isolines.push_back(new Isoline());
	}
}

vector<float> Isolines::getIsolineValues()
{
	return this->isoValues;
}

/**
 * @require isoValues.size() < lineIndex
 */
float Isolines::getIsolineValue(int lineIndex)
{
	return this->isoValues.at(lineIndex);
}

void Isolines::addIsolineValue(float isoValue) {
	this->isoValues.push_back(isoValue);
}

int Isolines::numLines()
{
	return this->isolines.size();
}


Isoline * Isolines::getLine(int lineIndex) {
	return this->isolines.at(lineIndex);
}

void Isolines::setLine(int lineIndex, Isoline * line)
{
	this->isolines.at(lineIndex) = line;
}

/**
 * @require sizeof( getLine(lineIndex) ) > pointIndex
 */
pair<float, float> Isolines::getPoint(int lineIndex, int pointIndex)
{
	return this->isolines.at(lineIndex)->getPoint(pointIndex);
}



