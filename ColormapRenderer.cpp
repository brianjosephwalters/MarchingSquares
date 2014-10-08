#include "ColormapRenderer.h"
#include "ScalarAttributes.h"
#include "Grid.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <algorithm>
#include <math.h>
#include <iostream>
#include <cstdio>

using namespace std;

void rainbow(float value,float& R,float& G,float& B)	//Maps a scalar in [0,1] to a color using the rainbow colormap
{
   const float dx=0.8f;

   value = (6-2*dx)*value+dx;
   R = max(0.0f,(3-(float)fabs(value-4)-(float)fabs(value-5))/2);
   G = max(0.0f,(4-(float)fabs(value-2)-(float)fabs(value-4))/2);
   B = max(0.0f,(3-(float)fabs(value-1)-(float)fabs(value-2))/2);
}

void grayscale(float value,float& R,float& G,float& B)	//Maps a scalar in [0,1] to a color using a grayscale colormap
{
   R = G = B = value;
}

void saturation(float value,float& R,float& G,float& B)	//Maps a scalar in [0,1] to a color using a saturation colormap
{
   float color_r = 0;									//The base color whose saturation we change (green).	
   float color_g = 1;									//Try different colors!	
   float color_b = 0;
   
   if (value<0.5)										//value in [0,0.5]: modulate the luminance from black to the base-color.
   {   
	   R = 2*value*color_r;
	   G = 2*value*color_g;
	   B = 2*value*color_b;
   }
   else													//value in [0.5,1]: modulate the saturation from base-color to white.
   {
	   value = 2*(value-0.5);	
	   R = (1-value)*color_r + 1*value;
	   G = (1-value)*color_g + 1*value;
	   B = (1-value)*color_b + 1*value;
   }
}

void ColormapRenderer::mapToColor(float value,float& R,float& G,float& B)
{														//Map v to a color using currently active colormap
	switch (colormap_type)
	{
	case COLORMAP_GRAYSCALE: 
		grayscale(value,R,G,B);
		break;
	case COLORMAP_RAINBOW: 
		rainbow(value,R,G,B);
		break;
	case COLORMAP_SATURATION: 
		saturation(value,R,G,B);
		break;
	}
}		

void ColormapRenderer::drawGrid(Grid& g)				//Draw 'g' as simple grid. Compare this code with SimpleRenderer.
{
	glColor3f(0,0,0);									//1.   Use black to render cells
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);			//     Render cells as outlines

	for(int i=0;i<g.numCells();++i)						//2. Draw all cells in the grid as black outlines
	{
		int   cell[10];									//Should have enough storage space for our largest cell
		float x[2];
		
		int nvertices = g.getCell(i,cell);				//2.1. Get the 'nvertices' vertex-IDs of i-th cell
		if (nvertices!=3 && nvertices!=4)				//     We only handle here drawing of triangle and quad cells.
		   continue;									//     It is quite simple to extend this to other cell types.

		if (nvertices==3)								//     Triangle cells:
			glBegin(GL_TRIANGLES);						
		else //nvertices==4	
			glBegin(GL_QUADS);							//     Quad cells:
		
		for(int j=0;j<nvertices;++j)					//2.2. Render current cell
		{
			g.getPoint(cell[j],x);						//     Get vertex coordinates of j-th vertex of i-th cell		
			glVertex3f(x[0],x[1],0);					//     Pass this coordinate to OpenGL
		}	
		glEnd();                                        //2.3. Finish rendering current cell
	}

	glPointSize(3);										//3.   Draw all vertices in the grid as large red points
	glColor3f(1,0,0);
	glBegin(GL_POINTS);									//3.1. Draw vertices as an OpenGL point-set	
	for(int i=0;i<g.numPoints();++i)						
	{
		float v[2];
		g.getPoint(i,v);								//3.2. Get coordinates of i-th vertex				
		glVertex3f(v[0],v[1],0);						//     Pass this coordinate to OpenGL
	}
	glEnd();											//3.3. Finish rendering the entire point-set
}

