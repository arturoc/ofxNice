/*
 * ofxNiceAgent.h
 *
 *  Created on: Aug 27, 2013
 *      Author: arturo
 */

#ifndef OFXNICEAGENT_H_
#define OFXNICEAGENT_H_

#include <agent.h>
#include <map>
#include "ofConstants.h"
#include "ofTypes.h"

class ofxNiceStream;

/// ofxNiceAgent represents an ICE agent. The ICE protocol allows to send data across
/// NAT routers. The way it works is by using a third party server usually only for stablishing
/// a connection. The peer that wants to start a connection connects to a STUNT server sending some
/// UDP pacakges to it, that way the NAT router keeps an open port to receive the answer, in most
/// routers it doens't matter where that answer comes from it'll be redirected to the internal
/// computer that initiated the connection allowing a peer to peer connection across NAT.
///
/// This class in particular represents an agent, that is the object responsible for opening the
/// connection to the STUNT server. An agent can contain several streams that will be used for sending
/// different data like video, audio... to the other peer
class ofxNiceAgent {
public:
	ofxNiceAgent();
	virtual ~ofxNiceAgent();

	/// starts a connection with the STUNT server specifying it's address, port,
	/// if this side is controlling the connection (usually if it's the side that initiated the
	/// connection.
	/// optionally we can specify a glib events loop, the ICE protocol specification we are
	/// using and if the connection should be reliable (TCP over UDP)
	void setup(const string & stunServer, int stunServerPort, bool controlling, GMainLoop * mainLoop = NULL, NiceCompatibility compatibility=NICE_COMPATIBILITY_RFC5245, bool reliable=false);

	/// add a stream to this agent
	void addStream(shared_ptr<ofxNiceStream> stream);

	/// add a TURN server to use for during discovery
	void addRelay(const string & ip, uint port, const string & user, const string & pwd, NiceRelayType type);

	/// get the internal NiceAgent, usually only for internal usage of the addon
	NiceAgent * getAgent();

	/// get the glib context, usually only for internal usage of the addon
	GMainContext * getContext();
private:
	NiceAgent * agent;
	GMainContext * ctx;
	map<guint,shared_ptr<ofxNiceStream> > streamsIndex;

	struct Relay{
		Relay(const string & ip, uint port, const string & user, const string & pwd, NiceRelayType type)
		:ip(ip)
		,port(port)
		,user(user)
		,pwd(pwd)
		,type(type)
		{}

		string ip;
		uint port;
		string user;
		string pwd;
		NiceRelayType type;
	};

	vector<Relay> relays;

	// nice callbacks
	static void cb_candidate_gathering_done(NiceAgent *agent, guint stream_id, ofxNiceAgent * client);
	static void cb_new_selected_pair(NiceAgent *agent, guint stream_id,
	    guint component_id, gchar *lfoundation,
	    gchar *rfoundation, ofxNiceAgent * client);
	static void cb_component_state_changed(NiceAgent *agent, guint stream_id,
	    guint component_id, guint state,
	    ofxNiceAgent * client);
	static void cb_reliable_transport_writable(NiceAgent *agent, guint stream_id,  guint component_id, ofxNiceAgent * client);
};

#endif /* OFXNICEAGENT_H_ */
