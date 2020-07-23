//
//  ofApp.cpp
//  kinectTCPServer
//
//  Created by maybites on 14.02.14.
//
//	Edited by Pierre Bürki on 23.07.2020.

#include "ofApp.h"

#define RECONNECT_TIME 400

#define DEPTH_X_RES 640
#define DEPTH_Y_RES 480

#ifndef BLOB

/**
 * Create the constant nuitrack -> realsense transformation matrix
 */
ofMatrix4x4 ofApp::makeNuitrackToRealSenseTransform()
{
	// Thankfully this matrix is symmetric, so we need not worry about the row-major-ness
	// of the matrix object
	float mat[16] = {
		1e-3,  0,    0,    0,
		0,    -1e-3, 0,    0,
		0,     0,    1e-3, 0,
		0,     0,    0,    1
	};
	return glm::make_mat4x4(mat);
}

const ofMatrix4x4 ofApp::nuitrackViewportToRealSenseViewportTransform = makeNuitrackToRealSenseTransform();

/**
 * Set up Nuitrack's config and callbacks 
 */
void ofApp::initNuitrack()
{
	nuitracker = ofxnui::Tracker::create();
	nuitracker->init();

	// depth feed settings
	nuitracker->setConfigValue("Realsense2Module.Depth.FPS", "30");
	nuitracker->setConfigValue("Realsense2Module.Depth.RawWidth", "1280");
	nuitracker->setConfigValue("Realsense2Module.Depth.RawHeight", "720");
	nuitracker->setConfigValue("Realsense2Module.Depth.ProcessWidth", "1280");
	nuitracker->setConfigValue("Realsense2Module.Depth.ProcessHeight", "720");
	nuitracker->setConfigValue("Realsense2Module.Depth.ProcessMaxDepth", "7000");

	// rgb feed settings
	nuitracker->setConfigValue("Realsense2Module.RGB.FPS", "30");
	nuitracker->setConfigValue("Realsense2Module.RGB.RawWidth", "1280");
	nuitracker->setConfigValue("Realsense2Module.RGB.RawHeight", "720");
	nuitracker->setConfigValue("Realsense2Module.RGB.ProcessWidth", "1280");
	nuitracker->setConfigValue("Realsense2Module.RGB.ProcessHeight", "720");

	// feeds alignement
	nuitracker->setConfigValue("DepthProvider.Depth2ColorRegistration", "true");

	// post processing settings
	nuitracker->setConfigValue("Realsense2Module.Depth.PostProcessing.SpatialFilter.spatial_alpha", "0.1");
	nuitracker->setConfigValue("Realsense2Module.Depth.PostProcessing.SpatialFilter.spatial_delta", "50");

	// distance settings
	nuitracker->setConfigValue("Segmentation.MAX_DISTANCE", "7000");
	nuitracker->setConfigValue("Skeletonization.MaxDistance", "7000");

	nuitracker->setIssuesCallback(
		[this](nuitrack::IssuesData::Ptr data)
		{
			auto issue = data->getIssue<nuitrack::Issue>();
			if (issue)
			{
				ofLogNotice() << "Issue detected! " << issue->getName() << " [" << issue->getId() << "]";
			}
		}
	);
	nuitracker->setRGBCallback(
		[this](nuitrack::RGBFrame::Ptr data)
		{
			pointCloudManager.updateRGB(data);
		}
	);
	nuitracker->setDepthCallback(
		[this](nuitrack::DepthFrame::Ptr data)
		{
			pointCloudManager.updateDepth(data);
		}
	);
	nuitracker->setSkeletonCallback(
		[this](nuitrack::SkeletonData::Ptr data)
		{
			tracker.update(data);
		}
	);

	pointCloudManager.setDepthSensor(nuitracker->depthTracker);
}

#endif

