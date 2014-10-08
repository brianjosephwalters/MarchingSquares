#pragma once

#include "Grid.h"
#include "ScalarAttributes.h"


class UniformGrid : public Grid
{
public:			
					UniformGrid(int N1, int N2, float m1, float m2, float M1, float M2)
					: scalars(N1*N2),
					  N1(N1),N2(N2),m1(m1),m2(m2),d1((M1-m1)/(N1-1)),d2((M2-m2)/(N2-1))
					  	{ }

		           ~UniformGrid();	

int					numPoints() 
						{ return N1*N2; }

int					numCells() 
						{ return (N1-1)*(N2-1); }

void				getPoint(int i,float* p) 
						{ p[0] = m1 + (i%N1)*d1; p[1] = m2 + (i/N1)*d2; }

int					getCell(int i,int* c); 

int					getDimension1()
						{ return N1; }

int					getDimension2()
						{ return N2; }
			
int					findCell(float*);

ScalarAttributes&	pointScalars()
						{ return scalars; }
			
protected:

ScalarAttributes scalars;
int				 N1,N2;			//Number of points along the x− and y−axes 
float			 m1,m2;			//Minimal coordinate values in this grid

private: 

float		d1,d2;				//Cell sizes in this grid
};




