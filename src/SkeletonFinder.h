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

struct Joint {
    nuitrack::JointType type;
    float confidence;
    glm::vec3 pos;

    Joint(nuitrack::JointType type, float confidence, glm::vec3 pos) :
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
    void initGUI(ofxGui& gui);
    void setTransformMatrix(ofMatrix4x4* mat);
    void update(nuitrack::SkeletonData::Ptr data);

    void updateSensorBox(int & value);
    
    void drawSensorBox();
    void drawSkeletons2d(ofRectangle _rect);
    void drawSkeletons();

    string getShortDesc();

    vector<Skeleton> getSkeletons();
    
private:

    ofxnui::TrackerRef tracker;
    vector<Skeleton> skeletons;

    ofMatrix4x4* transformMatrix;

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