void ColormapRenderer::drawPoints(Grid& g)				//Draw 'g' by showing scalar values at their definition locations (vertices)
{
	ScalarAttributes& scalars = g.pointScalars();		//     Get scalar range, to normalize scalar-values next in [0,1]
	float scalar_min = scalars.getMin();
	float scalar_max = scalars.getMax();
	
	glPointSize(3);										//1.   Draw all vertices in the grid as large points
	glBegin(GL_POINTS);									//2.   Draw vertices as an OpenGL point-set	
	for(int i=0;i<g.numPoints();++i)						
	{
		float v[2];
		g.getPoint(i,v);								//2.1. Get coordinates of i-th vertex	
		
		float val = (g.pointScalars().getC0Scalar(i)-scalar_min)/(scalar_max-scalar_min);
		float r,g,b;									//2.2.  Get scalar value at i-th vertex and normalize it to [0,1]
		mapToColor(val,r,g,b);							//2.3.  Map normalized scalar value to a color 
		glColor3f(r,g,b);								//2.4.  Pass color to OpenGL
		glVertex3f(v[0],v[1],0);						//2.5.  Pass this coordinate to OpenGL
	}
	glEnd();											//3.    Finish rendering the entire point-set
}

void ColormapRenderer::drawC0Cells(Grid& g)				//Draw 'g' by showing C0-interpolated scalar values over cells.
{														//Cell scalars are computed from vertex scalar by averaging.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			//1.    Render cells filled
	glShadeModel(GL_FLAT);								//2.    Do not interpolate vertex colors (use flat-shaded polygons)

	ScalarAttributes& scalars = g.pointScalars();		//3.    Get scalar range, to normalize scalar-values next in [0,1]
	float scalar_min = scalars.getMin();
	float scalar_max = scalars.getMax();

	for(int i=0;i<g.numCells();++i)						//4. Draw all cells in the grid as filled flat-shaded polygons
	{
		int   cell[10];									//Should have enough storage space for our largest cell
		
		// Put in "cell" the _indexes_ on the "scalars" list
		// which contain the proper scalar values.
		int nvertices = g.getCell(i,cell);				//4.1. Get the 'nvertices' vertex-IDs of i-th cell
		if (nvertices!=3 && nvertices!=4)				//     We only handle here drawing of triangle and quad cells.
		   continue;									//     It is quite simple to extend this to other cell types.

		if (nvertices==3)								//     Triangle cells:
			glBegin(GL_TRIANGLES);						
		else //nvertices==4	
			glBegin(GL_QUADS);							//     Quad cells:

		// For every vertex of the "cell", use the _index_
		// to get the scalar value for that vertex.
		// Produce the average scalar value for the whole cell.
		float average_val = 0;							//4.2. Compute average scalar over the current cell	
		for(int j=0;j<nvertices;++j)					//
			average_val += g.pointScalars().getC0Scalar(cell[j]);
		average_val /= nvertices;

														//4.3. Normalize average scalar in [0,1] range
		average_val = (average_val-scalar_min)/(scalar_max-scalar_min);		

		// Map the scalar value to a color.
		float R,G,B;									//4.4. Map average scalar to color
		mapToColor(average_val,R,G,B);					//4.5. Pass color to OpenGL. As we use flat-shading, the entire cell
		glColor3f(R,G,B);								//     will have this color	

		// Finally, pass the four cell vertexes
		// to the drawing functions as their
		// plotting point.
		for(int j=0;j<nvertices;++j)					//4.6. Render current cell
		{
			float x[2];
			g.getPoint(cell[j],x);						//     Get vertex coordinates of j-th vertex of i-th cell				
			glVertex3f(x[0],x[1],0);					//     Pass this coordinate to OpenGL
		}	
		glEnd();                                        //4.7. Finish rendering current cell
	}
}

