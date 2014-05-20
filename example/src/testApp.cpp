#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	agentServer.setup(true,NULL,NICE_COMPATIBILITY_RFC5245);
	agentServer.setStunServer("77.72.174.165",3478);
	streamServer->setup(agentServer,1);
	agentServer.addStream(streamServer);
	streamServer->listen();
	streamServer->gatherLocalCandidates();
	ofAddListener(streamServer->localCandidatesGathered,this,&testApp::onServerLocalCandidatesGathered);
	ofAddListener(streamServer->dataReceived,this,&testApp::onServerDataReceived);

	agentClient.setup(false,NULL,NICE_COMPATIBILITY_RFC5245);
	agentServer.setStunServer("77.72.174.165",3478);
	streamClient->setup(agentClient,1);
	agentClient.addStream(streamClient);
	streamClient->listen();
	streamClient->gatherLocalCandidates();
	ofAddListener(streamClient->localCandidatesGathered,this,&testApp::onClientLocalCandidatesGathered);
	ofAddListener(streamClient->dataReceived,this,&testApp::onClientDataReceived);
}

void testApp::onClientLocalCandidatesGathered(vector<ofxICECandidate> & candidates){
	cout << "setting server remote info " << endl;
	streamServer->setRemoteCredentials(streamClient->getLocalUFrag(),streamClient->getLocalPwd());
	streamServer->setRemoteCandidates(candidates);
}

void testApp::onServerLocalCandidatesGathered(vector<ofxICECandidate> & candidates){
	cout << "setting client remote info " << endl;
	streamClient->setRemoteCredentials(streamServer->getLocalUFrag(),streamServer->getLocalPwd());
	streamClient->setRemoteCandidates(candidates);
}

void testApp::onServerDataReceived(ofBuffer & data){
	cout << "server: " << data << endl;
}

void testApp::onClientDataReceived(ofBuffer & data){
	cout << "client " << data << endl;
}
//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key!=OF_KEY_RETURN){
		textToSend += (char)key;
	}else{
		streamServer->sendData(textToSend,1);
		streamClient->sendData(textToSend,1);
		textToSend = "";
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
