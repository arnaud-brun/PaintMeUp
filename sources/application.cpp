//==============================================================================
/*
Software License Agreement (BSD License)
Copyright (c) 2003-2016, CHAI3D.
(www.chai3d.org)

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

* Neither the name of CHAI3D nor the names of its contributors may
be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

\author    <http://www.chai3d.org>
\author    Francois Conti
\version   3.1.1 $Rev: 1925 $
*/
//==============================================================================

#pragma once
//------------------------------------------------------------------------------
#include <iostream>
#define _MSVC
#include "chai3d.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
#ifndef MACOSX
#include "GL/glut.h"
#else
#include "GLUT/glut.h"
#endif
//------------------------------------------------------------------------------

#include "MyCanvas.h"
#include "MyPalette.h"
#include "MyChevalet.h"
#include "ArtistTools.h"


//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

cStereoMode stereoMode = C_STEREO_DISABLED;
bool fullscreen = false;

//------------------------------------------------------------------------------
// DECLARED MACROS AND FUNCTIONS 
//------------------------------------------------------------------------------

static string resourceRoot; // root resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())

void resizeWindow(int w, int h);
void keySelect(unsigned char key, int x, int y);
void updateGraphics(void);
void graphicsTimer(int data);
void close(void);
void updateHaptics(void);

//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

/* World and Camera stuff */
cWorld* world;
cCamera* camera;
cDirectionalLight *light;
double cameraX = 0.8;
double cameraY = 0.0;
double cameraZ = 0.0;
double delta = 0.025; // delta of camera's translation

/* Haptic stuff */
cHapticDeviceHandler* handler;
cGenericHapticDevicePtr hapticDevice;
cToolCursor* tool;
cColorb paintColor;
double maxStiffness;
double maxDamping;

double radius = 0.01;

/* Simulation Stuff */
bool simulationRunning = false;
bool simulationFinished = true;
cFrequencyCounter frequencyCounter;

/* Computer screen and GLUT display */
int screenW;
int screenH;
int windowW;
int windowH;
int windowPosX;
int windowPosY;

/* User switches and boolean refreshment variable */
bool button0 = false;
bool button1 = false;
bool button2 = false;
bool button3 = false;
bool refresh = true;

/* Custom object for the simulation */
MyPalette* paletteObj;
ArtistTools* pencilObj;
MyChevalet* chevaletObj;
MyCanvas* canvasObj;

/* Audio features */
// audio device to play sound
cAudioDevice* audioDeviceBrush;
cAudioDevice* audioDevicePencil;
cAudioDevice* audioDeviceRoller;
// audio buffer to store a sound file
cAudioBuffer* audioBufferBrush;
cAudioBuffer* audioBufferPencil;
cAudioBuffer* audioBufferRoller;
// audio source which plays the audio buffer
cAudioSource* audioSourceBrush;
cAudioSource* audioSourcePencil;
cAudioSource* audioSourceRoller;

// usefull global variables
int idxCurrentTool = -1;
string current_mesh = "";
bool musicIsPlaying = false;



//==============================================================================
/*				  	  PAINT ME UP (on the) BEACHES 							  */
//==============================================================================

int main(int argc, char* argv[])
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Demo: 15-paint" << endl;
	cout << "Copyright 2003-2016" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[c] - Clear canvas" << endl;
	cout << "[r] - Save image to file as 'myPicture.jpg'" << endl;
	cout << "[f] - Enable/Disable full screen mode" << endl;
	cout << "[1] - Change the painting tool" << endl;
	cout << "[2] - Change the painting support" << endl;
	cout << "[x] - Exit application" << endl;
	cout << endl << endl;

	// parse first arg to try and locate resources
	resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);


	//--------------------------------------------------------------------------
	// OPEN GL - WINDOW DISPLAY
	//--------------------------------------------------------------------------

	// initialize GLUT
	glutInit(&argc, argv);

	// retrieve  resolution of computer display and position window accordingly
	screenW = glutGet(GLUT_SCREEN_WIDTH);
	screenH = glutGet(GLUT_SCREEN_HEIGHT);
	windowW = 0.8 * screenH;
	windowH = 0.8 * screenH;
	windowPosY = (screenH - windowH) / 2;
	windowPosX = windowPosY;

	// initialize the OpenGL GLUT window
	glutInitWindowPosition(windowPosX, windowPosY);
	glutInitWindowSize(windowW, windowH);

	if (stereoMode == C_STEREO_ACTIVE)
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STEREO);
	else
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// create display context and initialize GLEW library
	glutCreateWindow(argv[0]);

