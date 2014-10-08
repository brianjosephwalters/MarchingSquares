Marching Squares Project
Author: Brian J. Walters

Description:
The goal of this project is to use the "Marching Squares" algorithm to draw
isolines for a function displayed on a 2D grid.

Changes were primarily made to MarchingSquares.cpp and ColormapRenderer.cpp.  Original
code from: http://www.cs.rug.nl/svcg/DataVisualizationBook/Sp5
Isoline and Isolines classes were created to store the results of the the 
marching squares algorithm. 

Features:
Use 'c' to toggle the color 
Use space to change the rendering.
use 'i' to draw isolines.

To change the function, edit MarchingSquares.cpp and adjusted the f() method.
To add isolines, edit MarchingSquares.cpp and adjust the isoValues array.


TODO:
1.  "saving your 2D scalar dataset to the VTK uniform grid format".  Never figured out how to do this.
2.  Try more functions!
3.  Allow user to provide isoline values in some way:
	a) keyboard input
	b) command line
4.  Provide a "smarter" color choice for the isoline(s).  

First foray into C++ since 2002.  Rock on! 
