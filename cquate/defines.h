
#pragma once

// comment this out when building a public release
//#define _DEVELOP_VERSION_

#define BOOL bool
#define TRUE true
#define FALSE false

#define __PROGRAM_NAME "Cquate"
#define __PROGRAM_VERSION 0.5
#define __WORKSPACE_FILE_VERSION 1

#define WM_CLOSE_PLOT	(WM_USER+0)

#define COLORMAP_PATH "\\colormaps\\"

#define SCRIPT_EXTENSION ".cq"

#define DEFAULT_SURF_AZ				-45.			// about gl-Y axis
#define DEFAULT_SURF_EL				30.			// about gl-X axis
#define DEFAULT_SURF_RANGE		3.5		// dist of camera from origin

#define	DEFAULT_LIGHT_AZ 0.
#define	DEFAULT_LIGHT_EL 60.

#define D2R (PI/180.)
#define R2D (180./PI)

#define MAX_GRAPH_LABEL_DECIMALS 7

// property sheet page types
#define P_LINE		0
#define P_MESH		1
#define P_LINE_MESH	2

// plot types
#define G_EMPTY		0
#define G_PLOT		1
#define G_PLOT3		2
#define G_SURF		3
#define G_CONTOUR	4

// line types
#define L_POINT		0
#define L_LINE		1
#define L_STEM		2
#define L_BAR		3
#define L_DOTTED	4
#define L_DASH		5
#define L_DOTDASH	6

// surf types
#define S_POINT		0
#define S_LINE		1
#define S_WIREMESH	2
#define S_SOLIDMESH	3
#define S_SOLID		4
#define S_BAR		5
#define S_RIBBON	6
#define S_CONTOUR	7

// line marker types
#define M_NONE		0
#define M_X			1
#define M_PLUS		2
#define M_STAR		3
#define M_O			4
#define M_SQUARE	5

// contour plot types
#define CONTOUR_NORMAL		0
#define CONTOUR_3D			1
#define CONTOUR_SURFC		2

// color bar sizes
#define SMALL_WIDTH		40
#define MEDIUM_WIDTH	40
#define LARGE_WIDTH		60

#define SMALL_HEIGHT	240
#define MEDIUM_HEIGHT	340
#define LARGE_HEIGHT	440

// contour graph creation options
#define CONTOURLEVELS_DEFAULT		0
#define CONTOURLEVELS_NUMBER		1
#define CONTOURLEVELS_SUPPLIED		2

// enumeration of directions (contour plotting)
#define UP		0
#define DOWN	0
#define LEFT	0
#define RIGHT	0

// plot type selection (upon double-clicking in the variable view)
#define SELECT_PLOT2D		0
#define SELECT_SURF			1
#define SELECT_PCOLOR		2
#define SELECT_CONTOUR		3
#define SELECT_CONTOUR3		4
#define SELECT_SURFC		5


// display lists
#define LIST_LIGHT_ARROW 5000

// GUI kludge
#define MAX_LINE_PROP_EDIT_SIZE 50

// file header constants
#define BINARY_HEADER_VERSION_CHARS 30
#define CQ_BINARY_FILE_HEADER "Cquate v1.0.3 binary"
#define FILETYPE_BINARY 0

// matrix math error messages
#define __NO_ERRORS				0
#define SINGULAR_MATRIX			1
#define ILL_CONDITIONED			2
#define DIMENSION_MISMATCH		3
#define MEMORY_ERROR			4
