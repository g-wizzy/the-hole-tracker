#pragma once

#include <ofxGuiExtended.h>

#include <ofxRealSenseTwo.h>

#include <cubemos/engine.h>
#include <cubemos/skeleton_tracking.h>

using CUBEMOS_SKEL_Buffer_Ptr = std::unique_ptr<CM_SKEL_Buffer, void (*)(CM_SKEL_Buffer*)>;

#define KEYPOINTS_NUMBER 18
#define HEAD_INDEX 0

struct Keypoint
{
    glm::vec3 position;
    float confidence;
};

struct CM_Skeleton
{
    Keypoint keypoints[KEYPOINTS_NUMBER];
};

class CubemosTracker
{
public:
    CubemosTracker();
    ~CubemosTracker();

    void onDepthData(const ofPixels& depthFrame);
    void onVideoData(
        const ofPixels& videoFrame,
        ofxRealSenseTwo::RSDevicePtr& RS_device,
        const ofMatrix4x4& deviceToWorldMatrix
    );

    void initGUI(ofxGui& gui);

    void drawSensorBox();
    void drawSkeletons();

    bool getSkeleton(const CM_Skeleton** skeleton) const;

private:
    CM_SKEL_Handle* handle = nullptr;
    
    CUBEMOS_SKEL_Buffer_Ptr skeletonsPresent;
    CUBEMOS_SKEL_Buffer_Ptr skeletonsPast;

    CM_Skeleton trackedSkeleton;

    const ofPixels* currentDepthFrame;

    void detect_2d(const ofPixels& videoFrame);
    void detect_3d(ofxRealSenseTwo::RSDevicePtr& RS_device);
    void transform_to_world(const ofMatrix4x4& deviceToWorldMAtrix);

    const glm::vec2 absentKeyPoint = glm::vec2(-1.0f, -1.0f);
    
    CUBEMOS_SKEL_Buffer_Ptr create_skel_buffer()
    {
        return CUBEMOS_SKEL_Buffer_Ptr(new CM_SKEL_Buffer(), [](CM_SKEL_Buffer* pb) {
            cm_skel_release_buffer(pb);
            delete pb;
        });
    }

public:
    ofxGuiPanel *panel;
    ofxGuiGroup *sensorBoxGuiGroup;

    ofParameter<bool> filtering;
    ofParameter<int> sensorBoxLeft;
    ofParameter<int> sensorBoxRight;
    ofParameter<int> sensorBoxTop;
    ofParameter<int> sensorBoxBottom;
    ofParameter<int> sensorBoxFront;
    ofParameter<int> sensorBoxBack;

    ofVboMesh sensorBox;
};
