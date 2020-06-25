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

#ifdef BLOB
    #include "ofxRealSenseTwo.h"
    #include <librealsense2/rs.h>
    #include "BlobFinder.h"
#else
    #include "ofxNuitrack.h"
    #include <nuitrack/Nuitrack.h>
    #include "SkeletonFinder.h"
#endif

#include <ofMatrix4x4.h>

#define N_CAMERAS 3

#define VIEWGRID_WIDTH  264
#define MENU_WIDTH      500
#define VIEWPORT_HEIGHT 720

#define REALSENSE_DEPTH_WIDTH   848
#define REALSENSE_DEPTH_HEIGHT  480

#define REALSENSE_VIDEO_WIDTH   848
#define REALSENSE_VIDEO_HEIGHT  480

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
    ofCamera * cameras[N_CAMERAS];
    int iMainCamera;

    ofEasyCam cam;
    
    grid mainGrid;
    
    shared_ptr<ofBaseGLRenderer> opengl;
    shared_ptr<ofCairoRenderer> cairo;
    ofTexture render;
    
    /////////////
    //Sensor   //
    /////////////

#ifdef BLOB

    RSDevicePtr realSense;
    ofShader shader;

    BlobFinder tracker;

#else

    void initNuitrack();
    TrackerRef nuitracker;
    PointCloudManager pointCloudManager;
    
    SkeletonFinder tracker;

#endif

    void reloadTransformMatrix();

    bool dispRaw;

    bool bPreviewPointCloud;
    
    ofVboMesh previewmesh;//, capturemesh;
    
    CaptureMeshArray capMesh;
	
    void drawPreview();

    /////////////////
    //COLOR CONTOUR//
    /////////////////
       
    // used for viewing the point cloud
    ofEasyCam previewCam;
            
    ofMatrix4x4 deviceToWorldTransform;
    ofMatrix4x4 worldToDeviceTransform;

    //////////////
    //PROPERTIES//
    //////////////
    void setupTransformGui();

    ofxGui gui;
    
	ofxGuiPanel *guitransform;
	ofParameterGroup transformationGuiGroup;
    ofParameter<ofMatrix4x4> transformation;
    
    //////////
    // HELP //
    //////////
    string help;

    bool bShowHelp = true;

    void createHelp();

private:
    const static ofMatrix4x4 nuitrackViewportToRealSenseViewportTransform;
    static ofMatrix4x4 makeNuitrackToRealSenseTransform();
};