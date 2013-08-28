/*
 * ofxNice.h
 *
 *  Created on: Aug 27, 2013
 *      Author: arturo
 */

#ifndef OFXNICESTREAM_H_
#define OFXNICESTREAM_H_

#include <agent.h>
#include "ofConstants.h"
#include "ofEvents.h"


struct ofxICECandidate{
	int component;
	string foundation;
	int generation;
	string id;
	string ip;
	int network;
	int port;
	int priority;
	string protocol;
	string type;
};

class ofxNiceAgent;

class ofxNiceStream {
public:
	ofxNiceStream();
	virtual ~ofxNiceStream();

	void setLogName(const string & name);
	void setup(ofxNiceAgent & agent, int numberComponents);

	void gatherLocalCandidates();
	void setRemoteCredentials(const string & ufrag, const string & pwd);
	void setRemoteCandidates(const vector<ofxICECandidate> & candidates);
	void sendData(const string & data, int component = 1);
	void sendRawData(const char * data, size_t size, int component = 1);

	string getLocalUFrag();
	string getLocalPwd();

	ofEvent < vector<ofxICECandidate> > localCandidatesGathered;

	NiceAgent * getAgent();
	int getStreamID();

	string getName();

	void listen();

private:
	friend class ofxNiceAgent;
	void gatheringDone();
	void stateChanged(guint component_id, guint state);
	void pairSelected(guint component_id, gchar *lfoundation,  gchar *rfoundation);
	static void cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
	    guint len, gchar *buf, gpointer data);

	ofxNiceAgent * agent;
	int streamID;
	int numberComponents;
	string logName;
};

#endif /* OFXNICE_H_ */
