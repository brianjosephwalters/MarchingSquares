/*
 * MarchingSquares.cpp
 * Bogarted from SampledStructuredGridScalars.
 *
 *  Created on: Sep 19, 2014
 *      Author: bjw
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>
#include <iostream>
#include <vector>
#include "UniformGrid.h"
#include "Isolines.h"
#include "ColormapRenderer.h"

float xMin = -5, xMax = 5;
float yMin = -5, yMax = 5;
int   nX   = 15, nY   = 15;


float fov 	 = 60;
float aspect = 1;
float z_near = 0.1;
float z_far  = 30;

float eye_x = 10, eye_y = 10, eye_z = 10,
	  c_x   = 0,  c_y   = 0,  c_z   = 0,
	  up_x  = 0,  up_y  = 0,  up_z  = 1;

UniformGrid				grid(nX, nY, xMin, yMin, xMax, yMax);
float					isoValues[] = {1.8};
Isolines				isolines(isoValues, 2);

ColormapRenderer 		renderer;
int						drawing_style = 0;
int						colormap      = 0;

bool					drawIsoline = false;

unsigned getCase(float values[], float isoValue);
pair<float, float> getEdgeVertex(float point1[2], float point2[2], float value1, float value2, float isoValue);
pair<float, float> getBottomEdgeVertex(float points[][2], float values[], float isoValue);
pair<float, float> getTopEdgeVertex(float points[][2], float values[], float isoValue);
pair<float, float> getRightEdgeVertex(float points[][2], float values[], float isoValue);
pair<float, float> getLeftEdgeVertex(float points[][2], float values[], float isoValue);

float f(float x, float y)
{
	float ret = 8*expf(-(x*x+y*y)/5);
	return ret;
}

float f1(float x, float y) {
	float ret = x * sin(y)/.5f +1.0f;
	return ret;
}

void createUniformGrid()
{
	for (int i = 0; i < grid.numPoints(); i++)
	{
		float p[2];
		grid.getPoint(i, p);
		float value = f(p[0], p[1]);

		grid.pointScalars().setC0Scalar(i, value);
	}
}

/**
 * Preprocess the isoline verticies
 */
void createMarchingSquares()
{
	for (int l = 0; l < isolines.numLines(); l++)
	{
		Isoline * line = new Isoline();
		float isoValue = isolines.getIsolineValue(l);
		for (int i = 0; i < grid.numCells(); i++)
		{
			int cellHolder[4];
			int numVertices = grid.getCell(i, cellHolder);
			float points[4][2];
			float values[4];
			for (int j = 0; j < numVertices; j++)
			{
				grid.getPoint(cellHolder[j], points[j]);
				values[j] = grid.pointScalars().getC0Scalar(cellHolder[j]);
			}

			unsigned bits = getCase(values, isoValue);
			// TODO: Remove repeated cases
			switch((int)bits) {
				case 0: //0000
					break;
				case 1: //0001
					line->addPoint(getBottomEdgeVertex(points, values, isoValue));
					line->addPoint(getLeftEdgeVertex(points, values, isoValue));
					break;
				case 2: //0010
					line->addPoint(getBottomEdgeVertex(points, values, isoValue));
					line->addPoint(getRightEdgeVertex(points, values, isoValue));
					break;
				case 3: //0011
					line->addPoint(getLeftEdgeVertex(points, values, isoValue));
					line->addPoint(getRightEdgeVertex(points, values, isoValue));
					break;
				case 4: //0100
					line->addPoint(getRightEdgeVertex(points, values, isoValue));
					line->addPoint(getTopEdgeVertex(points, values, isoValue));
					break;
				case 5: //0101
					line->addPoint(getRightEdgeVertex(points, values, isoValue));
					line->addPoint(getTopEdgeVertex(points, values, isoValue));
					line->addPoint(getBottomEdgeVertex(points, values, isoValue));
					line->addPoint(getLeftEdgeVertex(points, values, isoValue));
					break;
				case 6: //0110
					line->addPoint(getBottomEdgeVertex(points, values, isoValue));
					line->addPoint(getTopEdgeVertex(points, values, isoValue));
					break;
				case 7: //0111
					line->addPoint(getTopEdgeVertex(points, values, isoValue));
					line->addPoint(getLeftEdgeVertex(points, values, isoValue));
					break;
				case 8: //1000
					line->addPoint(getTopEdgeVertex(points, values, isoValue));
					line->addPoint(getLeftEdgeVertex(points, values, isoValue));
					break;
				case 9: //1001
					line->addPoint(getBottomEdgeVertex(points, values, isoValue));
					line->addPoint(getTopEdgeVertex(points, values, isoValue));
					break;
				case 10://1010
					line->addPoint(getLeftEdgeVertex(points, values, isoValue));
					line->addPoint(getTopEdgeVertex(points, values, isoValue));
					line->addPoint(getBottomEdgeVertex(points, values, isoValue));
					line->addPoint(getRightEdgeVertex(points, values, isoValue));
					break;
				case 11://1011
					line->addPoint(getRightEdgeVertex(points, values, isoValue));
					line->addPoint(getTopEdgeVertex(points, values, isoValue));
					break;
				case 12://1100
					line->addPoint(getLeftEdgeVertex(points, values, isoValue));
					line->addPoint(getRightEdgeVertex(points, values, isoValue));
					break;
				case 13://1101
					line->addPoint(getBottomEdgeVertex(points, values, isoValue));
					line->addPoint(getRightEdgeVertex(points, values, isoValue));
					break;
				case 14://1110
					line->addPoint(getBottomEdgeVertex(points, values, isoValue));
					line->addPoint(getLeftEdgeVertex(points, values, isoValue));
					break;
				case 15://1111
					break;
				default:
					cout << "WTF!?";
			}
		}
		isolines.setLine(l, line);
	}
}

