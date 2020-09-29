#include "CubemosTracker.h"
#include "CubemosHelper.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>
#include "ofxRealSenseTwo.h"

CubemosTracker::CubemosTracker() :
    skeletonsPast(create_skel_buffer()),
    skeletonsPresent(create_skel_buffer())
{
    // Output all messages with severity level INFO or higher to the console and to a file
    cm_initialise_logging(CM_LogLevel::CM_LL_INFO, true, default_log_dir().c_str());

    CM_ReturnCode retCode = cm_skel_create_handle(&handle, default_license_dir().c_str());

    if (retCode == CM_FILE_DOES_NOT_EXIST)
    {                                                                       \
        std::cout << "Activation key does not exist. Please run the post installation script found"
                    << " in $CUBEMOS_SKEL_SDK / scripts to activate the license and "
                    << " use it in your application." << std::endl;
    }
}

CubemosTracker::~CubemosTracker()
{
    cm_skel_destroy_handle(&handle);
}

void CubemosTracker::onDepthData(const ofPixels& depthFrame)
{
    currentDepthFrame = &depthFrame;
}

void CubemosTracker::onVideoData(
    const ofPixels& videoFrame,
    ofxRealSenseTwo::RSDevicePtr& RS_device,
    const ofMatrix4x4& deviceToWorldMatrix)
{
    detect_2d(videoFrame);
    detect_3d(RS_device);
    transform_to_world(deviceToWorldMatrix);
}

void CubemosTracker::detect_2d(const ofPixels& videoFrame)
{
    cv::Mat capturedFrame = cv::Mat(
        cv::Size(videoFrame.getWidth(), videoFrame.getHeight()),
        CV_8UC3,
        (void*)videoFrame.getData(),
        cv::Mat::AUTO_STEP
    );

    CM_Image image = {
        capturedFrame.data,         CM_UINT8, capturedFrame.cols, capturedFrame.rows, capturedFrame.channels(),
        (int)capturedFrame.step[0], CM_HWC
    };

    CM_ReturnCode retCode = cm_skel_estimate_keypoints(handle, &image, 192, skeletonsPresent.get());

    if (retCode == CM_SUCCESS)
    {
        if (skeletonsPresent->numSkeletons > 0)
        {
            cm_skel_update_tracking_id(handle, skeletonsPast.get(), skeletonsPresent.get());
            skeletonsPast.swap(skeletonsPresent);
            cm_skel_release_buffer(skeletonsPresent.get());
        }
    }
}

void CubemosTracker::detect_3d(ofxRealSenseTwo::RSDevicePtr& RS_device)
{
    // We only care for one skeleton
    CM_SKEL_KeypointsBuffer& skel = skeletonsPast->skeletons[0];
    for (size_t j = 0; j < skel.numKeyPoints; j++)
    {
        float x = skel.keypoints_coord_x[j];
        float y = skel.keypoints_coord_y[j];
        if (x != -1.0f && y != -1.0f)
        {
            glm::vec3 position = RS_device->getSpacePointFromDepthFrameCoord(glm::vec2(x, y));
            
            trackedSkeleton.keypoints[j].position = position;
            trackedSkeleton.keypoints[j].confidence = skel.confidences[j];
        }
        else
        {
            trackedSkeleton.keypoints[j].confidence = 0.0f;
        }
    }
}

void CubemosTracker::transform_to_world(const ofMatrix4x4& deviceToWorldMatrix)
{
    for (size_t i = 0; i < KEYPOINTS_NUMBER; i++)
    {
        glm::vec3& keypoint = trackedSkeleton.keypoints[i].position;
        keypoint = (ofVec3f)keypoint * deviceToWorldMatrix;
    }
}

bool CubemosTracker::getSkeleton(const CM_Skeleton** skel) const
{
    if (trackedSkeleton.keypoints[HEAD_INDEX].confidence > 0)
    {
        *skel = &trackedSkeleton;
        return true;
    }
    return false;
}

void CubemosTracker::initGUI(ofxGui& gui)
{
    panel = gui.addPanel();

	panel->loadTheme("theme/theme_light.json");
	panel->setName("Tracking");

    /*
	sensorBoxLeft.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxRight.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxFront.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxBack.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxTop.addListener(this, &SkeletonFinder::updateSensorBox);
	sensorBoxBottom.addListener(this, &SkeletonFinder::updateSensorBox);
    */

	sensorBoxGuiGroup = panel->addGroup("SensorBox");
	sensorBoxGuiGroup->add(filtering.set("Filtering", false));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxLeft.set("left", 1000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxRight.set("right", -1000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxFront.set("front", 1000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxBack.set("back", -1000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxTop.set("top", 2000));
	sensorBoxGuiGroup->add<ofxGuiIntInputField>(sensorBoxBottom.set("bottom", 1000));

	panel->loadFromFile("tracking.xml");
	bool visible = false;
	panel->setVisible(visible);
}

void CubemosTracker::drawSensorBox()
{
    sensorBox.draw();
}

void CubemosTracker::drawSkeletons()
{
    for (size_t i = 0; i < KEYPOINTS_NUMBER; i++)
    {
        const Keypoint& keypoint = trackedSkeleton.keypoints[i];
        if (keypoint.confidence > 0)
        {
            ofDrawCircle(keypoint.position, 4);

            if (i == HEAD_INDEX)
            {
                ofDrawBitmapString("SALUT", keypoint.position + glm::vec3(0, 0, 0.3f));
            }
        }
    }

    const std::vector<std::pair<int, int>> limbKeypointsIds = { { 1, 2 },   { 1, 5 },   { 2, 3 }, { 3, 4 },  { 5, 6 },
                                                                { 6, 7 },   { 1, 8 },   { 8, 9 }, { 9, 10 }, { 1, 11 },
                                                                { 11, 12 }, { 12, 13 }, { 1, 0 }, { 0, 14 }, { 14, 16 },
                                                                { 0, 15 },  { 15, 17 } };

    for (const auto& limbKeypointsId : limbKeypointsIds)
    {
        const Keypoint& keypointFirst = trackedSkeleton.keypoints[limbKeypointsId.first];
        const Keypoint& keypointSecond = trackedSkeleton.keypoints[limbKeypointsId.second];
        
        if (keypointFirst.confidence > 0 && keypointSecond.confidence > 0)
        {
            ofDrawLine(keypointFirst.position, keypointSecond.position);
        }
    }
}