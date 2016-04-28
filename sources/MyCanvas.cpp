#include "MyCanvas.h"


MyCanvas::MyCanvas()
{
	canvas = new cMesh();
	imageNormale = new cImage();
}

void MyCanvas::setBox(double x, double y, double z)
{
	cCreateBox(canvas, x, y, z);
	canvas->createBruteForceCollisionDetector();
}

void MyCanvas::setPosition(double x, double y, double z)
{
	// set the position of the object
	canvas->setLocalPos(x, y, z);
	canvas->rotateAboutGlobalAxisRad(cVector3d(0, 1, 0), cDegToRad(90));
	canvas->rotateAboutGlobalAxisRad(cVector3d(1, 0, 0), cDegToRad(90));
	canvas->rotateAboutGlobalAxisRad(cVector3d(0, -1, 0), cDegToRad(16));

}


void MyCanvas::setHapticProperties()
{
	canvas->m_material->setStiffness(multiSupports.at(currentSupport).stiffness);
	canvas->m_material->setStaticFriction(multiSupports.at(currentSupport).statFriction);
	canvas->m_material->setDynamicFriction(multiSupports.at(currentSupport).dynFriction);
	canvas->m_material->setTextureLevel(multiSupports.at(currentSupport).textureLevel);
	canvas->m_material->setHapticTriangleSides(true, false);
}

void MyCanvas::loadNextSupport(){

	// load the next texture, and update haptics
	loadTextureAndNormales();
	setHapticProperties();

	//update support name and index for the following texture
	currentSupportName = multiSupports.at(currentSupport).primaryPath;
	currentSupport = (currentSupport + 1) % multiSupports.size();
}





int MyCanvas::loadTextureAndNormales()
{
	// set texture
	canvas->m_texture = cTexture2d::create();


	string texturePath = multiSupports.at(currentSupport).primaryPath;
	bool fileload = canvas->m_texture->loadFromFile(texturePath);
	if (!fileload)
	{
		cout << "Canvas object : loading " << texturePath << "fails !";
		return (-1);
	}

	// enable the copy of convas
	canvasOriginal = canvas->m_texture->m_image->copy();
	
	// enable texture mapping
	canvas->setUseTexture(true);
	canvas->m_material->setWhite();
	
	// convert palette image from RGB to RGBA
	canvas->m_texture->m_image->convert(GL_RGBA);

	// we set the white color (0xff, 0xff, 0xff) of the palette image to transparent (0x00).
	canvas->m_texture->m_image->setTransparentColor(0xff, 0xff, 0xff, 0x00);
	canvas->m_texture->m_image->setTransparency(0x00);


	//////////////////////////////////////////
	//////////////////////////////////////////

	// set normal map
	cNormalMapPtr normalMap = cNormalMap::create();

	bool useFile = multiSupports.at(currentSupport).needNormales;
	if (useFile)
	{
		// get back normal map from file
		string normalesPath = multiSupports.at(currentSupport).normalesPath;
		fileload = normalMap->loadFromFile(normalesPath);
		if (!fileload)
		{
			cout << "Canvas object : loading " << normalesPath << "fails !";
			return (-2);
		}
	}
	else{
		// create normal map from texture data	
		normalMap->createMap(canvas->m_texture);
		normalMap->setTextureUnit(GL_TEXTURE0_ARB);
	}

	// bind normal map to canvas and do stuff
	canvas->m_normalMap = normalMap;
	canvas->computeAllNormals();
	canvas->m_triangles->computeBTN();	// compute tangent vectors


	return 0;
}

string MyCanvas::getCurrentSupportName()
{
	return currentSupportName;
}


void MyCanvas::setpencilObjRef(ArtistTools* reference)
{
	pencilObjRef = reference;
}


void MyCanvas::applyPaint(cToolCursor* tool, cColorb paintColor, double radius, double force, double timeInterval, int currentMeshSelected)
{
	cCollisionEvent* contact = tool->m_hapticPoint->getCollisionEvent(0);
	if (contact != NULL)
	{
		// retrieve contact information
		cVector3d localPos = contact->m_localPos;
		unsigned int triangleIndex = contact->m_index;
		cTriangleArrayPtr triangles = contact->m_triangles;

		// retrieve texture coordinate
		cVector3d texCoord = triangles->getTexCoordAtPosition(triangleIndex, localPos);

		// retrieve pixel information
		int px, py;
		canvas->m_texture->m_image->getPixelLocation(texCoord, px, py);

		// paint color at tool position
		const double K_INK = 30;
		const double K_SIZE = 10;
		const int BRUSH_SIZE = radius * 2500;

		// apply the correct paint algorithm given the index of mesh
		string meshName = pencilObjRef->meshes_path.at(currentMeshSelected);
		if (meshName.find("roller") != std::string::npos)
		{
			applyRoller(paintColor, BRUSH_SIZE, force, timeInterval, px, py);
		}
		else if (meshName.find("brush") != std::string::npos)
		{
			applyBrush(paintColor, BRUSH_SIZE, force, timeInterval, px, py);
		}
		else if (meshName.find("pencil") != std::string::npos)
		{
			applyPencil(paintColor, BRUSH_SIZE, force, timeInterval, px, py);
		}
		else{
			cout << "Apply Painting : association with " << meshName << " is not found" << endl;
		}

		// update texture
		canvas->m_texture->markForUpdate();
	}
}


