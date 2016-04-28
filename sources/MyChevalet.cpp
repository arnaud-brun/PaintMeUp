#include "MyChevalet.h"


MyChevalet::MyChevalet(std::string path, double scale)
{
	chevalet = new cMultiMesh();

	// load multimesh from file and scale it properly
	chevalet->loadFromFile(path);
	chevalet->scale(scale, true);
	chevalet->createBruteForceCollisionDetector();
}

void MyChevalet::setPositionAndRotation()
{
	// set the position of the object
	chevalet->setLocalPos(-0.25, 0.3, 0.0);
	chevalet->rotateAboutGlobalAxisRad(cVector3d(0, 0, 1), cDegToRad(90));
}


void MyChevalet::enableTexture()
{
	// enable the use of materials and normals
	chevalet->setUseMaterial(true);
	chevalet->computeAllNormals();

}
void MyChevalet::setHapticProperties(double maxStiff, double currStiff, double statFriction, double dynFriction)
{
	// set haptic properties
	chevalet->m_material->setStiffness(currStiff * maxStiff);
	chevalet->m_material->setStaticFriction(statFriction);
	chevalet->m_material->setDynamicFriction(dynFriction);
	chevalet->m_material->setHapticTriangleSides(true, false);
}


MyChevalet::~MyChevalet()
{
	delete chevalet;
}
