#pragma once
//------------------------------------------------------------------------------
#include <iostream>
#include "chai3d.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;


class ArtistTools
{
public:
	// membres
	int idxToolToSelect;
	double stiffness, maxStiffness, statFriction, dynFriction;
	double maxDamping;
	std::vector<string> meshes_path;
	cMultiMesh* currentTool;


	ArtistTools();
	void setHapticProperties(double, double, double, double);
	void initMeshPathes(string);
	int loadMesh(cToolCursor*, cColorb);
	void updateScale(double);
	~ArtistTools();
};

