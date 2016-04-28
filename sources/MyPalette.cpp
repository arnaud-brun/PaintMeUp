#include "MyPalette.h"


MyPalette::MyPalette()
{
	// init the mesh, and the collision box
	mesh = new cMesh();
	cCreatePlane(mesh, 0.5, 0.5);
	mesh->createBruteForceCollisionDetector();
}


void MyPalette::setPositionAndRotation()
{
	// Adjust the position / rotation of the mesh
	mesh->setLocalPos(-0.25, -0.3, 0.0);
	mesh->rotateAboutGlobalAxisDeg(cVector3d(0, 1, 0), 90);
	mesh->rotateAboutGlobalAxisRad(cVector3d(1, 0, 0), cDegToRad(90));
}


int MyPalette::loadTexture(string path)
{
	// create texture and apply it to mesh
	texture = cTexture2d::create();
	mesh->setTexture(texture);

	// load texture from file
	bool fileload = mesh->m_texture->loadFromFile(path);
	if (!fileload){
		cout << "Error - Texture image '"<< path << "' failed to load correctly." << endl;
		return -1;
	}
	return 0;
}


void MyPalette::enableTexture()
{
	// we disable lighting properties for palette
	mesh->setUseMaterial(false);

	// convert palette image from RGB to RGBA
	mesh->m_texture->m_image->convert(GL_RGBA);

	// we set the white color (0xff, 0xff, 0xff) of the palette image to transparent (0x00).
	mesh->m_texture->m_image->setTransparentColor(0xff, 0xff, 0xff, 0x00);

	// enable mipmaps for adaptive texture size rendering
	mesh->m_texture->setUseMipmaps(true);

	// enable transparency for this object
	mesh->setUseTransparency(true);

	// enable texture mapping
	mesh->setUseTexture(true);
}


void MyPalette::setHapticProperties(double maxStiff, double currStiff, double statFriction, double dynFriction)
{
	// apply properties
	mesh->m_material->setStiffness(currStiff * maxStiff);
	mesh->m_material->setStaticFriction(statFriction);
	mesh->m_material->setDynamicFriction(dynFriction);
}


MyPalette::~MyPalette()
{
	delete mesh;
}
