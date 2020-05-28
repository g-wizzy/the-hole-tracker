//
//  BlobTracker.h
//  kinectTCPServer
//
//  Created by maybites on 14.02.14.
//
//
#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofVec3f.h"
#include "ofxGuiExtended.h"
#include "ofConstants.h"
#include "Planef.h"
#include "Linef.h"
#include "OrthoCamera.h"

#include "ofxNuitrack.h"
#include "nuitrack/Nuitrack.h"

#include <cmath>

#define N_MAX_BLOBS 30
#define SCALE 0.001

using namespace tdv;

// TODO: This is the file which will change the most
/* included in ofApp and TrackingNetworkManager
methods and fields called in ofApp :
    setup(gui)
    panel->set{Width,Position}
    {
    //update
    skeletonFinder.captureMaskBegin();
    drawCapturePointCloud(true);
    skeletonFinder.captureMaskEnd();
    }
    update
    {
    //draw
    skeletonFinder.grayImage.draw(viewGrid[2]);
    skeletonFinder.contourFinder.draw(viewGrid[3]);
    skeletonFinder.maskFbo.draw(viewGrid[4]);
    }
    drawSkel2d
    drawSkel
    sensorBoxXXX
    clearMask
    saveMask
    loadMask
methods and fields called in TrackingetworkManager :
    sensorBoxXXX
    blobEvents // should be bypassed via nuitrack itself
*/

struct Joint {
    nuitrack::JointType type;
    float confidence;
    ofVec3f pos;

    Joint(nuitrack::JointType type, float confidence, ofVec3f pos) :
        type(type), confidence(confidence), pos(pos) {}
};

struct Skeleton {
    int id;
    vector<Joint> joints;

    Skeleton(int id, vector<Joint> joints) :
        id(id), joints(joints) {}
};

struct Bone {
    Bone(nuitrack::JointType from, nuitrack::JointType to, glm::vec3 _direction) :
        from(from),
        to(to),
        direction(direction) {}

    nuitrack::JointType from;
    nuitrack::JointType to;
    glm::vec3 direction;
};


class SkeletonFinder {
    
public:
    SkeletonFinder() {}

    void setup(ofMatrix4x4* transformMatrix, ofxGui &gui);
    void run();

    void updateSensorBox(int & value);
    
    void drawSensorBox();
    void drawSkeletons2d(ofRectangle _rect);
    void drawSkeletons();

    //vector<Skeleton> getSkeletons();
    
private:

    void initNuitrack();
    void initGUI(ofxGui& gui);

    ofxnui::TrackerRef tracker;
    vector<Skeleton> skeletons;

    ofMatrix4x4* transformMatrix;
    void update(nuitrack::SkeletonData::Ptr data);

public:
    ofxGuiPanel *panel;
	ofxGuiGroup *sensorBoxGuiGroup;

    ofParameter<int> sensorBoxLeft;
    ofParameter<int> sensorBoxRight;
    ofParameter<int> sensorBoxTop;
    ofParameter<int> sensorBoxBottom;
    ofParameter<int> sensorBoxFront;
    ofParameter<int> sensorBoxBack;

    ofVboMesh sensorBox;
   
    // TODO: What are those
    ofVec3f normal;
    float p;
};


