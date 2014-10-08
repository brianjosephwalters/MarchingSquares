#pragma once

#include "Renderer.h"
#include "Isolines.h"
#include <vector>

using namespace std;


class ColormapRenderer : public Renderer
{
public:

enum DRAW_STYLE
{
    DRAW_GRID     =0,					//Draw the grid only. See the SimpleRenderer class.
	DRAW_POINTS,						//Draw points, color-mapped by their scalar value
	DRAW_C0_CELLS,						//Draw cells, color-mapped by their scalar value (computed by averaging vertex scalars)
	DRAW_C1_CELLS						//Draw cells, color-mapped by their vertex scalars, using bilinear interpolation
};	


enum COLORMAP_TYPE
{
    COLORMAP_GRAYSCALE =0,				//Classical grayscale (luminance) colormap
	COLORMAP_RAINBOW,					//Classical rainbow colormap
	COLORMAP_SATURATION					//Colormap obtained from all saturation levels of a given hue (green)
};	

				ColormapRenderer(): draw_style(DRAW_GRID), colormap_type(COLORMAP_GRAYSCALE) {}

void			setDrawingStyle(DRAW_STYLE s)
				{ draw_style = s; }

void			setColormapType(COLORMAP_TYPE c)
				{ colormap_type = c; }
				

void			draw(Grid&);
void 			drawWithIsolines(Grid& g, Isolines& isolines);


protected:

DRAW_STYLE		draw_style;
COLORMAP_TYPE	colormap_type;
void			mapToColor(float v,float& R,float& G,float& B);

private:

void			drawGrid(Grid& g);
void			drawPoints(Grid& g);
void			drawC0Cells(Grid& g);
void			drawC0CellsWithIsoCell(Grid& grid);
void			drawC0CellsWithMidpointIsoLine(Grid& grid);
void			drawC0CellsWithIsoLine(Grid& grid);
void 			drawIsoLineForCell(float points[][2], float values[], float isoValue);
void			drawC1Cells(Grid& g);

void 			drawLines(Isolines& isolines);
};



						

