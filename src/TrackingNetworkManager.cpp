//
//  TrackingNetworkManager.cpp
//  kinectServer
//
//  Created by maybites on 14.02.14.
//
//

#include "TrackingNetworkManager.h"


void TrackingNetworkManager::setup(ofxGui &gui){    
    string localAddress = "127.0.0.1";
    
    for(int i = 0; i < localIpAddresses.size(); i++){
		ofLog(OF_LOG_NOTICE) << "try to find local ip addresses.. not sure if this function works properly...";
		if(matchesInRegex(localIpAddresses[i], "\\b^(?:(?!127).)+\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\b").size() == 1) {
            localAddress = localIpAddresses[i];
			ofLog(OF_LOG_NOTICE) << "found valid address" << localAddress;
        }
    }

    panel = gui.addPanel();
    
    panel->loadTheme("theme/theme_light.json");
    panel->setName("Broadcasting");
	panel->add<ofxGuiIntInputField>(mServerID.set("ServerID", 0, 0, 10));
    
    broadcastGroup = panel->addGroup("Broadcast TX");
    broadcastGroup->add<ofxGuiTextField>(broadcastIP.set("TX IP","127.0.0.1"));
    broadcastGroup->add<ofxGuiIntInputField>(broadcastPort.set("TX Port", NETWORK_BROADCAST_PORT, NETWORK_BROADCAST_PORT, NETWORK_BROADCAST_PORT + 99));
    
    listeningGroup = panel->addGroup("Listening RX");
    listeningGroup->add<ofxGuiTextField>(listeningIP.set("RX IP",localAddress));
    listeningGroup->add<ofxGuiIntInputField>(listeningPort.set("RX Port", NETWORK_LISTENING_PORT, NETWORK_LISTENING_PORT, NETWORK_LISTENING_PORT + 99));

    panel->loadFromFile("broadcast.xml");
}


//--------------------------------------------------------------
void TrackingNetworkManager::update(const BodyFinder& bodyFinder){
    long currentMillis = ofGetElapsedTimeMillis();

    //send trackingdata to all connected clients
    sendTrackingData(bodyFinder);
    
    // OSC receiver queues up new messages, so you need to iterate
	// through waiting messages to get each incoming message
	// check for waiting messages
	while (serverReceiver.hasWaitingMessages()) {
		// get the next message
		ofxOscMessage m;
		serverReceiver.getNextMessage(m);
		//Log received message for easier debugging of participants' messages:
		ofLog(OF_LOG_NOTICE) << "Server recvd msg " << getOscMsgAsString(m)
			<< "\n\tfrom " << m.getRemoteIp();

		// check the address of the incoming message
		if (m.getAddress() == "/ks/request/handshake") {
			//Identify host of incoming msg
			string incomingHost = m.getRemoteIp();
			//See if incoming host is a new one:
			// get the first argument (listeningport) as an int
			if (m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_INT32) {
				knownClients[getTrackingClientIndex(incomingHost, m.getArgAsInt32(0))].update(currentMillis);
			}
			else {
				ofLog(OF_LOG_WARNING) << "Server recvd malformed message."
					<< "\n\tExpected: /ks/request/handshake <ClientListeningPort>"
					<< "\n\tReceived: " << getOscMsgAsString(m)
					<< "\n\tfrom " << incomingHost;
			}
		}
		else if (m.getAddress() == "/ks/request/update") {
			//Identify host of incoming msg
			string incomingHost = m.getRemoteIp();
			//See if incoming host is a new one:
			// get the first argument (listeningport) as an int
			if (m.getNumArgs() == 1 && m.getArgType(0) == OFXOSC_TYPE_INT32) {
				knownClients[getTrackingClientIndex(incomingHost, m.getArgAsInt32(0))].update(currentMillis);
			}
			else {
				ofLog(OF_LOG_WARNING) << "Server recvd malformed message."
					<< "\n\tExpected: /ks/request/update <ClientListeningPort>"
					<< "\n\tReceived: " << getOscMsgAsString(m)
					<< "\n\tfrom " + incomingHost;
			}
		}
		// handle getting random OSC messages here
		else {
			ofLogWarning("Server got weird message: " + m.getAddress());
		}
	}
    
	//this is purely workaround for a mysterious OSCpack bug on 64bit linux
	// after startup, reinit the receivers
	// must be a timing problem, though - in debug, stepping through, it works.
	if(ofGetFrameNum() == 60){
		serverReceiver.setup(listeningPort.get());
	}
}

