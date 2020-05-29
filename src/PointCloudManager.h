#pragma once

#include "ofMain.h"
#include <nuitrack/Nuitrack.h>
#include "ofxNuitrack.h"

using namespace tdv::nuitrack;

class PointCloudManager
{
private:
	DepthSensor::Ptr depthSensor;

	ofTexture rgbTex;
	glm::vec2 rgbFrameSize;

	ofTexture depthTex;
	glm::vec2 depthFrameSize;

	ofVboMesh pointCloud;

	const int skip = 2;

	void createPointCloudIfNotExist(glm::vec2 dim);

public:
	void drawPointCloud();
	void drawRGB(const ofRectangle& viewRect);
	void drawDepth(const ofRectangle& viewRect);

	void updateRGB(RGBFrame::Ptr data);
	void updateDepth(DepthFrame::Ptr data);

	void setDepthSensor(DepthSensor::Ptr sensor);
};

