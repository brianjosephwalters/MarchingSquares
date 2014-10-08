#pragma once


class ScalarAttributes;


class Grid
{
public: 

virtual			         ~Grid();
virtual int				  numPoints()			=0; 
virtual int				  numCells()			=0; 
virtual void			  getPoint(int,float*)	=0; 
virtual int				  getCell(int,int*)		=0; 
virtual int				  findCell(float*)		=0;
virtual ScalarAttributes& pointScalars()		=0;

protected:
						  Grid() { }
};



