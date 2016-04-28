#pragma once
//------------------------------------------------------------------------------
#include <iostream>
#include "chai3d.h"
#include "ArtistTools.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;


/*
 * needNormales and normalesPath seems to be useless in the haptic rendering 
 * Imported normales files generate undesirable shake effects
*/
struct aSupportProperties{
	double stiffness;
	double statFriction;
	double dynFriction;
	double textureLevel;
	string primaryPath;
	bool needNormales;
	string normalesPath;
};



class MyCanvas
{
public:
	// variables
	double stiffness, maxStiffness, statFriction, dynFriction;
	int currentSupport = 0;
	string currentSupportName = "";
	vector<aSupportProperties> multiSupports;
	cMesh* canvas;
	cImagePtr canvasOriginal;
	cImage* imageNormale;

	// reference to a pencil instance
	ArtistTools* pencilObjRef;


	// constructeur
	MyCanvas();

	// initializer
	void setBox(double, double, double);
	void setPosition(double, double, double);
	void setHapticProperties();
	void loadNextSupport();
	int loadTextureAndNormales();	
	string getCurrentSupportName();


	//Reference to artistTool instance
	void setpencilObjRef(ArtistTools*);

	// paint dispersion algorithm
	void applyPaint(cToolCursor*, cColorb, double, double, double, int);
	void applyBrush(cColorb, const int, double, double, int, int);
	void applyPencil(cColorb, const int, double, double, int, int);
	void applyRoller(cColorb, const int, double, double, int, int);



	// destrcuteur
	~MyCanvas();
};