/**
 * First try:
 * My goal here is to simply identify the cells through
 * which an isoline will pass.  Sort of just learning how the
 * grid class works, and the kind of information that is available.
 */
void ColormapRenderer::drawC0CellsWithIsoCell(Grid& grid)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_FLAT);

	ScalarAttributes& scalars = grid.pointScalars();
	float scalarMin = scalars.getMin();
	float scalarMax = scalars.getMax();
	for (int i = 0; i < grid.numCells(); i++)
	{
		int cellHolder[4];
		int numVertices = grid.getCell(i, cellHolder);
		glBegin(GL_QUADS);

		// Shading most cells
		float average = 0.0f;
		for (int j = 0; j < numVertices; j++)
		{
			average += scalars.getC0Scalar(cellHolder[j]);
		}
		average /= numVertices;

		average = (average-scalarMin)/(scalarMax-scalarMin);

		// Check if isovalue crosses cell.
		float isoValue = float(scalarMin + scalarMax) / 2;
		bool isInside = false;

		float scalarHolder[4];
		for (int j = 0; j < numVertices; j++) {
			scalarHolder[j] = scalars.getC0Scalar(cellHolder[j]);
		}

		if ( ( (scalarHolder[0] < isoValue) && (scalarHolder[1] > isoValue) ) ||
		     ( (scalarHolder[1] < isoValue) && (scalarHolder[2] > isoValue) ) ||
		     ( (scalarHolder[2] < isoValue) && (scalarHolder[3] > isoValue) ) ||
		     ( (scalarHolder[3] < isoValue) && (scalarHolder[0] > isoValue) ) ) {
			isInside = true;
		}

		float R, G, B;

		if (isInside) {
			mapToColor(0.0, R, G, B);
		} else {
			mapToColor(average,R,G,B);
		}
		glColor3f(R,G,B);

		for (int j = 0; j < numVertices; j++)
		{
			float x[2];
			grid.getPoint(cellHolder[j], x);
			glVertex3f(x[0],x[1],0);
		}
		glEnd();
	}
}


/**
 * Second try:
 * Here I draw a line from the midpoint of edges where
 * the isoline crosses.  I'm narrowing from thinking about
 * the cell as a whole to the individual edges that cross
 * an isoline.  I'm figuring out how to create vertices for GL_LINES,
 * and making sure things "just work".
 */
