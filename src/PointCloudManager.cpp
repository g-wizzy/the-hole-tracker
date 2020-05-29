#include "PointCloudManager.h"

void PointCloudManager::drawPointCloud()
{
	//ofLog(OF_LOG_NOTICE) << "Drawing cloud point (" << pointCloud.getVertices().size() << ") points)";
	ofPushMatrix();
	ofNoFill();
	pointCloud.draw();
	auto vs = pointCloud.getVertices();
	if (vs.size() > 0) {
		auto v = vs[12000];
		ofLog(OF_LOG_NOTICE) << "Color at vertex 12000 (of " << vs.size() << "): \n"
			<< "R " << v.r << "\n"
			<< "G " << v.g << "\n"
			<< "B " << v.b;
	}
	ofPopMatrix();
}

void PointCloudManager::drawRGB(const ofRectangle& viewRect) {
	rgbTex.draw(viewRect);
}

void PointCloudManager::drawDepth(const ofRectangle& viewRect) {
	depthTex.draw(viewRect);
}

void PointCloudManager::updateRGB(RGBFrame::Ptr data)
{
	rgbFrameSize.x = data->getCols();
	rgbFrameSize.y = data->getRows();

	const Color3* rgbData = data->getData();
	uint8_t* colorDataPtr = (uint8_t*)rgbData;
	ofPixels rgbPix;
	// Assuming the data comes from the live feed (hence BGR)
	rgbPix.setFromPixels(colorDataPtr, rgbFrameSize.x, rgbFrameSize.y, OF_PIXELS_RGB);
	rgbTex.loadData(rgbPix);

	createPointCloudIfNotExist(depthFrameSize);

	for (int y = 0; y < depthFrameSize.y; y += skip) {
		for (int x = 0; x < depthFrameSize.x; x += skip) {
			const int index = y * depthFrameSize.x + x;
			const int skippedIndex = (y / skip) * (depthFrameSize.x / skip) + (x / skip);

			pointCloud.setColor(skippedIndex, rgbPix.getColor(index));
		}
	}
}

void PointCloudManager::updateDepth(DepthFrame::Ptr data)
{
	depthFrameSize.x = data->getCols();
	depthFrameSize.y = data->getRows();

	const unsigned short* depthData = data->getData();
	ofShortPixels pix;
	pix.setFromPixels(depthData, depthFrameSize.x, depthFrameSize.y, OF_PIXELS_GRAY);
	depthTex.loadData(pix);

	createPointCloudIfNotExist(depthFrameSize);
	
	for (int y = 0; y < depthFrameSize.y; y += skip) {
		for (int x = 0; x < depthFrameSize.x; x += skip) {
			const int index = y * depthFrameSize.x + x;
			const int skippedIndex = (y / skip) * (depthFrameSize.x / skip) + (x / skip);

			const unsigned short d = depthData[index];
			Vector3 v = depthSensor->convertProjToRealCoords(x, y, depthData[index]);
			pointCloud.setVertex(skippedIndex, ofxnui::Tracker::fromVector3(v) * 0.001);
		}
	}
}

void PointCloudManager::createPointCloudIfNotExist(glm::vec2 dim)
{
	if (pointCloud.getVertices().size() == 0) {
		ofLog(OF_LOG_NOTICE) << "YOU ONLY SEE THIS ONCE";

		pointCloud.setMode(OF_PRIMITIVE_POINTS);
		// Allocate
		pointCloud.clear();
		int size = ceil(((float)dim.x / skip) * ((float)dim.y / skip));

		vector<glm::vec3> p;
		p.assign(size, glm::vec3(0, 0, 0));
		pointCloud.addVertices(p);

		vector<ofFloatColor> c;
		c.assign(size, ofFloatColor(0, 0, 0, 0.9));
		pointCloud.addColors(c);
	}
}

void PointCloudManager::setDepthSensor(DepthSensor::Ptr sensor) {
	depthSensor = sensor;
}
