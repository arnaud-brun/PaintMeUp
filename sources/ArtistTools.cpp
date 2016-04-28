#include "ArtistTools.h"


ArtistTools::ArtistTools()
{	

}



void ArtistTools::setHapticProperties(double maxStiff, double stiff, double statFric, double dynFric)
{
	maxStiffness = maxStiff;
	stiffness = stiff;
	statFriction = statFric;
	dynFriction = dynFric;
}




void ArtistTools::initMeshPathes(string toolsPath)
{
	// save meshes entries into vector
	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile((toolsPath + "*").c_str(), &fd);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			
			if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {								
				string fileName = string(toolsPath + fd.cFileName);
				meshes_path.push_back(fileName);
				cout << "[..]"<< fd.cFileName << " pushed into vector" << endl;
			}

		} while (FindNextFile(h, &fd));
		FindClose(h);
	}

	int count = meshes_path.size();
	printf("Files count in directory : %d\n", count);

	// Set up the first mesh to pick up
	idxToolToSelect = 0;
}


int ArtistTools::loadMesh(cToolCursor* tool, cColorb paintColor)
{
	 
	// remove old childrend of tools
	tool->deleteAllChildren();
	//tool->removeChild(currentTool);

	// add a new fresh and reliable child
	currentTool = new cMultiMesh();
	string toolPath = meshes_path.at(idxToolToSelect);
	currentTool->loadFromFile(toolPath);


	double goodScale = tool->m_hapticPoint->getRadiusContact();
	currentTool->scale(2.5 * goodScale);
	currentTool->setLocalTransform(tool->getLocalTransform());
	currentTool->getMesh(0)->m_material->setColor(paintColor);

	/* do some haptic specifities */
	if (toolPath.find("brush") != std::string::npos) {
		currentTool->createEffectSurface();
		currentTool->createEffectVibration();
		currentTool->deleteEffectViscosity();
	}
	else if (toolPath.find("encil") != std::string::npos) {		
		currentTool->deleteEffectSurface();
		currentTool->deleteEffectVibration();
		currentTool->m_material->setViscosity(1.5 * maxDamping);
		currentTool->createEffectViscosity();
	}
	else if (toolPath.find("oller") != std::string::npos) {
		currentTool->deleteEffectSurface();
		currentTool->deleteEffectVibration();
		currentTool->m_material->setViscosity(0.9 * maxDamping);
		currentTool->createEffectViscosity();
	}		

	// assign pencil to tool
	tool->m_image = currentTool;
	tool->addChild(currentTool);
	tool->setShowEnabled(false, false);

	// preserve index of current mesh and update the index for the future mesh
	int idxCurrentTool = idxToolToSelect;
	idxToolToSelect = (idxToolToSelect + 1) % meshes_path.size();

	return idxCurrentTool;
}

void ArtistTools::updateScale(double scale)
{
	// set new scale and update the value registered
	currentTool->scale(scale);
}


ArtistTools::~ArtistTools()
{
	delete currentTool;
}
