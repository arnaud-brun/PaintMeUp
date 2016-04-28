#pragma once
//------------------------------------------------------------------------------
#include <iostream>
#include "chai3d.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;


class MyChevalet
{
public:
	// membre
	cMultiMesh* chevalet;

	// fonctions
	MyChevalet(std::string, double);
	void setPositionAndRotation();
	void enableTexture();
	void setHapticProperties(double, double, double, double);
	~MyChevalet();
};

