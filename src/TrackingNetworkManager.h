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
#include "SkeletonFinder.h"
#include "Frustum.h"

#include <cmath>
#include <regex>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

#define BROADCAST_NOCLIENT_FREQ     1000
#define BROADCAST_CLIENT_FREQ       10000

#define NETWORK_BROADCAST_PORT 47500
#define NETWORK_LISTENING_PORT 47600

class TrackingNetworkManager {
    
public:    
    void setup(ofxGui &gui, string _realsenseSerial);
    void update(const SkeletonFinder& skeletonFinder);

    void sendTrackingData(const SkeletonFinder& skeletonFinder);
    void sendSkeletonData(const Skeleton& skeleton);

    void sendMessageToTrackingClients(ofxOscMessage _msg);
    void checkTrackingClients(long _currentMillis);
    int getTrackingClientIndex(string _ip, int _port);

    void sendBroadCastAddress();
    
    string getOscMsgAsString(ofxOscMessage m);
 
	vector<string> matchesInRegex(string _str, string _reg);

    vector<string>  localIpAddresses;
    
    string          mDeviceSerial;
    
    float           scale;
    
    long             frameNumber;
    
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
    
    ofxGuiGroup *colorToggles;
    
    ofxGuiGroup *broadcastGroup;
    
    ofParameter<string> broadcastIP;
    ofParameter<int> broadcastPort;

    ofxGuiGroup *listeningGroup;
    
    ofParameter<string> listeningIP;
    ofParameter<int> listeningPort;

    ofParameter<int> mServerID;

    ofParameterGroup streamingGuiGroup;
    
    ofParameter<bool> streamingWholeBody;
};


