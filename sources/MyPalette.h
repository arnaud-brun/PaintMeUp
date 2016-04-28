#pragma once
//------------------------------------------------------------------------------
#include <iostream>
#include "chai3d.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;

class MyPalette
{
public:
	//membres
	cMesh* mesh;
	cTexture2dPtr texture;

	//fonctions
	MyPalette();
	void setPositionAndRotation();
	int loadTexture(string path);
	void enableTexture();
	void setHapticProperties(double, double, double, double);
	~MyPalette();
};