void ColormapRenderer::drawC0CellsWithMidpointIsoLine(Grid& grid)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_FLAT);

	ScalarAttributes& scalars = grid.pointScalars();
	float scalarMin = scalars.getMin();
	float scalarMax = scalars.getMax();
	for (int i = 0; i < grid.numCells(); i++)
	{

		int cellHolder[4];
		int numVertices = grid.getCell(i, cellHolder);


		// Shading most cells
		float average = 0.0f;
		for (int j = 0; j < numVertices; j++)
		{
			average += scalars.getC0Scalar(cellHolder[j]);
		}
		average /= numVertices;
		average = (average-scalarMin)/(scalarMax-scalarMin);

		// Check if isovalue crosses cell.
		float isoValue = float(scalarMin + scalarMax) / 8;
		//cout << "ISOVALUE: " << isoValue << "\n";
		bool isInside = false;

		float scalarHolder[4];
		for (int j = 0; j < numVertices; j++)
		{
			scalarHolder[j] = scalars.getC0Scalar(cellHolder[j]);
		}

		glBegin(GL_QUADS);
		float R, G, B;
		mapToColor(average,R,G,B);
		glColor3f(R,G,B);

		for (int j = 0; j < numVertices; j++)
		{
			float x[2];
			grid.getPoint(cellHolder[j], x);
			glVertex3f(x[0],x[1],0);
		}
		glEnd();

		glLineWidth(2.5);
		glColor3f(0.5f,1.0f,0.0f);
		glBegin(GL_LINES);

		float x0[2], m0[2];
		float x1[2], m1[2];
		float x2[2], m2[2];
		float x3[2], m3[2];

		grid.getPoint(cellHolder[0], x0);
		grid.getPoint(cellHolder[1], x1);
		grid.getPoint(cellHolder[2], x2);
		grid.getPoint(cellHolder[3], x3);

		// Finds the midpoint of the cell's edges
		m0[0] = (x1[0] + x0[0]) / 2;
		m0[1] = (x1[1] + x0[1]) / 2;
		m1[0] = (x2[0] + x1[0]) / 2;
		m1[1] = (x2[1] + x1[1]) / 2;
		m2[0] = (x3[0] + x2[0]) / 2;
		m2[1] = (x3[1] + x2[1]) / 2;
		m3[0] = (x0[0] + x3[0]) / 2;
		m3[1] = (x0[1] + x3[1]) / 2;

		if ( ((scalarHolder[0] < isoValue) && (scalarHolder[1] > isoValue)) ||
			 ((scalarHolder[1] < isoValue) && (scalarHolder[0] > isoValue)) )
		{
			glVertex3f(m0[0],m0[1],0);
		}
		if ( ((scalarHolder[1] < isoValue) && (scalarHolder[2] > isoValue)) ||
			 ((scalarHolder[2] < isoValue) && (scalarHolder[1] > isoValue)) )
		{
			glVertex3f(m1[0],m1[1],0);
		}
		if ( ((scalarHolder[2] < isoValue) && (scalarHolder[3] > isoValue)) ||
			 ((scalarHolder[3] < isoValue) && (scalarHolder[2] > isoValue)) )
		{
			glVertex3f(m2[0],m2[1],0);
		}
		if ( ((scalarHolder[3] < isoValue) && (scalarHolder[0] > isoValue)) ||
			 ((scalarHolder[0] < isoValue) && (scalarHolder[3] > isoValue)) )
		{
			glVertex3f(m3[0],m3[1],0);
		}
		//cout << '\n';
		glEnd();
	}
}

/**
 * Third try:
 * Now I'm drawing the isoline at the location where it actually
 * crosses the edge.  I'm looking at the edge crossing cases more closely,
 * since "high" vs. "low" vertex matters when calculating the
 * crossing point.  I'm also abstracting out some of the calculation
 * in order to see what information is used in which part of the algorithm.
 */
void ColormapRenderer::drawC0CellsWithIsoLine(Grid& grid)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_FLAT);

	ScalarAttributes& scalars = grid.pointScalars();
	float scalarMin = scalars.getMin();
	float scalarMax = scalars.getMax();
	for (int i = 0; i < grid.numCells(); i++)
	{
		// Get vertex indices
		int cellHolder[4];
		int numVertices = grid.getCell(i, cellHolder);

		// Get data for cell: vertex coordinates and scalars.
		float points[4][2];
		float values[4];
		for (int j = 0; j < numVertices; j++) {
			grid.getPoint(cellHolder[j], points[j]);
			values[j] = scalars.getC0Scalar(cellHolder[j]);
		}


		glBegin(GL_QUADS);

		// Shading most cells
		float average = 0.0f;
		for (int j = 0; j < numVertices; j++) {
			average += scalars.getC0Scalar(cellHolder[j]);
		}
		average /= numVertices;
		average = (average-scalarMin)/(scalarMax-scalarMin);

		//Create a default isoline value.
		float isoValue = float(scalarMin + scalarMax) / 8;

		float scalarHolder[4];
		for (int j = 0; j < numVertices; j++) {
			scalarHolder[j] = scalars.getC0Scalar(cellHolder[j]);
		}

		float R, G, B;
		mapToColor(average,R,G,B);
		glColor3f(R,G,B);

		for (int j = 0; j < numVertices; j++) {
			float x[2];
			grid.getPoint(cellHolder[j], x);
			glVertex3f(x[0],x[1],0);
		}
		glEnd();

		drawIsoLineForCell(points, values, isoValue);
	}
}

/**
 * This is a helper method for drawC0CellsWithIsoLine.
 */