void ofApp::initViewports() {
	float xOffset = VIEWGRID_WIDTH;
	float yOffset = VIEWPORT_HEIGHT / N_CAMERAS;

	viewMain.x = xOffset;
	viewMain.y = 0;
	viewMain.width = ofGetWidth() - xOffset - MENU_WIDTH;
	viewMain.height = VIEWPORT_HEIGHT;

	for (int i = 0; i < N_CAMERAS; i++) {

		viewGrid[i].x = 0;
		viewGrid[i].y = yOffset * i;
		viewGrid[i].width = xOffset;
		viewGrid[i].height = yOffset;
	}

	iMainCamera = 0;

	previewCam.setUpAxis(glm::vec3(0, 0, 1));
	previewCam.setTranslationSensitivity(2., 2., 2.);
	previewCam.setNearClip(0.001f);
}

void ofApp::setupTransformGui() {
	guitransform = gui.addPanel();
	guitransform->loadTheme("theme/theme_light.json");
	guitransform->setName("Transformation");

	transformationGuiGroup.setName("Matrix");
	transformationGuiGroup.add(transformation.set("Transform", ofMatrix4x4()));
	guitransform->addGroup(transformationGuiGroup);

	// This method apparently requires a reference for some reason
	bool visible = false;
	guitransform->setVisible(visible);

	reloadTransformMatrix();
}

/**
 * Load the transformation matrix from the xml file
 * Also applies the nuitrack->realsense transformation if needed
 */
void ofApp::reloadTransformMatrix()
{
	guitransform->loadFromFile("transformation.xml");

#ifdef BLOB
	deviceToWorldTransform = transformation.get();
#else
	// ofMatrices multiplication works in reverse
	deviceToWorldTransform = nuitrackViewportToRealSenseViewportTransform * transformation.get();
#endif
}

void ofApp::setup()
{

#ifdef BLOB
	if (ofIsGLProgrammableRenderer()) {
		shader.load("shadersGL3/shader");
	}
	else {
		shader.load("shadersGL2/shader");
	}

	ofLog(OF_LOG_NOTICE) << "MainAPP: looking for RealSense Device...";

	realSense = RSDevice::createUniquePtr();
	realSense->checkConnectedDialog();
	realSense->setVideoSize(REALSENSE_VIDEO_WIDTH, REALSENSE_VIDEO_HEIGHT);

	ofLog(OF_LOG_NOTICE) << "... RealSense Device found.";
#else
	ofLog(OF_LOG_NOTICE) << "Nuitrack setup started";
	initNuitrack();
	tracker.setTransformMatrix(&deviceToWorldTransform);
#endif

	ofLog(OF_LOG_NOTICE) << "Viewport setup started";
	initViewports();

	ofLog(OF_LOG_NOTICE) << "Loading GUI";
	setupTransformGui();
	tracker.initGUI(gui);

	ofLog(OF_LOG_NOTICE) << "MainAPP: starting attached Device...";

#ifdef BLOB
	if (realSense->capture())
	{
		createGUIDeviceParams();
		createGUIPostProcessingParams();
	}
#else
	nuitracker->run();
#endif
    
	ofLog(OF_LOG_NOTICE) << "MainAPP: setting up networking...";
	networkMng.setup(gui);

    setupViewports();
    
    capMesh.reSize(4);
    
	ofLog(OF_LOG_NOTICE) << "Setup over";
}

//--------------------------------------------------------------
void ofApp::setupViewports(){
	tracker.panel->setWidth(MENU_WIDTH / 2);
	tracker.panel->setPosition(ofGetWidth() - MENU_WIDTH, 20);

#ifdef BLOB
	device->setWidth(MENU_WIDTH / 2);
	device->setPosition(ofGetWidth() - MENU_WIDTH / 2, ofGetHeight() / 2);
#endif
}

#ifdef BLOB
void ofApp::createGUIDeviceParams() {
	device = gui.addPanel();

	device->clear();
	device->loadTheme("theme/theme_light.json");
	device->setName("RealSense Device");
	device->add<ofxGuiLabel>(realSense->getSerialNumber(-1));

	intrinsicGuiGroup.clear();
	intrinsicGuiGroup.setName("Settings");
	intrinsicGuiGroup.add(realSense->param_deviceLaser);
	intrinsicGuiGroup.add(realSense->param_deviceLaser_mag);
	intrinsicGuiGroup.add(realSense->param_deviceAutoExposure);
	intrinsicGuiGroup.add(realSense->param_deviceExposure_mag);
	intrinsicGuiGroup.add(realSense->param_deviceGain_mag);
	intrinsicGuiGroup.add(realSense->param_deviceFrameQueSize_mag);
	intrinsicGuiGroup.add(realSense->param_deviceAsicTemparature);
	intrinsicGuiGroup.add(realSense->param_deviceProjectorTemparature);

	device->addGroup(intrinsicGuiGroup);

	// This seems to make the app crash on Linux
	//device->loadFromFile(realSense->getSerialNumber(-1) + ".xml");
}

