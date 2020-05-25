//
//  SkeletonFinder.cpp
//
//  Created by Pierre Bürki on 19.05.20.
//  Adapted from BlobFinder.cpp by maybites (14.02.14).
//

#include "SkeletonFinder.h"

void SkeletonFinder::initNuitrack() {
	tracker = ofxnui::Tracker::create();
	tracker->init();

	// depth feed settings
	tracker->setConfigValue("Realsense2Module.Depth.FPS", "30");
	tracker->setConfigValue("Realsense2Module.Depth.RawWidth", "1280");
	tracker->setConfigValue("Realsense2Module.Depth.RawHeight", "720");
	tracker->setConfigValue("Realsense2Module.Depth.ProcessWidth", "1280");
	tracker->setConfigValue("Realsense2Module.Depth.ProcessHeight", "720");
	tracker->setConfigValue("Realsense2Module.Depth.ProcessMaxDepth", "7000");

	// rgb feed settings
	tracker->setConfigValue("Realsense2Module.RGB.FPS", "30");
	tracker->setConfigValue("Realsense2Module.RGB.RawWidth", "1280");
	tracker->setConfigValue("Realsense2Module.RGB.RawHeight", "720");
	tracker->setConfigValue("Realsense2Module.RGB.ProcessWidth", "1280");
	tracker->setConfigValue("Realsense2Module.RGB.ProcessHeight", "720");

	// post processing settings
	tracker->setConfigValue("Realsense2Module.Depth.PostProcessing.SpatialFilter.spatial_alpha", "0.1");
	tracker->setConfigValue("Realsense2Module.Depth.PostProcessing.SpatialFilter.spatial_delta", "50");

	// distance settings
	tracker->setConfigValue("Segmantation.MAX_DISTANCE", "7000");
	tracker->setConfigValue("Skeletonization.MaxDistance", "7000");

	// skeleton detection callback
	tracker->setSkeletonCallback([this](nuitrack::SkeletonData::Ptr data) {
		update(data);
	});
}

void SkeletonFinder::setup(ofMatrix4x4* transformMatrix, ofxGui& gui){
	this->transformMatrix = transformMatrix;
	initNuitrack();
	initGUI(gui);
}

void SkeletonFinder::update(nuitrack::SkeletonData::Ptr data) {
	skeletons.clear();
	// TODO: filter using the capture bounds
	for (nuitrack::Skeleton skel : data->getSkeletons()) {
		vector<Joint> joints;
		for (nuitrack::Joint joint : skel.joints) {
			ofVec3f pos = ofxnui::Tracker::fromVector3(joint.real);
			pos = *transformMatrix * pos;

			joints.push_back(Joint(joint.type, joint.confidence, pos));
		}

		skeletons.push_back(Skeleton(skel.id, joints));
	}
}