void ColormapRenderer::drawIsoLineForCell(float points[][2], float values[], float isoValue) {

	float q0[2], q1[2], q2[2], q3[2];
	glLineWidth(2.5);
	glColor3f(0.5f,1.0f,0.0f);
	glBegin(GL_LINES);

	// Finds the actual point of intersection with the cell's edge.
	if ( ((values[0] < isoValue) && (values[1] > isoValue)) ||
		 ((values[1] < isoValue) && (values[0] > isoValue)) )
	{
		if (values[0] < values[1]) {
			q0[0] = (points[0][0] * (values[1] - isoValue) + points[1][0] * (isoValue - values[0])) / (values[1] - values[0]);
			q0[1] = (points[0][1] * (values[1] - isoValue) + points[1][1] * (isoValue - values[0])) / (values[1] - values[0]);

		}
		else
		{
			q0[0] = (points[1][0] * (values[0] - isoValue) + points[0][0] * (isoValue - values[1])) / (values[0] - values[1]);
			q0[1] = (points[1][1] * (values[0] - isoValue) + points[0][1] * (isoValue - values[1])) / (values[0] - values[1]);

		}
		glVertex3f(q0[0],q0[1],0);
	}
	if ( ((values[1] < isoValue) && (values[2] > isoValue)) ||
		 ((values[2] < isoValue) && (values[1] > isoValue)) )
	{
		if (values[1] < values[2])
		{
			q1[0] = (points[1][0] * (values[2] - isoValue) + points[2][0] * (isoValue - values[1])) / (values[2] - values[1]);
			q1[1] = (points[1][1] * (values[2] - isoValue) + points[2][1] * (isoValue - values[1])) / (values[2] - values[1]);

		}
		else
		{
			q1[0] = (points[2][0] * (values[1] - isoValue) + points[1][0] * (isoValue - values[2])) / (values[1] - values[2]);
			q1[1] = (points[2][1] * (values[1] - isoValue) + points[1][1] * (isoValue - values[2])) / (values[1] - values[2]);

		}
		glVertex3f(q1[0],q1[1],0);
	}
	if ( ((values[2] < isoValue) && (values[3] > isoValue)) ||
		 ((values[3] < isoValue) && (values[2] > isoValue)) )
	{
		if (values[2] < values[3])
		{
			q2[0] = (points[2][0] * (values[3] - isoValue) + points[3][0] * (isoValue - values[2])) / (values[3] - values[2]);
			q2[1] = (points[2][1] * (values[3] - isoValue) + points[3][1] * (isoValue - values[2])) / (values[3] - values[2]);

		} else {
			q2[0] = (points[3][0] * (values[2] - isoValue) + points[2][0] * (isoValue - values[3])) / (values[2] - values[3]);
			q2[1] = (points[3][1] * (values[2] - isoValue) + points[2][1] * (isoValue - values[3])) / (values[2] - values[3]);

		}
		glVertex3f(q2[0],q2[1],0);
	}
	if ( ((values[3] < isoValue) && (values[0] > isoValue)) ||
		 ((values[0] < isoValue) && (values[3] > isoValue)) )
	{
		if (values[3] < values[0])
		{
			q3[0] = (points[3][0] * (values[0] - isoValue) + points[0][0] * (isoValue - values[3])) / (values[0] - values[3]);
			q3[1] = (points[3][1] * (values[0] - isoValue) + points[0][1] * (isoValue - values[3])) / (values[0] - values[3]);

		}
		else
		{
			q3[0] = (points[0][0] * (values[3] - isoValue) + points[3][0] * (isoValue - values[0])) / (values[3] - values[0]);
			q3[1] = (points[0][1] * (values[3] - isoValue) + points[3][1] * (isoValue - values[0])) / (values[3] - values[0]);
		}
		glVertex3f(q3[0],q3[1],0);
	}
	glEnd();
}