void ofApp::createGUIPostProcessingParams() {
	post = gui.addPanel();
	post->loadTheme("theme/theme_light.json");
	post->setName("PostProcessing");
	post->add(realSense->param_usePostProcessing);
	post->add(realSense->param_filterDecimation);
	post->add(realSense->param_filterDecimation_mag);
	post->add(realSense->param_filterDisparities);
	post->add(realSense->param_filterSpatial);
	post->add(realSense->param_filterSpatial_smoothAlpha);
	post->add(realSense->param_filterSpatial_smoothDelta);
	post->add(realSense->param_filterSpatial_mag);
	post->add(realSense->param_filterTemporal);
	post->add(realSense->param_filterTemporal_smoothAlpha);
	post->add(realSense->param_filterTemporal_smoothDelta);
	post->add(realSense->param_filterTemporal_persistency);

	post->loadFromFile("postprocessing.xml");
}
#endif

//--------------------------------------------------------------
void ofApp::update()
{

	ofBackground(100, 100, 100);

#ifdef BLOB
	if(realSense->update(ofxRealSenseTwo::PointCloud::VIDEO))
	{
		if (maskUpdatesCounter < MASK_UPDATE_CYCLES)
		{
			if (maskUpdatesCounter == 0)
			{
				tracker.clearMask();
			}
			tracker.captureMaskBegin();
			drawCapturePointCloud(true);
			tracker.captureMaskEnd();

			maskUpdatesCounter++;
		} 
		else
		{
			// Cature captureCloud to FBO
			tracker.captureBegin();
			drawCapturePointCloud(false);
			tracker.captureEnd();

			// BlobFinding on the captured FBO
			tracker.update();
		}
	}
#else
	nuitracker->poll();
#endif

	networkMng.update(tracker);
}

/**
 * Save current settings to the various xml files
 */
void ofApp::saveSettings()
{
	tracker.panel->saveToFile("tracking.xml");
	networkMng.panel->saveToFile("broadcast.xml");
	guitransform->saveToFile("transformation.xml");

#ifdef BLOB
	tracker.saveMask();
	post->saveToFile("postprocessing.xml");
	device->saveToFile(realSense->getSerialNumber(-1) + ".xml");
#endif
}

/**
 * Load the settings from the various xml files
 */
void ofApp::loadSettings()
{
	tracker.panel->loadFromFile("tracking.xml");
	networkMng.panel->loadFromFile("broadcast.xml");
	guitransform->loadFromFile("transformation.xml");
	reloadTransformMatrix();
#ifdef BLOB
	tracker.loadMask();
	post->loadFromFile("postprocessing.xml");
	// This seems to make the app crash on Linux
	//device->loadFromFile(realSense->getSerialNumber(-1) + ".xml");
#endif
}