#ifdef GLEW_VERSION
	// initialize GLEW
	glewInit();
#endif

	// setup GLUT options
	glutDisplayFunc(updateGraphics);
	glutKeyboardFunc(keySelect);
	glutReshapeFunc(resizeWindow);
	glutSetWindowTitle("CHAI3D");

	// set fullscreen mode
	if (fullscreen)
	{
		glutFullScreen();
	}


	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	// create a new world.
	world = new cWorld();
	//world->m_backgroundColor.setWhite();
	camera = new cCamera(world);
	world->addChild(camera);

	// position and orient the camera
	camera->set(cVector3d(cameraX, cameraY, cameraZ),    // camera position (eye)
		cVector3d(0.0, 0.0, 0.0),    // lookat position (target)
		cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector

	// set the near and far clipping planes of the camera
	camera->setClippingPlanes(-10.0, 10.0);

	// set orthographic camera mode
	if (stereoMode == C_STEREO_DISABLED)
	{
		camera->setOrthographicView(1.3);
	}

	// set stereo mode
	camera->setStereoMode(stereoMode);

	// set stereo eye separation and focal length (applies only if stereo is enabled)
	camera->setStereoEyeSeparation(0.01);
	camera->setStereoFocalLength(1.0);
	
	// disable multi-pass rendering to handle transparent objects
	camera->setUseMultipassTransparency(true);

	// create a light source
	light = new cDirectionalLight(world);
	world->addChild(light);
	light->setEnabled(true);
	// define the direction of the light beam
	light->setDir(-1.0, 0.0, -0.4);


	//--------------------------------------------------------------------------
	// HAPTIC DEVICES / TOOLS
	//--------------------------------------------------------------------------

	// create a haptic device handler
	handler = new cHapticDeviceHandler();	
	handler->getDevice(hapticDevice, 0); // get access to the first available haptic device
	hapticDevice->open();

	cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();

	tool = new cToolCursor(world);
	world->addChild(tool);
	tool->setHapticDevice(hapticDevice);

	// define a radius for the tool
	double toolRadius = 0.01;
	tool->setRadius(toolRadius);

	// map the physical workspace of the haptic device to a larger virtual workspace.
	tool->setWorkspaceRadius(1.0);
	tool->setWaitForSmallForce(true); // Wait for init of haptic device
	tool->start();

	// read the scale factor between the physical workspace of the haptic
	// device and the virtual workspace defined for the tool
	double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

	// haptics special properties
	maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;
	maxDamping = hapticDeviceInfo.m_maxLinearDamping / workspaceScaleFactor;


	//--------------------------------------------------------------------------
	// CREATE OBJECTS
	//--------------------------------------------------------------------------

	/* PALETTE AND DEFAULT COLOR */
	paletteObj = new MyPalette();
	world->addChild(paletteObj->mesh);
	
	paletteObj->setPositionAndRotation();
	if (paletteObj->loadTexture(string(RESOURCE_PATH("../resources/images/palette.jpg"))) != 0){
		close();
	}
	paletteObj->enableTexture();
	paletteObj->setHapticProperties(maxStiffness, 1.0, 0.2, 0.2);

	paintColor.setBlueRoyal();
	tool->m_hapticPoint->m_sphereProxy->m_material->setColor(paintColor);


	/* CANVAS */
	canvasObj = new MyCanvas;
	world->addChild(canvasObj->canvas);

	// load haptic properties
	aSupportProperties canvasProps = {
		0.8 * maxStiffness,
		0.1,
		0.05,
		1.0,
		string(RESOURCE_PATH("../resources/images/canvas.jpg")),
		false,
		""
	};
	canvasObj->multiSupports.push_back(canvasProps);

	aSupportProperties cimentProps = {
		0.5 * maxStiffness,
		0.3,
		0.3,
		1.0,
		string(RESOURCE_PATH("../resources/images/ciment.jpg")),
		false,
		""
	};
	canvasObj->multiSupports.push_back(cimentProps);

	aSupportProperties wallProps = {
		0.5 * maxStiffness,
		0.2,
		0.2,
		0.4,
		string(RESOURCE_PATH("../resources/images/brick-color.png")),
		true,
		string(RESOURCE_PATH("../resources/images/brick-normal.png"))
	};
	canvasObj->multiSupports.push_back(wallProps);

	// finish the intialisation
	canvasObj->setBox(0.4, 0.60, 0.02);
	canvasObj->setPosition(-0.27, 0.30, 0.15);
	canvasObj->loadNextSupport();

	
	/* CHEVALET */
	std::string chevaletPath = string(RESOURCE_PATH("../resources/meshes/non-tools/chevalet.stl"));
	chevaletObj = new MyChevalet(chevaletPath, 0.02);
	world->addChild(chevaletObj->chevalet);

	chevaletObj->setPositionAndRotation();
	chevaletObj->enableTexture();
	chevaletObj->setHapticProperties(maxStiffness, 1.0, 0.2, 0.15);


	/* BACKGROUND */
	cBackground* background = new cBackground();
	background->setColor(cColorf(1.00f, 1.00f, 1.00f));
	camera->m_backLayer->addChild(background);
	background->setCornerColors(cColorf(1.00f, 1.00f, 1.00f),
		cColorf(1.00f, 1.00f, 1.00f),
		cColorf(0.80f, 0.80f, 0.80f),
		cColorf(0.80f, 0.80f, 0.80f));



	/* PENCILS AND STUFF FOR DRAWING */
	pencilObj = new ArtistTools();
	pencilObj->setHapticProperties(maxStiffness, 1.0, 0.2, 0.15);
	pencilObj->maxDamping = maxDamping;

	string toolsPath = "../resources/meshes/tools/";
	pencilObj->initMeshPathes(string(RESOURCE_PATH(toolsPath)));
	idxCurrentTool = pencilObj->loadMesh(tool, paintColor);
	current_mesh = pencilObj->meshes_path.at(idxCurrentTool);

	// add reference to canvas
	canvasObj->setpencilObjRef(pencilObj);


	//--------------------------------------------------------------------------
	// AUDIO SETUP 
	//--------------------------------------------------------------------------

	// create an audio device to play sounds
	audioDeviceBrush = new cAudioDevice();
	audioDevicePencil = new cAudioDevice();
	audioDeviceRoller = new cAudioDevice();

	// create an audio buffer and load audio wave file: 
	audioBufferBrush = audioDeviceBrush->newAudioBuffer();
	audioBufferBrush->convertToMono();
	audioBufferPencil = audioDevicePencil->newAudioBuffer();
	audioBufferRoller = audioDeviceRoller->newAudioBuffer();

	bool fileload2= audioBufferBrush->loadFromFile(RESOURCE_PATH("../resources/sounds/brush1.wav"));
	bool fileload3 = audioBufferPencil->loadFromFile(RESOURCE_PATH("../resources/sounds/pencil.wav"));
	bool fileload4 = audioBufferRoller->loadFromFile(RESOURCE_PATH("../resources/sounds/roll.wav"));
	// check for errors
	if (!(fileload2 && fileload3 && fileload4))
	{
		cout << "Error - Sounds files failed to load or initialize correctly." << endl;
		close();
		return (-1);
	}


	// create audio source
	audioSourceBrush = audioDeviceBrush->newAudioSource();
	audioSourcePencil = audioDevicePencil->newAudioSource();
	audioSourceRoller = audioDeviceRoller->newAudioSource();

	// assign auio buffer to audio source
	audioSourceBrush->setAudioBuffer(audioBufferBrush);
	audioSourcePencil->setAudioBuffer(audioBufferPencil);
	audioSourceRoller->setAudioBuffer(audioBufferRoller);

	// set volume
	audioSourceBrush->setGain(100.0);
	audioSourcePencil->setGain(100.0);
	audioSourceRoller->setGain(100.0);

	// set speed at which the audio file is played. we will modulate this with the record speed.
	audioSourceBrush->setPitch(0.2);
	audioSourcePencil->setPitch(0.2);
	audioSourceRoller->setPitch(0.2);

	// loop audio play
	audioSourceBrush->setLoop(true);
	audioSourcePencil->setLoop(true);
	audioSourceRoller->setLoop(true);

	// finish initialization
	audioSourceBrush->play();
	audioSourceBrush->pause();

	audioSourcePencil->play();
	audioSourcePencil->pause();

	audioSourceRoller->play();
	audioSourceRoller->pause();


	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------

	// create a thread which starts the main haptics rendering loop
	simulationFinished = false;
	cThread* hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup callback when application exits
	atexit(close);

	// start the main graphics rendering loop
	glutTimerFunc(50, graphicsTimer, 0);
	glutMainLoop();

	return (0);
}