// TODO: remove once it is safe to do so
// There could be something of interest here
void /*SkeletonFinder::*/update(){
	return;
	//ofColor white = ofColor::white;
	//ofColor black = ofColor::black;

	///****************************************************************
 //             	      PREPARE NEW FRAME
	//*****************************************************************/

	//int minID = 0;

	////tells all the blobs that the next frame is comming
	//for (int e = 0; e < skeletonEvents.size(); e++) {
	//	skeletonEvents[e].updatePrepare();
	//	minID = (skeletonEvents[e].mID >= minID) ? skeletonEvents[e].mID + 1 : minID;
	//}

	//if (useMask.get()) {
	//	fbo.begin();
	//	// Cleaning everthing with alpha mask on 0 in order to make it transparent for default
	//	ofClear(0, 0, 0, 0);

	//	maskShader.begin();
	//	maskShader.setUniformTexture("maskTex", maskImg.getTexture(), 1);

	//	captureFBO.draw(0, 0);

	//	maskShader.end();
	//	fbo.end();

	//	fbo.readToPixels(fbopixels);
	//}
	//else {
	//	captureFBO.readToPixels(fbopixels);
	//}

 //   colorImg.setFromPixels(fbopixels);

 //   // load grayscale captured depth image from the color source
 //   grayImage.setFromColorImage(colorImg);
 //   
 //   //grayImage.blurHeavily();
 //   
	//ofPixelsRef blobRefPixls = skeletonRef.getPixels();
 //   
 //   ofPixelsRef greyref = grayImage.getPixels();
	//ofPixelsRef eyeRef = grayEyeLevel.getPixels();
	//
	//eyeRef.setColor(black);

 //   float sensorFieldFront = sensorBoxFront.get() * SCALE;
 //   float sensorFieldBack = sensorBoxBack.get() * SCALE;
 //   float sensorFieldLeft = sensorBoxLeft.get() * SCALE;
 //   float sensorFieldRight = sensorBoxRight.get() * SCALE;
 //   float sensorFieldTop = sensorBoxTop .get() * SCALE;
 //   float sensorFieldBottom = sensorBoxBottom.get() * SCALE;
 //   float sensorFieldWidth = sensorFieldRight - sensorFieldLeft;
 //   float sensorFieldHeigth = sensorFieldTop - sensorFieldBottom;
 //   float sensorFieldDepth = sensorFieldBack - sensorFieldFront;
 //   
 //   int eyeLevelColor = eyeLevel.get() / sensorFieldHeigth * 255;
 //   
 //   int headTopThreshold = eyeLevelColor / 4;
 //   
 //   //ofLog(OF_LOG_NOTICE, "eyeLevelColor = " + ofToString(eyeLevelColor));
 //   
 //   //ofLog(OF_LOG_NOTICE, "eyref size : " + ofToString(eyeRef.size()));
 // 
	///****************************************************************
	//				  FIND BODY CONTOURS
	//*****************************************************************/

	//int minBlobSize = pow(blobAreaMinStp1.get() * sensorFboSize.get(), 2);
	//int maxBlobSize = pow(blobAreaMax.get() * sensorFboSize.get(), 2);


	//detectedHeads.clear();

	//contourFinder.findContours(grayImage, minBlobSize, maxBlobSize, countBlob.get(), false);

 //   for (int i = 0; i < contourFinder.nBlobs; i++){
 //       ofRectangle bounds = contourFinder.blobs[i].boundingRect;
 //       float pixelBrightness = 0;
 //       float brightness = 0;
 //       
 //       // find the brightest pixel within the blob. this defines the height of the blob
 //       for(int x = bounds.x; x < bounds.x + bounds.width; x++){
 //           for(int y = bounds.y; y < bounds.y + bounds.height; y++){
 //               pixelBrightness = greyref.getColor(x, y).getBrightness();
 //               brightness = (pixelBrightness > brightness)?pixelBrightness: brightness;
 //           }
 //       }

	//	/*
	//	float averageBrightness = 0;
 //       int averageCounter = 0;

 //       // go through the pixels again and get the average brightness for the headTopThreshold
 //       for(int x = bounds.x; x < bounds.x + bounds.width; x++){
 //           for(int y = bounds.y; y < bounds.y + bounds.height; y++){
 //               pixelBrightness = greyref.getColor(x, y).getBrightness();
 //               if(pixelBrightness > brightness - headTopThreshold){
 //                   averageBrightness += pixelBrightness;
 //                   averageCounter++;
 //               }
 //           }
 //       }
 //       
 //       brightness = averageBrightness / averageCounter;
 //       */
 //       
 //       // find all the pixels down to the eyelevel threshold. this yealds an image with blobs that mark the size of the head at eyelevel.
 //       ofVec2f headtop2d = ofVec2f();
 //       int brighCounter = 0;
 //       for(int x = bounds.x; x < bounds.x + bounds.width; x++){
 //           for(int y = bounds.y; y < bounds.y + bounds.height; y++){
 //               pixelBrightness = greyref.getColor(x, y).getBrightness();
 //               if(pixelBrightness > (brightness - eyeLevelColor)){
 //                   //writes the pixels above the eyelevel into the eyeRef image
 //                   eyeRef.setColor(x, y, brightness);
 //               }else{
 //                   eyeRef.setColor(x, y, black);
 //               }
 //               if(pixelBrightness >= brightness - (eyeLevelColor / 4)){
 //                   headtop2d += ofVec2f(x, y);
 //                   brighCounter++;
 //               }
 //           }
 //       }
 //       headtop2d /= brighCounter;
 //       
 //       //ofLog(OF_LOG_NOTICE, "headtop2d = " + ofToString(headtop2d));
 //       		
	//	ofVec3f headTop = ofVec3f((headtop2d.x / captureScreenSize.x) * sensorFieldWidth + sensorFieldLeft, sensorFieldBack - (headtop2d.y / captureScreenSize.y ) * sensorFieldDepth, (brightness / 255.0) * sensorFieldHeigth + sensorFieldBottom);

 //       //ofVec3f headCenter = ofVec3f(headTop.x, headTop.y, headTop.z - eyeLevel.get());

	//	DetectedSkeleton newBodyBlob;
	//	newBodyBlob.bound = bounds;
	//	newBodyBlob.headTop = headTop;	
	//	newBodyBlob.hasBeenTaken = false;

	//	detectedHeads.push_back(newBodyBlob);

	//}

	///****************************************************************
	//	  UPDATE BODY EVENTS
	//*****************************************************************/

	//int matchBlobID = -1;
	//int matchEventID = -1;
	//float minDistance = 1000000;

	//do {
	//	matchBlobID = -1;
	//	matchEventID = -1;
	//	minDistance = 1000000;
	//	for (int i = 0; i < skeletonEvents.size(); i++) {											// iterate through all the current events
	//		if (skeletonEvents[i].isAlive() && !skeletonEvents[i].hasBeenUpdated()) {					//   all those that haven't been updated yet 
	//			for (int j = 0; j < detectedHeads.size(); j++) {								// now we go through all the new blobs
	//				if (!detectedHeads[j].hasBeenTaken) {										//   but only if the blobs havent been taken before
	//					float dist = skeletonEvents[i].getDistance(detectedHeads[j].headTop);
	//					if (dist < eventMaxSize.get()*SCALE) {										//   and test if the distance is within the threshold
	//						if (minDistance > dist) {
	//							minDistance = dist;
	//							matchBlobID = j;
	//							matchEventID = i;
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//	if (matchEventID >= 0) {
	//		ofRectangle bounds = detectedHeads[matchBlobID].bound;
	//		ofVec3f headTop = detectedHeads[matchBlobID].headTop;

	//		//calculate the blob pos in worldspace
	//		ofVec3f blobPos = ofVec3f(((float)bounds.getCenter().x / captureScreenSize.x) * sensorFieldWidth + sensorFieldLeft, sensorFieldBack - ((float)bounds.getCenter().y / captureScreenSize.y) * sensorFieldDepth, headTop.z);

	//		//calculate the blob size in worldspace
	//		ofVec2f blobSize = ofVec2f(((float)bounds.getWidth() / captureScreenSize.x) * sensorFieldWidth, ((float)bounds.getHeight() / captureScreenSize.y) * sensorFieldDepth);

	//		//ofLogVerbose("Updating old event with ID: " + ofToString(blobEvents[matchEventID].mID));

	//		skeletonEvents[matchEventID].update(bounds, blobPos, blobSize, headTop, smoothFactor.get());
	//		detectedHeads[matchBlobID].hasBeenTaken = true;
	//	}
	//} while (matchEventID >= 0);

	///****************************************************************
	//	  CREATE NEW BODY EVENTS
	//*****************************************************************/
	//for (int j = 0; j < detectedHeads.size(); j++) {
	//	if (!detectedHeads[j].hasBeenTaken) {			
	//		ofRectangle bounds = detectedHeads[j].bound;
	//		ofVec3f headTop = detectedHeads[j].headTop;

	//		//calculate the blob pos in worldspace
	//		ofVec3f blobPos = ofVec3f(((float)bounds.getCenter().x / captureScreenSize.x) * sensorFieldWidth + sensorFieldLeft, sensorFieldBack - ((float)bounds.getCenter().y / captureScreenSize.y) * sensorFieldDepth, headTop.z);

	//		//calculate the blob size in worldspace
	//		ofVec2f blobSize = ofVec2f(((float)bounds.getWidth() / captureScreenSize.x) * sensorFieldWidth, ((float)bounds.getHeight() / captureScreenSize.y) * sensorFieldDepth);

	//		//ofLogVerbose("Creating new event with ID: " + ofToString(minID));

	//		skeletonEvents.push_back(SkeletonTracker(minID++, eventBreathSize.get(), bounds, blobPos, blobSize, headTop));
	//	}
	//}


	///****************************************************************
	//			  FIND HEAD CONTOURS
	//*****************************************************************/
	///**

 //   //preprocesses the eyeRef image
 //   //grayEyeLevel.setFromPixels(eyeRef.getPixels(), eyeRef.getWidth(), eyeRef.getHeight());
 //   //grayEyeLevel.invert();
 //   //grayEyeLevel.threshold(20);
 //   //grayEyeLevel.invert();
 //   grayEyeLevel.blurGaussian();

 //   //ofLog(OF_LOG_NOTICE, "contourEyeFinder nBlobs : " + ofToString(contourEyeFinder.nBlobs));

	//int minBlobSize2 = pow(blobAreaMinStp2.get() * sensorFboSize.get(), 2);

 //   //find head shape on eye height contours
 //   contourEyeFinder.findContours(grayEyeLevel, minBlobSize2, maxBlobSize, countBlob.get(), false);
 //   for(int i = 0; i < contourEyeFinder.nBlobs; i++){

	//	ofRectangle bounds = contourEyeFinder.blobs[i].boundingRect;

	//	int brightness = eyeRef.getColor((float)bounds.getCenter().x, (float)bounds.getCenter().y).getBrightness();
	//	float height = (brightness / 255.0) * sensorFieldHeigth + sensorFieldBottom;

	//	//calculate the blob pos in worldspace
	//	ofVec3f headBlobCenter = ofVec3f(((float)bounds.getCenter().x / captureScreenSize.x) * sensorFieldWidth + sensorFieldLeft, sensorFieldBack - ((float)bounds.getCenter().y / captureScreenSize.y) * sensorFieldDepth, height);

	//	//calculate the blob size in worldspace
	//	ofVec2f headBlobSize = ofVec2f(((float)bounds.getWidth() / captureScreenSize.x) * sensorFieldWidth, ((float)bounds.getHeight() / captureScreenSize.y) * sensorFieldDepth);

	//	//calculate the gazeVector
	//	//ofVec3f gaze = blobEvents[bid].getCurrentHeadCenter() - gazePoint.get();

	//	//gaze.z = 0;

	//	float smalestAngle = 180;
	//	ofVec3f eyePoint = headBlobCenter;

	//	//clears the contour storage
	//	blobEvents[bid].countour.clear();

	//	// findes the closest contour point to the eyegave-vector, takes its distance to the headCenter and calculated
	//	// the eye - center - point
	//	for (int v = 0; v < contourEyeFinder.blobs[i].pts.size(); v++) {
	//		ofVec3f headPoint = ofVec3f(((float)contourEyeFinder.blobs[i].pts[v].x / captureScreenSize.x) * sensorFieldWidth + sensorFieldLeft, sensorFieldBack - ((float)contourEyeFinder.blobs[i].pts[v].y / captureScreenSize.y) * sensorFieldDepth, blobEvents[bid].headCenter.z);

	//		blobEvents[bid].countour.push_back(headPoint);

	//		ofVec3f gaze2 = blobEvents[bid].getCurrentHeadCenter() - headPoint;

	//		float angle = gaze.angle(gaze2);

	//		if (smalestAngle > angle) {
	//			smalestAngle = angle;
	//			eyePoint = blobEvents[bid].getCurrentHeadCenter() - gaze.normalize().scale(gaze2.length() * eyeInset.get());
	//		}
	//	}

	//	bool foundBlob = false;

 //       for(int bid = 0; bid < blobEvents.size(); bid++){
 //           // find the blob
 //           if(!blobEvents[bid].hasBeenUpdated() && blobEvents[bid].isMatching(bounds, eventMaxSize.get())){
 //              
	//			/////////////////////////////////////////////////////////////////
	//					  UPDATE HEAD EVENTS
	//			/////////////////////////////////////////////////////////////////

	//			ofLogVerbose("Updating old event with ID: " + ofToString(blobEvents[bid].mID) + " headHight = " + ofToString(headBlobCenter.z));

	//			blobEvents[bid].update(bounds, headBlobCenter, headBlobSize, eyePoint, smoothFactor.get());
	//			foundBlob = true;
	//			break;
 //           }
 //       }
	//	// if none is found, create a new one.
	//	if (!foundBlob) {
	//		ofLogVerbose("Creating new event with ID: " + ofToString(minID));
	//		blobEvents.push_back(BlobTracker(minID++, bounds, eventBreathSize.get()));
	//		blobEvents.back().update(bounds, headBlobCenter, headBlobSize, eyePoint, smoothFactor.get());
	//	}

 //   }
	//*/

	///****************************************************************
	//	  SORT EVENTS
	//*****************************************************************/

	///*
	////sets the sort value to the current index.
	//int sortPos = 0;

	//for (int i = 0; i < blobEvents.size(); i++) {
	//	blobEvents[i].sortPos = sortPos++;
	//}

	//// if we are using the gaze point
	//if (useGazePoint.get()) {
	//	if (blobEvents.size() > 0) {
	//		for (int i = 0; i < (blobEvents.size() - 1); i++) {
	//			for (int j = 1; j < blobEvents.size(); j++) {
	//				if ((blobEvents[i].headCenter - gazePoint.get()).length() < (blobEvents[j].headCenter - gazePoint.get()).length()) {
	//					if (blobEvents[i].sortPos > blobEvents[j].sortPos) {
	//						int savepos = blobEvents[j].sortPos;
	//						blobEvents[j].sortPos = blobEvents[i].sortPos;
	//						blobEvents[i].sortPos = savepos;
	//					}
	//				}
	//				else {
	//					if (blobEvents[i].sortPos < blobEvents[j].sortPos) {
	//						int savepos = blobEvents[j].sortPos;
	//						blobEvents[j].sortPos = blobEvents[i].sortPos;
	//						blobEvents[i].sortPos = savepos;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	//*/

 //
	///****************************************************************
	//				CLEANUP EVENTS
	//*****************************************************************/

 //   //updates all alive blobs and removes all the blobs that havent had an update for a specific number of frames or have been killed
 //   for(int e = skeletonEvents.size() - 1; e >= 0; e--){
 //       if(skeletonEvents[e].isDead()){
	//		skeletonEvents.erase(skeletonEvents.begin() + e);
 //       }
 //   }
}

