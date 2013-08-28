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

class ofxNiceStream;

class ofxNiceAgent {
public:
	ofxNiceAgent();
	virtual ~ofxNiceAgent();

	void setup(const string & stunServer, int stunServerPort, bool controlling, GMainLoop * mainLoop = NULL, NiceCompatibility compatibility=NICE_COMPATIBILITY_RFC5245);
	void addStream(ofxNiceStream * stream);

	NiceAgent * getAgent();

	GMainContext * getContext();
private:
	NiceAgent * agent;
	GMainContext * ctx;
	map<guint,ofxNiceStream*> streamsIndex;

	// nice callbacks
	static void cb_candidate_gathering_done(NiceAgent *agent, guint stream_id,
	    gpointer data);
	static void cb_new_selected_pair(NiceAgent *agent, guint stream_id,
	    guint component_id, gchar *lfoundation,
	    gchar *rfoundation, gpointer data);
	static void cb_component_state_changed(NiceAgent *agent, guint stream_id,
	    guint component_id, guint state,
	    gpointer data);
	static void cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
	    guint len, gchar *buf, gpointer data);
};

#endif /* OFXNICEAGENT_H_ */