pair<float, float> getEdgeVertex(float point1[2], float point2[2], float value1, float value2, float isoValue)
{
	pair<float, float> p = pair<float, float>();
	if (value1 < value2) {
		p.first = (point1[0] * (value2 - isoValue) + point2[0] * (isoValue - value1)) / (value2 - value1);
		p.second = (point1[1] * (value2 - isoValue) + point2[1] * (isoValue - value1)) / (value2 - value1);

	} else {
		p.first = (point2[0] * (value1 - isoValue) + point1[0] * (isoValue - value2)) / (value1 - value2);
		p.second = (point2[1] * (value1 - isoValue) + point1[1] * (isoValue - value2)) / (value1 - value2);
	}
	return p;
}

pair<float, float> getBottomEdgeVertex(float points[][2], float values[], float isoValue)
{
	return getEdgeVertex(points[0], points[1], values[0], values[1], isoValue);
}
pair<float, float> getRightEdgeVertex(float points[][2], float values[], float isoValue)
{
	return getEdgeVertex(points[1], points[2], values[1], values[2], isoValue);
}
pair<float, float> getTopEdgeVertex(float points[][2], float values[], float isoValue)
{
	return getEdgeVertex(points[2], points[3], values[2], values[3], isoValue);
}
pair<float, float> getLeftEdgeVertex(float points[][2], float values[], float isoValue)
{
	return getEdgeVertex(points[3], points[0], values[3], values[0], isoValue);
}

unsigned getCase(float values[], float isoValue) {
	unsigned bits = 0;
	for (int i = 0; i < 4; i++) {
		if (values[i] > isoValue)
			bits |= 1 << i;
	}
	return bits;
}

void draw()
{
	if (drawIsoline) {
		renderer.drawWithIsolines(grid, isolines);
	} else {
		renderer.draw(grid);
	}
}

void keyboard(unsigned char c, int, int)
{
	switch(c)
	{
	case ' ':
		{
		  drawing_style = (++drawing_style) % 4;		//Ask renderer to use another drawing-style mode
		  renderer.setDrawingStyle((ColormapRenderer::DRAW_STYLE)drawing_style);
		  break;
		}
	case 'c':
		{
		  colormap = (++colormap) % 3;					//Ask renderer to use another colormap
		  renderer.setColormapType((ColormapRenderer::COLORMAP_TYPE)colormap);
		  break;
		}
	case 'i':
		{
		  if (drawIsoline)
		  {
			  drawIsoline = false;

		  }
		  else
		  {
			  drawIsoline = true;
		  }
		}
	}

	glutPostRedisplay();
}

void viewing(int W, int H)
{
	cout<<"My Marching Squares program"<<endl;
	cout<<"In the graphics window, press"<<endl;
	cout<<"  space: to toggle through various color-mapping modes."<<endl;
	cout<<"  c: to toggle through various color maps."<<endl;
	cout<<"  i: to draw isolines."<<endl;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye_x, eye_y, eye_z,
			  c_x,   c_y,   c_z,
			  up_x,  up_y,  up_z);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, float(W) / H, z_near, z_far);
	glViewport(0, 0, W, H);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);								//1. Initialize the GLUT toolkit
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);		//2. Ask GLUT to create next windows with a RGB framebuffer and a Z-buffer too
	glutInitWindowSize(500,500);						//3. Tell GLUT how large are the windows we want to create next
	glutCreateWindow("2. Structured grid (color mapping scalars)");
														//4. Create our window

	createUniformGrid();								//5. Create the structured grid and add scalar values to its vertices
	createMarchingSquares();

	glutKeyboardFunc(keyboard);
	glutDisplayFunc(draw);								//6. Add a drawing callback to the window
	glutReshapeFunc(viewing);							//7. Add a resize callback to the window
	glutMainLoop();										//8. Start the event loop that displays the graph and handles window-resize events

	return 0;
}
