#pragma once

#include "Grid.h"
#include "ScalarAttributes.h"
#include <vector>

using namespace std;




class UnstructuredGrid : public Grid
{
public: 
					UnstructuredGrid(int P,int C): scalars(P)
					{
						pointsX.resize(P);
						pointsY.resize(P);
						pointsZ.resize(P);
						cells.resize(3*C);
					}	 
					 	
int					numPoints()
					{ return pointsX.size(); }				 

int					numCells()	
					{ return cells.size()/3; }

void				getPoint(int i,float* p);

void				setPoint(int i,float* p);		

int					getCell(int cell,int* vertices); 

void				setCell(int cell,int* vertices);

int					findCell(float* p);

ScalarAttributes&	pointScalars()
					{ return scalars; }

private:

ScalarAttributes	scalars;
vector<float>		pointsX,pointsY,pointsZ;
vector<int>			cells;
};



