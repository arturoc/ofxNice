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

void ofxNiceAgent::cb_candidate_gathering_done(NiceAgent *agent, guint stream_id, ofxNiceAgent * client){
	client->streamsIndex[stream_id]->gatheringDone();
}

void ofxNiceAgent::cb_component_state_changed(NiceAgent *agent, guint stream_id,
    guint component_id, guint state,
    ofxNiceAgent * client)
{
	client->streamsIndex[stream_id]->stateChanged(component_id, state);
}


void ofxNiceAgent::cb_new_selected_pair(NiceAgent *agent, guint stream_id,
    guint component_id, gchar *lfoundation,
    gchar *rfoundation, ofxNiceAgent * client)
{
	client->streamsIndex[stream_id]->pairSelected(component_id,lfoundation,rfoundation);

}

void ofxNiceAgent::cb_reliable_transport_writable(NiceAgent *agent, guint stream_id,  guint component_id, ofxNiceAgent * client)
{
	client->streamsIndex[stream_id]->reliableTransportWritable(component_id);
}

void ofxNiceAgent::setup(const string & stunServer, int stunServerPort, bool controlling, GMainLoop * mainLoop, NiceCompatibility compatibility, bool reliable){
	if(mainLoop){
		ctx = g_main_loop_get_context(mainLoop);
	}else{
		ofGstUtils::startGstMainLoop();
		ctx = g_main_loop_get_context(ofGstUtils::getGstMainLoop());
	}
	if(reliable){
		agent = nice_agent_new_reliable(ctx,compatibility);
	}else{
		agent = nice_agent_new(ctx,compatibility);
	}

	g_object_set(G_OBJECT(agent), "stun-server", stunServer.c_str(), NULL);
	g_object_set(G_OBJECT(agent), "stun-server-port", stunServerPort, NULL);
	g_object_set(G_OBJECT(agent), "controlling-mode", controlling?1:0, NULL);
	//TODO: activating upnp crashes for some reason
	g_object_set(G_OBJECT(agent), "upnp", FALSE, NULL);

	// Connect to the signals
	g_signal_connect(G_OBJECT(agent), "candidate-gathering-done", G_CALLBACK(cb_candidate_gathering_done), this);

	g_signal_connect(G_OBJECT(agent), "new-selected-pair", G_CALLBACK(cb_new_selected_pair), this);

	g_signal_connect(G_OBJECT(agent), "component-state-changed", G_CALLBACK(cb_component_state_changed), this);

	if(reliable){
		g_signal_connect(G_OBJECT(agent), "reliable-transport-writable", G_CALLBACK(cb_reliable_transport_writable), this);
	}
}

void ofxNiceAgent::setProxy(const string & ip, uint port, NiceProxyType type, const string & user, const string & pwd){
	if(ip!=""){
		g_object_set(G_OBJECT(agent), "proxy-ip", ip.c_str(), NULL);
	}else{
		g_object_set(G_OBJECT(agent), "proxy-ip", NULL, NULL);
	}
	g_object_set(G_OBJECT(agent), "proxy-port", port, NULL);
	g_object_set(G_OBJECT(agent), "proxy-type", type, NULL);
	if(user!=""){
		g_object_set(G_OBJECT(agent), "proxy-username", user.c_str(), NULL);
	}else{
		g_object_set(G_OBJECT(agent), "proxy-username", NULL, NULL);
	}
	if(user!=""){
		g_object_set(G_OBJECT(agent), "proxy-password", pwd.c_str(), NULL);
	}else{
		g_object_set(G_OBJECT(agent), "proxy-password", NULL, NULL);
	}
}

void ofxNiceAgent::addRelay(const string & ip, uint port, const string & user, const string & pwd, NiceRelayType type){
	relays.push_back(Relay(ip,port,user,pwd,type));
}

GMainContext * ofxNiceAgent::getContext(){
	return ctx;
}

void ofxNiceAgent::addStream(shared_ptr<ofxNiceStream> stream){
	if(stream->getStreamID()==0) ofLogError() << "trying to add stream that has not been setup yet";
	streamsIndex[stream->getStreamID()] = stream;

	for(size_t r=0;r<relays.size();r++){
		const Relay & relay = relays[r];
		const char * user=NULL;
		const char * pwd=NULL;
		if(relay.user!=""){
			user = relay.user.c_str();
		}
		if(relay.pwd!=""){
			pwd = relay.pwd.c_str();
		}
		for(int i=0;i<stream->getNumComponents();i++){
			nice_agent_set_relay_info(agent,stream->getStreamID(),i+1,relay.ip.c_str(),relay.port,user,pwd,relay.type);
		}
	}
}

NiceAgent * ofxNiceAgent::getAgent(){
	return agent;
}


void ofxNiceEnableDebug(){
#ifdef TARGET_LINUX
	nice_debug_enable(true);
#else
	stun_debug_enable();
#endif
}

void ofxNiceDisableDebug(){
#ifdef TARGET_LINUX
	nice_debug_disable(true);
#else
	stun_debug_disable();
#endif
}