//------------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
	windowW = w;
	windowH = h;
}

//------------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
	// option ESC: exit
	if ((key == 27) || (key == 'x'))
	{
		exit(0);
	}

	// option c: clear canvas
	if (key == 'c')
	{
		// copy original image of canvas to texture and update it
		canvasObj->canvasOriginal->copyTo(canvasObj->canvas->m_texture->m_image);
		canvasObj->canvas->m_texture->markForUpdate();
		cout << "> Canvas has been erased.            \r";
	}

	// option s: save canvas to file
	if (key == 'r')
	{
		// save current texture image to file
		canvasObj->canvas->m_texture->m_image->convert(GL_RGBA);
		canvasObj->canvas->m_texture->m_image->saveToFile("myPicture.png");
		cout << "> Canvas has been saved to file to myPicture.png.     \r";
	}

	// option f: toggle fullscreen
	if (key == 'f')
	{
		if (fullscreen)
		{
			windowPosX = glutGet(GLUT_INIT_WINDOW_X);
			windowPosY = glutGet(GLUT_INIT_WINDOW_Y);
			windowW = glutGet(GLUT_INIT_WINDOW_WIDTH);
			windowH = glutGet(GLUT_INIT_WINDOW_HEIGHT);
			glutPositionWindow(windowPosX, windowPosY);
			glutReshapeWindow(windowW, windowH);
			fullscreen = false;
		}
		else
		{
			glutFullScreen();
			fullscreen = true;
		}
	}

	//if ((key == 'e') || (key == '+'))
	//{
	//	camera->translate(cVector3d(10 * delta, 0, 0));
	//	tool->translate(cVector3d(10 * delta, 0, 0));
	//}
	//if ((key == 'a') || (key == '-'))
	//{
	//	camera->translate(cVector3d(10 * -delta, 0, 0));
	//	tool->translate(cVector3d(10 * -delta, 0, 0));
	//}
	//if (key == 'z')
	//{
	//	camera->translate(cVector3d(0, 0, delta));
	//	tool->translate(cVector3d(0, 0, delta));
	//}
	//if (key == 's')
	//{
	//	camera->translate(cVector3d(0, 0, -delta));
	//	tool->translate(cVector3d(0, 0, -delta));
	//}
	//if (key == 'q')
	//{
	//	camera->translate(cVector3d(0, -delta, 0));
	//	tool->translate(cVector3d(0, -delta, 0));
	//}
	//if (key == 'd')
	//{
	//	//cameraY -= 0.1;
	//	camera->translate(cVector3d(0, delta, 0));
	//	tool->translate(cVector3d(0, delta, 0));
	//}

	if (key == '1')
	{
		idxCurrentTool = pencilObj->loadMesh(tool, paintColor);
		current_mesh = pencilObj->meshes_path.at(idxCurrentTool);
	}

	if (key == '2')
	{
		canvasObj->loadNextSupport();
	}
		
}