void TrackingNetworkManager::sendTrackingData(const BodyFinder& bodyFinder) {

#ifdef BLOB
    vector<BlobTracker> blobs = bodyFinder.blobEvents;
	int i = 0;
	bool done = false;
	while (i < blobs.size() && !done) {
		if (blobs[i].isActive() && blobs[i].hasBeenUpdated()) {
			sendBlobData(blobs[i]);
			done = true;
		}

		++i;
	}
#else
	vector<Skeleton> skeletons = bodyFinder.getSkeletons();
	if (skeletons.size() > 0) {
		// Only one skeleton is to be on the scene for the perspective to work
		if (skeletons.size() > 1) {
			sendMultipleBodiesAlert();
		}
		sendSkeletonData(skeletons[0]);
	}
#endif
}

void TrackingNetworkManager::sendMultipleBodiesAlert() {
	ofxOscMessage alertMsg;
	alertMsg.setAddress("/ks/server/track/multiple-bodies");
	sendMessageToTrackingClients(alertMsg);
}

#ifdef BLOB
void TrackingNetworkManager::sendBlobData(const BlobTracker& blob) {
	ofxOscMessage blobMsg;
	blobMsg.setAddress("/ks/server/track/headblob");

	blobMsg.addIntArg(mServerID.get());

	blobMsg.addFloatArg(blob.headBlobCenter.x);
	blobMsg.addFloatArg(blob.headBlobCenter.y);
	blobMsg.addFloatArg(blob.headBlobCenter.z);

	sendMessageToTrackingClients(blobMsg);
}
#else
void TrackingNetworkManager::sendSkeletonData(const Skeleton& skel) {
	ofxOscMessage skeletonMsg;
	skeletonMsg.setAddress("/ks/server/track/skeleton");

	skeletonMsg.addIntArg(mServerID.get());

	Joint head = skel.joints[nuitrack::JOINT_HEAD];
	skeletonMsg.addFloatArg(head.pos.x);
	skeletonMsg.addFloatArg(head.pos.y);
	skeletonMsg.addFloatArg(head.pos.z);
	skeletonMsg.addFloatArg(head.confidence);

	sendMessageToTrackingClients(skeletonMsg);
}
#endif

void TrackingNetworkManager::sendMessageToTrackingClients(ofxOscMessage _msg){
    for(int j = 0; j < knownClients.size(); j++){
        broadcastSender.setup(knownClients[j].clientDestination, knownClients[j].clientSendPort);
        broadcastSender.sendMessage(_msg);
    }
}

void TrackingNetworkManager::checkTrackingClients(long _currentMillis){
    for(int i = 0; i < knownClients.size(); i++){
        if(!knownClients[i].stillAlive(_currentMillis)){
            ofLog(OF_LOG_NOTICE) << "Server removed TrackingClient ip: "
				<< knownClients[i].clientDestination
				<< " port:  " << ofToString(knownClients[i].clientSendPort);
            knownClients[i] = knownClients.back();
            knownClients.pop_back();
            i--;
        }
    }
}

int TrackingNetworkManager::getTrackingClientIndex(string _ip, int _port){
    for(int i = 0; i < knownClients.size(); i++){
        if(knownClients[i].clientDestination.find(_ip) != string::npos && knownClients[i].clientSendPort == _port){
            return i;
        }
    }
    knownClients.push_back(TrackingClient(_ip, _port));
    ofLog(OF_LOG_NOTICE) << "Server added new TrackingClient ip: " << _ip
		<< " port:  " + ofToString(_port)
		<< " knownClients:  " << ofToString(knownClients.size());
    return knownClients.size() -1;
}

//--------------------------------------------------------------
string TrackingNetworkManager::getOscMsgAsString(ofxOscMessage m){
	string msg_string;
	msg_string = m.getAddress();
	msg_string += ":";
	for(int i = 0; i < m.getNumArgs(); i++){
		// get the argument type
		msg_string += " " + m.getArgTypeName(i);
		msg_string += ":";
		// display the argument - make sure we get the right type
		if(m.getArgType(i) == OFXOSC_TYPE_INT32){
			msg_string += ofToString(m.getArgAsInt32(i));
		}
		else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
			msg_string += ofToString(m.getArgAsFloat(i));
		}
		else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
			msg_string += m.getArgAsString(i);
		}
		else{
			msg_string += "unknown";
		}
	}
	return msg_string;
}

vector<string> TrackingNetworkManager::matchesInRegex(string _str, string _reg) {
	regex regEx(_reg, regex_constants::icase);
	vector<string> results;
	auto wordsBegin = sregex_iterator(_str.begin(), _str.end(), regEx);
	auto wordsEnd = sregex_iterator();

	for (std::sregex_iterator i = wordsBegin; i != wordsEnd; ++i) {
		smatch m = *i;
		results.push_back(m.str());
	}
	return results;
}