void MyCanvas::applyRoller(cColorb paintColor, const int BRUSH_SIZE, double force, double timeInterval, int px, int py)
{
	//cout << "roller " << endl;
	
	const double K_INK = 30;
	double sizeX = (double)(BRUSH_SIZE)* 12;
	double sizeY = (double)(BRUSH_SIZE);
	
	for (int x = -BRUSH_SIZE * 6; x<BRUSH_SIZE * 6; x++)
	{
		for (int y = -BRUSH_SIZE; y<BRUSH_SIZE; y++)
		{

			if (x <= sizeX && x >= -sizeX && y <= sizeY && y >= -sizeY)
			{
				// get color at location
				cColorb color, newColor;
				canvas->m_texture->m_image->getPixelColor(px + x, py + y, color);

				// compute color factor based of pixel position and force interaction 
				double factor = cClamp(K_INK * timeInterval * cClamp(force, 0.0, 10.0), 0.0, 1.0);

				// compute new color
				newColor.setR((1.0 - factor) * color.getR() + factor * paintColor.getR());
				newColor.setG((1.0 - factor) * color.getG() + factor * paintColor.getG());
				newColor.setB((1.0 - factor) * color.getB() + factor * paintColor.getB());

				// assign new color to pixel
				canvas->m_texture->m_image->setPixelColor(px + x, py + y, newColor);
			}
		}
	}
}


void MyCanvas::applyBrush(cColorb paintColor, const int BRUSH_SIZE, double force, double timeInterval, int px, int py)
{

	//cout << "brush " << endl;

	const double K_INK = 30;
	const double K_SIZE = 10;
	double sizeX = cClamp((K_SIZE * force), 0.0, (double)(BRUSH_SIZE) / 2);
	double sizeY = cClamp((K_SIZE * force), 0.0, (double)(BRUSH_SIZE)* 3);
	for (int x = -BRUSH_SIZE; x<BRUSH_SIZE; x++)
	{
		for (int y = -BRUSH_SIZE; y<BRUSH_SIZE; y++)
		{
			// compute new color percentage

			if (x <= sizeX && x >= -sizeX && y <= sizeY && y >= -sizeY)
			{
				// get color at location
				cColorb color, newColor;
				canvas->m_texture->m_image->getPixelColor(px + x, py + y, color);

				// compute color factor based of pixel position and force interaction 
				double factor = cClamp(K_INK * timeInterval * cClamp(force, 0.0, 10.0), 0.0, 1.0);

				// compute new color
				newColor.setR((1.0 - factor) * color.getR() + factor * paintColor.getR());
				newColor.setG((1.0 - factor) * color.getG() + factor * paintColor.getG());
				newColor.setB((1.0 - factor) * color.getB() + factor * paintColor.getB());
				newColor.setf(newColor.getColorf().getR(), newColor.getColorf().getG(), newColor.getColorf().getB(), 0.5f);

				// assign new color to pixel
				canvas->m_texture->m_image->setPixelColor(px + x, py + y, newColor);
			}
		}
	}
}



void MyCanvas::applyPencil(cColorb paintColor, const int BRUSH_SIZE, double force, double timeInterval, int px, int py)
{

	//cout << "pencil " << endl;

	const double K_INK = 30;
	const double K_SIZE = 10;
	double size = cClamp((K_SIZE * force), 0.0, (double)(BRUSH_SIZE));
	for (int x = -BRUSH_SIZE; x<BRUSH_SIZE; x++)
	{
		for (int y = -BRUSH_SIZE; y<BRUSH_SIZE; y++)
		{
			// compute new color percentage
			double distance = sqrt((double)(x*x + y*y));
			if (distance <= size)
			{
				// get color at location
				cColorb color, newColor;
				canvas->m_texture->m_image->getPixelColor(px + x, py + y, color);

				// compute color factor based of pixel position and force interaction 
				double factor = cClamp(K_INK * timeInterval * cClamp(force, 0.0, 10.0) * cClamp(1 - distance / size, 0.0, 1.0), 0.0, 1.0);

				// compute new color
				newColor.setR((1.0 - factor) * color.getR() + factor * paintColor.getR());
				newColor.setG((1.0 - factor) * color.getG() + factor * paintColor.getG());
				newColor.setB((1.0 - factor) * color.getB() + factor * paintColor.getB());

				// assign new color to pixel
				canvas->m_texture->m_image->setPixelColor(px + x, py + y, newColor);
			}
		}
	}
}




MyCanvas::~MyCanvas()
{
	delete imageNormale;
	delete canvas;
}