void ofApp::draw() {
	ofSetColor(255, 255, 255);

    if(bShowVisuals){
        // Draw viewport previews
#ifdef BLOB
		realSense->drawVideoStream(viewGrid[0]);
		realSense->drawDepthStream(viewGrid[1]);
#else
		pointCloudManager.drawRGB(viewGrid[0]);
		pointCloudManager.drawDepth(viewGrid[1]);
#endif
		previewCam.begin(viewGrid[2]);
		mainGrid.drawPlane(5, 5, false);
		drawPreview();
		previewCam.end();
        
        switch (iMainCamera) {
            case 0:
#ifdef BLOB
				realSense->drawVideoStream(viewMain);
#else
				pointCloudManager.drawRGB(viewMain);
#endif
                break;
            case 1:
#ifdef BLOB
				realSense->drawDepthStream(viewMain);
#else
				pointCloudManager.drawDepth(viewMain);
#endif
                break;
            case 2:
                previewCam.begin(viewMain);
                mainGrid.drawPlane(5., 5, false);
                drawPreview();
                previewCam.end();
                break;
            default:
                break;
        }

        glDisable(GL_DEPTH_TEST);
        ofPushStyle();

        // Highlight background of selected camera
        ofSetColor(255, 0, 255, 255);
        ofNoFill();
        ofSetLineWidth(3);
        ofDrawRectangle(viewGrid[iMainCamera]);
    }    

	// draw instructions
	ofSetColor(255, 255, 255);
    
    if(bShowHelp) {
		createHelp();
        ofDrawBitmapString(help, 20 ,VIEWPORT_HEIGHT + 50);
    }

    ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), ofGetWidth() - 200, 10);

    ofPopStyle();
}

void ofApp::drawPreview() {
	glPointSize(4);
	glEnable(GL_DEPTH_TEST);

	ofPushMatrix();

	if (bPreviewPointCloud) {
		ofMultMatrix(deviceToWorldTransform);
#ifdef BLOB
		realSense->draw();
#else
		pointCloudManager.drawPointCloud();
#endif
	}
	ofPopMatrix();
	
	ofPushStyle();

    ofSetColor(255, 255, 0);
    tracker.drawSensorBox();

    ofSetColor(255, 100, 255);
#ifdef BLOB
	glLineWidth(5);
	tracker.drawBodyBlobsBox();
    tracker.drawBodyBlobsHeadTop();
#else
	ofPopMatrix();
	tracker.drawSkeletons();
#endif

	ofPopStyle();

	glDisable(GL_DEPTH_TEST);  
}

#ifdef BLOB
void ofApp::drawCapturePointCloud(bool _mask) {
    glEnable(GL_DEPTH_TEST);

	shader.begin();

	float lowerLimit = tracker.sensorBoxBottom.get() / 1000.f;
	float upperLimit = tracker.sensorBoxTop.get() / 1000.f;

	if (_mask) {
		shader.setUniform1i("mask", 1);
		glPointSize(6 * 4);
	}
	else {
		shader.setUniform1i("mask", 0);
		glPointSize(6 * 2);
	}
	shader.setUniform1f("lowerLimit", lowerLimit);
	shader.setUniform1f("upperLimit", upperLimit);
	shader.setUniformMatrix4f("viewMatrixInverse", glm::inverse(ofGetCurrentViewMatrix()));

	ofPushMatrix();
	ofMultMatrix(deviceToWorldTransform);
	realSense->draw();
	ofPopMatrix();
	
	shader.end();
	
	glDisable(GL_DEPTH_TEST);
}
#endif

void ofApp::exit() {
    ofLog(OF_LOG_NOTICE) << "exiting application...";
	
#ifdef BLOB
	realSense->stop();
#else
	// Nuitrack auto-releases on destroy ...
#endif
}

void ofApp::createHelp(){
	stringstream helpStream;
    helpStream << "press v -> to show visualizations\n";
	helpStream << "press 1 - 3 -> to change the viewport\n";
	helpStream << "press p -> to show pointcloud\n";
	helpStream << "\n";
    helpStream << "press h -> to show help \n";
	helpStream << "press l -> to load last saved settings\n";

	help = helpStream.str();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case ' ':
			break;
			
		case 'p':
			bPreviewPointCloud = !bPreviewPointCloud;
            break;
            
		case 'v':
			bShowVisuals = !bShowVisuals;
            break;
 
        case 's':
            saveSettings();
			break;

        case 'l':
            loadSettings();
			break;
           
		case 'h':
			bShowHelp = !bShowHelp;
            if (bShowHelp) {
                createHelp();
            }
			break;
            
		case '1':
            iMainCamera = 0;
			break;
			
		case '2':
            iMainCamera = 1;
			break;
			
		case '3':
            iMainCamera = 2;
			break;						            
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){


}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	stroke.push_back(ofPoint(x,y));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	setupViewports();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


