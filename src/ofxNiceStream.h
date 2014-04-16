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
#include "ofFileUtils.h"


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

/// An ICE stream represents a channel of data in a connection with another peer
/// if we are opening a connection using video and audio there'll usually be a
/// stream for audio and another for video, both of them should be passed to the same
/// agent
class ofxNiceStream {
public:
	ofxNiceStream();
	virtual ~ofxNiceStream();

	/// set a specific log name for debugging purposes so we can know
	/// which channel failed connecting...
	void setLogName(const string & name);

	/// setup this stream from an agent + the number of components, for example
	/// a video stream can have 2 components, 1 for receiving and 1 for sending
	void setup(ofxNiceAgent & agent, int numberComponents);

	/// checks all the possible IPs this computer has that will be sent to the
	/// other peer to check which one is the best way of connecting to this computer
	/// for exammple, IP in local network, NAT router external IP...
	/// this method works asynchronously since it involves some calls to the STUNT server
	/// we need to listen the localCandidatesGathered event to receive the candidates
	void gatherLocalCandidates();

	/// we should receive this from the other peer, usually through a session initiation
	/// protocol like XMPP or SIP, once we have this credentials and the remote cnadidates
	/// we can start the conection with the other peer
	void setRemoteCredentials(const string & ufrag, const string & pwd);

	/// we should receive this from the other peer, usually thorugh a session initiation
	/// protocol like XMPP or SIP
	void setRemoteCandidates(const vector<ofxICECandidate> & candidates);

	/// once the connection is stablisehd we can send data to the other peer using this method
	/// this version sends a null terminated string and should not be used for sending binary data
	/// returns the number of bytes sent or a negative error code
	int sendData(const string & data, int component = 1);

	/// once the connection is stablisehd we can send data to the other peer using this method
	/// this version can be used to send binary data
	/// returns the number of bytes sent or a negative error code
	int sendRawData(const char * data, size_t size, int component = 1);

	/// get local ufrag usually used to send it to the other peer
	string getLocalUFrag();

	/// get the local pwd, usually used to send it to the other peer
	string getLocalPwd();

	/// listen this event to receive the local candidates to be sent to the remote peer
	ofEvent <vector<ofxICECandidate> > localCandidatesGathered;

	/// listen this event to receive the data sent from the remote peer
	ofEvent <ofBuffer> dataReceived;

	/// listen this event to know when a component is ready this
	ofEvent <int> componentReady;

	/// listen this event to know when a component has finished connecting
	ofEvent <int> componentConnected;

	/// listen to this event to know if a component failed connecting
	ofEvent <int> componentFailed;

	/// listen to this event to know if a reliable component is writable, since TCP
	/// doesn't drop packages if the internal buffers are full, sending will fail and return
	/// a negative error code, in that case we need to wait till this event is triggered for
	/// the specific component before atemting to send again
	ofEvent <int> reliableComponentWritable;

	/// get the internal Nice agent, usually only used internally by the addon
	NiceAgent * getAgent();

	/// get the ID of this stream
	int getStreamID();

	/// get the log name of this stream
	string getName();

	/// this needs to be called before gathering the local candidates
	void listen();

private:
	friend class ofxNiceAgent;
	void gatheringDone();
	void stateChanged(guint component_id, guint state);
	void pairSelected(guint component_id, gchar *lfoundation,  gchar *rfoundation);
	void reliableTransportWritable(guint component_id);

	static void cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
	    guint len, gchar *buf, gpointer data);

	ofxNiceAgent * agent;
	int streamID;
	int numberComponents;
	string logName;
};

#endif /* OFXNICE_H_ */
