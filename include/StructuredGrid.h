#pragma once

#include "UniformGrid.h"
#include <vector>

using namespace std;



class StructuredGrid : public UniformGrid
{
public:			
				StructuredGrid(int N1, int N2): UniformGrid(N1,N2,0,0,0,0)
				{ pointsX.resize(N1*N2); pointsY.resize(N1*N2); pointsZ.resize(N1*N2); } 
void			getPoint(int i,float* p);
void			setPoint(int i,float* p);		
int				findCell(float*);
			
private: 

vector<float>	pointsX,pointsY,pointsZ;		//Explicit vertex coordinates for all vertices
};





