//
//  TrackingNetworkManager.h
//  kinectTCPServer
//
//  Created by maybites on 14.02.14.
//
//
#pragma once

#include "ofMain.h"
#include "ofVec3f.h"
#include "ofConstants.h"
#include "ofxOsc.h"
#include "TrackingClient.h"

#include "DetectionMethod.h"
#ifdef BLOB
    #include "BlobFinder.h"
    #include "BlobTracker.h"
#else
    #include "SkeletonFinder.h"
#endif

#include <cmath>
#include <regex>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

#define BROADCAST_NOCLIENT_FREQ     1000
#define BROADCAST_CLIENT_FREQ       10000

#define NETWORK_LISTENING_PORT 47600

#ifdef BLOB
    typedef BlobFinder BodyFinder;
#else
    typedef SkeletonFinder BodyFinder;
#endif

class TrackingNetworkManager {
    
public:    
    void setup(ofxGui &gui);

    void update(const BodyFinder& bodyFinder);
    void sendTrackingData(const BodyFinder& bodyFinder);
    void sendMultipleBodiesAlert();
    void sendNoBodyFound();
    void sendBody(string address, glm::vec3 position, float confidence);

#ifdef BLOB
    void sendBlobData(const BlobTracker& blob);
#else
    void sendSkeletonData(const glm::vec3&);
#endif

    void sendMessageToTrackingClients(ofxOscMessage _msg);
    void checkTrackingClients(long _currentMillis);
    int getTrackingClientIndex(string _ip, int _port);
    
    string getOscMsgAsString(ofxOscMessage m);
 
	vector<string> matchesInRegex(string _str, string _reg);

    vector<string>  localIpAddresses;
    
    //----------------------------------------
    // Server side:
    ofxOscReceiver  serverReceiver;         // OSC receiver
    
    // Message display variables
    vector<string>  serverMessages;         //vector containing the received messages for display
    unsigned int    maxServerMessages;      //nr of messages fitting on the screen
    
    vector<TrackingClient> knownClients;    //collected IP's of chat participants

    ofxOscSender    broadcastSender;        // broadcastSender object

    long            broadCastTimer;
    
    // GUI
    ofxGuiPanel *panel;
    ofParameter<int> mServerID;
};