void ColormapRenderer::drawC1Cells(Grid& g)				//Draw 'g' by showing C1-interpolated scalars over cells
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			//1.   Render cells afilled
	glShadeModel(GL_SMOOTH);							//2.   Use bilinear interpolation when drawing cells as polygons

	ScalarAttributes& scalars = g.pointScalars();		//3.    Get scalar range, to normalize scalar-values next in [0,1]
	float scalar_min = scalars.getMin();
	float scalar_max = scalars.getMax();

	for(int i=0;i<g.numCells();++i)						//4. Draw all cells in the grid filled bilinearly-interpolated polygons
	{
		int   cell[10];									//Should have enough storage space for our largest cell
		
		int nvertices = g.getCell(i,cell);				//4.1. Get the 'nvertices' vertex-IDs of i-th cell
		if (nvertices!=3 && nvertices!=4)				//     We only handle here drawing of triangle and quad cells.
		   continue;									//     It is quite simple to extend this to other cell types.

		if (nvertices==3)								//     Triangle cells:
			glBegin(GL_TRIANGLES);						
		else //nvertices==4	
			glBegin(GL_QUADS);							//     Quad cells:
		
		for(int j=0;j<nvertices;++j)					//4.2. Render current cell
		{
			float x[2];
			g.getPoint(cell[j],x);						//4.3. Get vertex coordinates of j-th vertex of i-th cell	
						
			float value = (g.pointScalars().getC0Scalar(cell[j])-scalar_min)/(scalar_max-scalar_min);
														//4.4. Get vertex scalar value, normalize it in [0,1]    	
			float R,G,B;								
			mapToColor(value,R,G,B);						//4.5. Map normalized scalar to color
			glColor3f(R,G,B);							//4.6. Pass vertex color to OpenGL
			glVertex3f(x[0],x[1],0);					//4.7. Pass vertex coordinate to OpenGL
		}	
		glEnd();                                        //4.8. Finish rendering current cell
	}
}

/**
 * Draw the isolines.
 */
void ColormapRenderer::drawLines(Isolines& isolines) {
	//TODO: Pick better colors! ugggg!
	glLineWidth(2.5);
	glColor3f(0.5f,1.0f,0.0f);
	// For every isoline desired...
	for (int i = 0; i < isolines.getIsolineValues().size(); i++) {
		// For every two points in the set of points of the line...
		for (int j = 0; j < isolines.getLine(i)->numPoints() - 1; j += 2) {
			// Draw them!
			glBegin(GL_LINES);
			glVertex3f(isolines.getPoint(i, j).first, isolines.getPoint(i, j).second, 0);
			glVertex3f(isolines.getPoint(i, j+1).first, isolines.getPoint(i, j+1).second, 0);
			glEnd();
		}
	}
}

/**
 * Used to draw the grid and the isolines when
 * 'i' has been selected.
 */
void ColormapRenderer::drawWithIsolines(Grid& g, Isolines& isolines)
{
	glClearColor(1,1,1,1);								//1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (draw_style)
	{
	case DRAW_GRID:
		drawGrid(g);
		break;
	case DRAW_POINTS:
		drawPoints(g);
		break;
	case DRAW_C0_CELLS:
		drawC0Cells(g);
		break;
	case DRAW_C1_CELLS:
		drawC1Cells(g);
		break;
	}

	drawLines(isolines);

	glutSwapBuffers();
}

void ColormapRenderer::draw(Grid& g)					//Draw grid 'g' depending on drawing style attribute-value
{
	glClearColor(1,1,1,1);								//1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (draw_style)
	{
	case DRAW_GRID: 
		drawGrid(g);
		break;
	case DRAW_POINTS:
		drawPoints(g);
		break;
	case DRAW_C0_CELLS:
		drawC0Cells(g);
		//drawC0CellsWithIsoCell(g);
		//drawC0CellsWithMidpointIsoLine(g);
		//drawC0CellsWithIsoLine(g);
		break;
	case DRAW_C1_CELLS:
		drawC1Cells(g);
		break;
	}
	
	glutSwapBuffers();
}