//------------------------------------------------------------------------------

void close(void)
{
	// Stop the simulation
	simulationRunning = false;
	while (!simulationFinished) { cSleepMs(100); }

	// destroy all objects :
	tool->stop();
	delete paletteObj;
	delete pencilObj;
	delete chevaletObj;
	delete canvasObj;
	delete audioDeviceBrush;
	delete audioDevicePencil;
	delete audioDeviceRoller;
}

//------------------------------------------------------------------------------

void graphicsTimer(int data)
{
	if (simulationRunning)
		glutPostRedisplay();

	glutTimerFunc(50, graphicsTimer, 0);
}

//------------------------------------------------------------------------------

void updateGraphics(void)
{	
	/* Render scene */
	world->updateShadowMaps(false); // update shadow maps (if any)
	camera->renderView(windowW, windowH);
	
	glutSwapBuffers(); // swap buffers	
	glFinish(); // wait until all GL commands are completed

	// check for any OpenGL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
	refresh = true;
}

//------------------------------------------------------------------------------

void updateHaptics(void)
{
	// reset clock
	cPrecisionClock clock;
	clock.reset();

	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// main haptic simulation loop
	while (simulationRunning)
	{
		/* Simulation time */		
		clock.stop(); // stop the simulation clock		
		double timeInterval = clock.getCurrentTimeSeconds(); // read the time increment in seconds

		// restart the simulation clock
		clock.reset();
		clock.start();


		/* Haptic force computation */

		// compute global reference frames for each object
		world->computeGlobalPositions(true);

		// update position and orientation of tool
		tool->updateFromDevice();

		// compute interaction forces
		tool->computeInteractionForces();
		double force = tool->getDeviceGlobalForce().length();

		// send forces to haptic device
		tool->applyToDevice();
		hapticDevice->getUserSwitch(0, button0);
		hapticDevice->getUserSwitch(1, button1);
		hapticDevice->getUserSwitch(2, button2);
		hapticDevice->getUserSwitch(3, button3);


		/* User haptic device interface */
		//refresh is here to hanlde action once
		if (refresh) {
			if (button0)
			{

			}
			else if (button1 || button3)
			{
				// Increase or decrease the size of the tool (radius of the tool)s
				button1 ? radius -= 0.0001 : radius += 0.0001;
				tool->setRadius(radius);
				button1 ? pencilObj->updateScale(1 - radius*0.75) : pencilObj->updateScale(1 + radius*0.75);
				refresh = false;
			}
			else if (button2)
			{

			}
			else if (button3)
			{
				
			}
			else
			{
			}		
		}


		/////////////////////////////////////////////////////////////////////
		// INTERACTION WITH PALETTE
		/////////////////////////////////////////////////////////////////////

		if (tool->isInContact(paletteObj->mesh))
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
				paletteObj->mesh->m_texture->m_image->getPixelLocation(texCoord, px, py);

				// retrieve color information at pixel
				paletteObj->mesh->m_texture->m_image->getPixelColor(px, py, paintColor);

				// update color of tool
				tool->m_hapticPoint->m_sphereProxy->m_material->setColor(paintColor);
				pencilObj->currentTool->getMesh(0)->m_material->setColor(paintColor);
			}
		}


		/////////////////////////////////////////////////////////////////////
		// INTERACTION WITH CANVAS
		/////////////////////////////////////////////////////////////////////

		if (tool->isInContact(canvasObj->canvas))
		{

			// if the resource is the brick, aply paint with radius/4 because brick resource is 256*256
			if (canvasObj->getCurrentSupportName().find("brick") != std::string::npos)
			{
				canvasObj->applyPaint(tool, paintColor, radius/4, force, timeInterval / 7, idxCurrentTool);
			}
			else{
				// classical resources : 1024 * 1024
				canvasObj->applyPaint(tool, paintColor, radius, force, timeInterval / 7, idxCurrentTool);
			}

			// handle the good sound
			if (!musicIsPlaying)
			{
				musicIsPlaying = true;
				if (current_mesh.find("brush") != std::string::npos){
					audioSourceBrush->play();
					audioSourcePencil->pause();
					audioSourceRoller->pause();
				}
				else if (current_mesh.find("pencil") != std::string::npos){
					audioSourceBrush->pause();
					audioSourcePencil->play();
					audioSourceRoller->pause();
				}
				else if (current_mesh.find("roller") != std::string::npos){
					audioSourceBrush->pause();
					audioSourcePencil->pause();
					audioSourceRoller->play();
				}
			}
		}
		else 
		{
			// stop sound if necessary
			if (musicIsPlaying) 
			{
				musicIsPlaying = false;
				if (current_mesh.find("brush") != std::string::npos){
					audioSourceBrush->pause();
					if (audioSourcePencil->isPlaying()) audioSourcePencil->pause();
					if (audioSourceRoller->isPlaying()) audioSourceRoller->pause();
				}
				else if (current_mesh.find("pencil") != std::string::npos){
					audioSourcePencil->pause();
					if (audioSourceBrush->isPlaying()) audioSourceBrush->pause();
					if (audioSourceRoller->isPlaying()) audioSourceRoller->pause();
				}
				else if (current_mesh.find("roller") != std::string::npos){
					audioSourceRoller->pause();
					if (audioSourceBrush->isPlaying()) audioSourceBrush->pause();
					if (audioSourcePencil->isPlaying()) audioSourcePencil->pause();
					
				}
			}			
		}

		// update frequency counter
		frequencyCounter.signal(1);
	}

	// exit haptics thread
	simulationFinished = true;
}

