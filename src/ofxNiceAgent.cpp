/*
 * ofxNiceAgent.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: arturo
 */

#include "ofxNiceAgent.h"
#include "ofLog.h"
#include "ofGstUtils.h"
#include "ofxNiceStream.h"

ofxNiceAgent::ofxNiceAgent()
:agent(NULL)
,ctx(NULL)
{
	// TODO Auto-generated constructor stub

}

ofxNiceAgent::~ofxNiceAgent() {
	// TODO Auto-generated destructor stub
}

void ofxNiceAgent::cb_candidate_gathering_done(NiceAgent *agent, guint stream_id, gpointer data){

	ofxNiceAgent * client = (ofxNiceAgent *) data;
	client->streamsIndex[stream_id]->gatheringDone();
}

void ofxNiceAgent::cb_component_state_changed(NiceAgent *agent, guint stream_id,
    guint component_id, guint state,
    gpointer data)
{

	ofxNiceAgent * client = (ofxNiceAgent *) data;
	client->streamsIndex[stream_id]->stateChanged(component_id, state);
}


void ofxNiceAgent::cb_new_selected_pair(NiceAgent *agent, guint stream_id,
    guint component_id, gchar *lfoundation,
    gchar *rfoundation, gpointer data)
{
	ofxNiceAgent * client = (ofxNiceAgent *) data;
	client->streamsIndex[stream_id]->pairSelected(component_id,lfoundation,rfoundation);

}

void ofxNiceAgent::setup(const string & stunServer, int stunServerPort, bool controlling, GMainLoop * mainLoop, NiceCompatibility compatibility){
	if(mainLoop){
		ctx = g_main_loop_get_context(mainLoop);
	}else{
		ofGstUtils::startGstMainLoop();
		ctx = g_main_loop_get_context(ofGstUtils::getGstMainLoop());
	}
	agent = nice_agent_new(ctx,compatibility);


	g_object_set(G_OBJECT(agent), "stun-server", stunServer.c_str(), NULL);
	g_object_set(G_OBJECT(agent), "stun-server-port", stunServerPort, NULL);
	g_object_set(G_OBJECT(agent), "controlling-mode", controlling?1:0, NULL);

	// Connect to the signals
	g_signal_connect(G_OBJECT(agent), "candidate-gathering-done", G_CALLBACK(cb_candidate_gathering_done), this);

	g_signal_connect(G_OBJECT(agent), "new-selected-pair", G_CALLBACK(cb_new_selected_pair), this);

	g_signal_connect(G_OBJECT(agent), "component-state-changed", G_CALLBACK(cb_component_state_changed), this);
}

GMainContext * ofxNiceAgent::getContext(){
	return ctx;
}

void ofxNiceAgent::addStream(ofxNiceStream * stream){
	if(stream->getStreamID()==0) ofLogError() << "trying to add stream that has not been setup yet";
	streamsIndex[stream->getStreamID()] = stream;
}

NiceAgent * ofxNiceAgent::getAgent(){
	return agent;
}
