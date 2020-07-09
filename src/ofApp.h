#pragma once


#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxGuiExtended.h"
#include "Planef.h"
#include "Linef.h"
#include "Grid.h"
#include "TrackingNetworkManager.h"
#include "Frustum.h"
#include "CaptureMeshArray.h"
#include "PointCloudManager.h"
#include <ofMatrix4x4.h>

#include "DetectionMethod.h"
#ifdef BLOB
    #include "ofxRealSenseTwo.h"
    #include <librealsense2/rs.h>
    #include "BlobFinder.h"
#else
    #include "ofxNuitrack.h"
    #include <nuitrack/Nuitrack.h>
    #include "SkeletonFinder.h"
#endif

#include <boost/filesystem/operations.hpp>

#define N_CAMERAS 3

#define VIEWGRID_WIDTH  264
#define MENU_WIDTH      500
#define VIEWPORT_HEIGHT 720

#define REALSENSE_DEPTH_WIDTH   848
#define REALSENSE_DEPTH_HEIGHT  480

#define REALSENSE_VIDEO_WIDTH   848
#define REALSENSE_VIDEO_HEIGHT  480

#define MASK_UPDATE_CYCLES 100
#define N_MEASURMENT_CYCLES 10

using namespace std;
#ifdef BLOB
    using namespace ofxRealSenseTwo;
#else
    using namespace ofxnui;
#endif

//helpfull links during development:
// https://github.com/openframeworks/openFrameworks/issues/3817

class ofApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();
    void exit();
        
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);		

    vector <string> storeText;
		
    //ofxUDPManager udpConnection;

	ofTrueTypeFont  mono;
	ofTrueTypeFont  monosm;
	vector<ofPoint> stroke;

    bool bShowVisuals = false;

    //////////////////
    //    NETWORK   //
    //////////////////

    TrackingNetworkManager networkMng;
    
    //////////////////
    //OPENGL CAMERAS//
    //////////////////

    //viewports
    void initViewports();
    void setupViewports();
    
    ofRectangle viewMain;
    ofRectangle viewGrid[N_CAMERAS];

    //camera pointers
    int iMainCamera;
    
    grid mainGrid;
        
    /////////////
    //Sensor   //
    /////////////

#ifdef BLOB

    RSDevicePtr realSense;
    void createGUIDeviceParams();
    void createGUIPostProcessingParams();

    ofShader shader;
    BlobFinder tracker;
    int maskUpdatesCounter = 0;

#else

    void initNuitrack();
    TrackerRef nuitracker;
    PointCloudManager pointCloudManager;
    
    SkeletonFinder tracker;

#endif

    void reloadTransformMatrix();

    bool dispRaw;

    bool bPreviewPointCloud = false;
        
    CaptureMeshArray capMesh;
	
    void drawPreview();

#ifdef BLOB
    void drawCapturePointCloud(bool mask);
#endif
       
    // used for viewing the point cloud
    ofEasyCam previewCam;
            
    ofMatrix4x4 deviceToWorldTransform;

    //////////////
    //PROPERTIES//
    //////////////
    void setupTransformGui();

    ofxGui gui;
    
    ofxGuiPanel *setupCalib;
	ofxGuiPanel *device;
	ofxGuiPanel *post;
	ofxGuiPanel *guitransform;

	ofParameterGroup transformationGuiGroup;
    ofParameter<ofMatrix4x4> transformation;

    ofParameterGroup intrinsicGuiGroup;

    ofParameter<float> depthCorrectionBase;
    ofParameter<float> depthCorrectionDivisor;
    ofParameter<float> pixelSizeCorrector;
    ofParameter<int> blobGrain;
    ofParameter<bool> captureVideo;

    void saveSettings();
    void loadSettings();  

    //////////
    // HELP //
    //////////
    string help;

    bool bShowHelp = true;

    void createHelp();
    
#ifndef BLOB
private:
    const static ofMatrix4x4 nuitrackViewportToRealSenseViewportTransform;
    static ofMatrix4x4 makeNuitrackToRealSenseTransform();
#endif
};