void SkeletonFinder::initGUI(ofxGui& gui) {
	panel = gui.addPanel();

	panel->loadTheme("theme/theme_light.json");
	panel->setName("Tracking...");

	// TODO: consider adding nuitrack params

	sensorBoxLeft.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxRight.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxFront.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxBack.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxTop.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxBottom.addListener(this, &SkeletonFinder::updateSensorBox);

	sensorBoxGuiGroup = panel->addGroup("SensorBox");
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxLeft.set("left", 1000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxRight.set("right", -1000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxFront.set("front", 1000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxBack.set("back", -1000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxTop.set("top", 2000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxBottom.set("bottom", 1000));

	panel->loadFromFile("trackings.xml");
}

void SkeletonFinder::drawSensorBox()
{
	sensorBox.draw();
}

void SkeletonFinder::drawSkeletons2d(ofRectangle _rect){
	// TODO ? this seems to draw the skeleton on displays 1-5, which isn't _really_ useful
    /*float xFactor = _rect.width / captureScreenSize.x;
    float yFactor = _rect.height / captureScreenSize.y;
    
    ofNoFill();
    for(int i = 0; i < skeletonEvents.size(); i++){
		if (skeletonEvents[i].hasBeenUpdated() && skeletonEvents[i].isActive()) {
			ofSetColor(255, 0, 0, 255);
			ofDrawRectangle(_rect.x + skeletonEvents[i].baseRectangle2d.x * xFactor, _rect.y + skeletonEvents[i].baseRectangle2d.y * yFactor, skeletonEvents[i].baseRectangle2d.width * xFactor, skeletonEvents[i].baseRectangle2d.height * yFactor);
			ofDrawBitmapString("blob[" + ofToString(skeletonEvents[i].mID) + "]\n alive = " + ofToString(skeletonEvents[i].getAgeInMillis()) + "\n sort = " + ofToString(skeletonEvents[i].sortPos) + "\n x = " + ofToString(skeletonEvents[i].headTop.x) + "\n y = " + ofToString(skeletonEvents[i].headTop.y) + "\n z = " + ofToString(skeletonEvents[i].headTop.z), _rect.x + skeletonEvents[i].baseRectangle2d.getCenter().x * xFactor, _rect.y + skeletonEvents[i].baseRectangle2d.getCenter().y * yFactor);
		}
        
    }*/
}

// adapted from example
void SkeletonFinder::drawSkeletons() {
	static vector<Bone> bones = {
		Bone(nuitrack::JOINT_WAIST, nuitrack::JOINT_TORSO, glm::vec3(0, 1, 0)),
		Bone(nuitrack::JOINT_TORSO, nuitrack::JOINT_NECK, glm::vec3(0, 1, 0)),
		Bone(nuitrack::JOINT_NECK, nuitrack::JOINT_HEAD, glm::vec3(0, 1, 0)),

		Bone(nuitrack::JOINT_LEFT_COLLAR, nuitrack::JOINT_LEFT_SHOULDER, glm::vec3(1, 0, 0)),
		Bone(nuitrack::JOINT_LEFT_SHOULDER, nuitrack::JOINT_LEFT_ELBOW, glm::vec3(1, 0, 0)),
		Bone(nuitrack::JOINT_LEFT_ELBOW, nuitrack::JOINT_LEFT_WRIST, glm::vec3(1, 0, 0)),
		Bone(nuitrack::JOINT_LEFT_WRIST, nuitrack::JOINT_LEFT_HAND, glm::vec3(1, 0, 0)),

		Bone(nuitrack::JOINT_WAIST, nuitrack::JOINT_LEFT_HIP, glm::vec3(1, 0, 0)),
		Bone(nuitrack::JOINT_LEFT_HIP, nuitrack::JOINT_LEFT_KNEE, glm::vec3(0, -1, 0)),
		Bone(nuitrack::JOINT_LEFT_KNEE, nuitrack::JOINT_LEFT_ANKLE, glm::vec3(0, -1, 0)),

		Bone(nuitrack::JOINT_RIGHT_COLLAR, nuitrack::JOINT_RIGHT_SHOULDER, glm::vec3(-1, 0, 0)),
		Bone(nuitrack::JOINT_RIGHT_SHOULDER, nuitrack::JOINT_RIGHT_ELBOW, glm::vec3(-1, 0, 0)),
		Bone(nuitrack::JOINT_RIGHT_ELBOW, nuitrack::JOINT_RIGHT_WRIST, glm::vec3(-1, 0, 0)),
		Bone(nuitrack::JOINT_RIGHT_WRIST, nuitrack::JOINT_RIGHT_HAND, glm::vec3(-1, 0, 0)),

		Bone(nuitrack::JOINT_WAIST, nuitrack::JOINT_RIGHT_HIP, glm::vec3(-1, 0, 0)),
		Bone(nuitrack::JOINT_RIGHT_HIP, nuitrack::JOINT_RIGHT_KNEE, glm::vec3(0, -1, 0)),
		Bone(nuitrack::JOINT_RIGHT_KNEE, nuitrack::JOINT_RIGHT_ANKLE, glm::vec3(0, -1, 0)),
	};

	for (Skeleton skel : skeletons) {
		for (Bone bone : bones) {
			auto j1 = skel.joints[bone.from];
			auto j2 = skel.joints[bone.to];

			if (j1.confidence < 0.15 || j2.confidence < 0.15) {
				continue;
			}

			ofSetColor(ofColor(0, 1, 0));
			ofDrawLine(j1.pos, j2.pos);
		}
	}
}

void SkeletonFinder::updateSensorBox(int & value){
    sensorBox.clear();
    sensorBox.setMode(OF_PRIMITIVE_LINES);
    
    sensorBox.addVertex(ofPoint(sensorBoxLeft.get() * SCALE, sensorBoxFront.get() * SCALE, sensorBoxBottom.get() * SCALE));
    sensorBox.addVertex(ofPoint(sensorBoxRight.get() * SCALE, sensorBoxFront.get() * SCALE, sensorBoxBottom.get() * SCALE));
    
    sensorBox.addVertex(ofPoint(sensorBoxRight.get() * SCALE, sensorBoxFront.get() * SCALE, sensorBoxBottom.get() * SCALE));
    sensorBox.addVertex(ofPoint(sensorBoxRight.get() * SCALE, sensorBoxBack.get() * SCALE, sensorBoxBottom.get() * SCALE));
    
    sensorBox.addVertex(ofPoint(sensorBoxRight.get() * SCALE, sensorBoxBack.get() * SCALE, sensorBoxBottom.get() * SCALE));
    sensorBox.addVertex(ofPoint(sensorBoxLeft.get() * SCALE, sensorBoxBack.get() * SCALE, sensorBoxBottom.get() * SCALE));
    
    sensorBox.addVertex(ofPoint(sensorBoxLeft.get() * SCALE, sensorBoxBack.get() * SCALE, sensorBoxBottom.get() * SCALE));
    sensorBox.addVertex(ofPoint(sensorBoxLeft.get() * SCALE, sensorBoxFront.get() * SCALE, sensorBoxBottom.get() * SCALE));
    
    sensorBox.addVertex(ofPoint(sensorBoxLeft.get() * SCALE, sensorBoxFront.get() * SCALE, sensorBoxTop.get() * SCALE));
    sensorBox.addVertex(ofPoint(sensorBoxRight.get() * SCALE, sensorBoxFront.get() * SCALE, sensorBoxTop.get() * SCALE));
    
    sensorBox.addVertex(ofPoint(sensorBoxRight.get() * SCALE, sensorBoxFront.get() * SCALE, sensorBoxTop.get() * SCALE));
    sensorBox.addVertex(ofPoint(sensorBoxRight.get() * SCALE, sensorBoxBack.get() * SCALE, sensorBoxTop.get() * SCALE));
    
    sensorBox.addVertex(ofPoint(sensorBoxRight.get() * SCALE, sensorBoxBack.get() * SCALE, sensorBoxTop.get() * SCALE));
    sensorBox.addVertex(ofPoint(sensorBoxLeft.get() * SCALE, sensorBoxBack.get() * SCALE, sensorBoxTop.get() * SCALE));
    
    sensorBox.addVertex(ofPoint(sensorBoxLeft.get() * SCALE, sensorBoxBack.get() * SCALE, sensorBoxTop.get() * SCALE));
    sensorBox.addVertex(ofPoint(sensorBoxLeft.get() * SCALE, sensorBoxFront.get() * SCALE, sensorBoxTop.get() * SCALE));
    
    //captureCam.setPosition((sensorBoxLeft.get() * SCALE + sensorBoxRight.get() * SCALE)/2, (sensorBoxBack.get() * SCALE + sensorBoxBack.get() * SCALE)/2, sensorBoxTop.get() * SCALE);
    //captureCam.setPosition(5, 5, 0);
    //captureCam.
}
