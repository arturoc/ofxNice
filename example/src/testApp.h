#pragma once

#include "ofMain.h"
#include "ofxNice.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void onClientLocalCandidatesGathered(vector<ofxICECandidate> & candidates);
		void onServerLocalCandidatesGathered(vector<ofxICECandidate> & candidates);

		void onServerDataReceived(ofBuffer & data);
		void onClientDataReceived(ofBuffer & data);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofxNiceAgent agentClient;
		ofxNiceAgent agentServer;
		shared_ptr<ofxNiceStream> streamClient;
		shared_ptr<ofxNiceStream> streamServer;

		string textToSend;
};
