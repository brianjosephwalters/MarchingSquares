#pragma once

#include "UniformGrid.h"
#include <vector>

using namespace std;



class RectilinearGrid : public UniformGrid
{
public:			

		RectilinearGrid(vector<float>& dimsX, vector<float>& dimsY)
				: UniformGrid(dimsX.size(),				//number of samples along X
							  dimsY.size(),				//number of samples along y	
							  dimsX[0],					//minimal X value
							  dimsY[0],					//minimal Y value
							  dimsX[dimsX.size()-1],	//maximal X value
							  dimsY[dimsY.size()-1])	//maximal Y value
		{ dX = dimsX; dY = dimsY; }
		
void	getPoint(int i,float* p);
			
protected:

private: 

	    vector<float> dX,dY;							//Sampling positions along the X and Y axes
};




