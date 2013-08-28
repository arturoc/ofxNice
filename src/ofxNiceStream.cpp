/*
 * ofxNice.cpp
 *
 *  Created on: Aug 27, 2013
 *      Author: arturo
 */

#include "ofxNiceStream.h"
#include "ofLog.h"
#include "ofUtils.h"
#include "ofGstUtils.h"
#include "ofxNiceAgent.h"
#include <glib/gslist.h>


static const gchar *candidate_type_name[] = {"host", "srflx", "prflx", "relay"};

static const gchar *state_name[] = {"disconnected", "gathering", "connecting",
                                    "connected", "ready", "failed"};


void ofxNiceStream::cb_nice_recv(NiceAgent *agent, guint stream_id, guint component_id,
    guint len, gchar *buf, gpointer data)
{
	ofLogNotice() << component_id << ": " << buf;
}

ofxNiceStream::ofxNiceStream()
:agent(NULL)
,streamID(0)
,numberComponents(0)
,logName("ofxNice"){
	// TODO Auto-generated constructor stub

}

ofxNiceStream::~ofxNiceStream() {
	// TODO Auto-generated destructor stub
}

void ofxNiceStream::setLogName(const string & name){
	logName = name;
}

void ofxNiceStream::setup(ofxNiceAgent & agent, int numberComponents){
	this->agent = &agent;
	this->numberComponents = numberComponents;

	streamID = nice_agent_add_stream(agent.getAgent(),numberComponents);
	if (streamID == 0)
	    ofLogError(logName) << "Failed to add stream";
}

void ofxNiceStream::listen(){
	// Attach to the component to receive the data
	// Without this call, candidates cannot be gathered
	for(int i=1;i<numberComponents+1;i++){
		nice_agent_attach_recv(agent->getAgent(), streamID, i,  agent->getContext(), cb_nice_recv, this);
	}
}

void ofxNiceStream::gatherLocalCandidates(){
	if (!nice_agent_gather_candidates(agent->getAgent(), streamID)){
		ofLogError(logName) << "Failed to start candidate gathering";
	}
}

void ofxNiceStream::setRemoteCredentials(const string & ufrag, const string & pwd){
	nice_agent_set_remote_credentials(agent->getAgent(), streamID, ufrag.c_str(), pwd.c_str());
}

void ofxNiceStream::setRemoteCandidates(const vector<ofxICECandidate> & candidates){
	GSList * candidates_lists[numberComponents];
    for(int i=0;i<numberComponents;i++){
        candidates_lists[i] = NULL;
    }

	for(size_t j=0;j<candidates.size();j++){
		const ofxICECandidate & candidate =  candidates[j];

		ofLogNotice(logName) << "creating candidate for component " << candidate.component;
		NiceCandidate * niceCandidate;
		if(candidate.type=="host"){
			niceCandidate = nice_candidate_new(NICE_CANDIDATE_TYPE_HOST);
		}else if(candidate.type=="srflx"){
			niceCandidate = nice_candidate_new(NICE_CANDIDATE_TYPE_SERVER_REFLEXIVE);
		}else if(candidate.type=="prflx"){
			niceCandidate = nice_candidate_new(NICE_CANDIDATE_TYPE_PEER_REFLEXIVE);
		}else if(candidate.type=="relay"){
			niceCandidate = nice_candidate_new(NICE_CANDIDATE_TYPE_RELAYED);
		}
		nice_address_set_from_string(&niceCandidate->addr,candidate.ip.c_str());
		nice_address_set_port(&niceCandidate->addr,candidate.port);
		niceCandidate->transport = NICE_CANDIDATE_TRANSPORT_UDP;
		niceCandidate->priority = candidate.priority;
		niceCandidate->stream_id = streamID;
		niceCandidate->component_id = candidate.component;
		strncpy(niceCandidate->foundation, ofToString(candidate.foundation).c_str(), NICE_CANDIDATE_MAX_FOUNDATION);


		candidates_lists[candidate.component-1] = g_slist_append(candidates_lists[candidate.component-1],niceCandidate);
	}

	for(int i=0;i<numberComponents;i++){
		ofLogNotice(logName) << "adding remote candidates for component " << i+1 << " " << candidates_lists[i];
		if(candidates_lists[i] && nice_agent_set_remote_candidates(agent->getAgent(),streamID,i+1,candidates_lists[i])<1){
			ofLogError(logName) << "Error setting remote candidates for component " << i;
		}
	}
}

string ofxNiceStream::getLocalUFrag(){
	gchar * ufrag, * pwd;
	nice_agent_get_local_credentials(agent->getAgent(),streamID,&ufrag,&pwd);
	return ufrag;
}

string ofxNiceStream::getLocalPwd(){
	gchar * ufrag, * pwd;
	nice_agent_get_local_credentials(agent->getAgent(),streamID,&ufrag,&pwd);
	return pwd;
}

void ofxNiceStream::sendData(const string & data, int component){
	nice_agent_send(agent->getAgent(),streamID,component,data.size()+1,data.c_str());
}

void ofxNiceStream::sendRawData(const char * data, size_t size, int component){
	nice_agent_send(agent->getAgent(),streamID,component,size,data);
}

NiceAgent * ofxNiceStream::getAgent(){
	return agent->getAgent();
}

int ofxNiceStream::getStreamID(){
	return streamID;
}


string ofxNiceStream::getName(){
	return logName;
}

void ofxNiceStream::gatheringDone(){
	ofLogNotice(logName) << "candidate gathering done";

	gchar *local_ufrag = NULL;
	gchar *local_password = NULL;
	gchar ipaddr[INET6_ADDRSTRLEN];
	GSList *cands = NULL, *item;

	if (!nice_agent_get_local_credentials(agent->getAgent(), streamID, &local_ufrag, &local_password)){
		ofLogError(logName) << "couldn't get local credentials";
		return;
	}


	ofLogNotice(logName) << local_ufrag << " " << local_password;


	vector<ofxICECandidate> candidates;


	for(int i=1;i<numberComponents+1;i++){
		cands = nice_agent_get_local_candidates(agent->getAgent(), streamID, i);
		if (cands == NULL){
			ofLogError() << "coudn't get local candidates";
			return;
		}

		for (item = cands; item; item = item->next) {
			NiceCandidate *c = (NiceCandidate *)item->data;

			nice_address_to_string(&c->addr, ipaddr);

			// (foundation),(prio),(addr),(port),(type)
			ofLogNotice(logName) <<
				"component: " << i << " " <<
				" priority: " << c->priority << " " <<
				" address: " << ipaddr << " " <<
				":" << nice_address_get_port(&c->addr) << " " <<
				" type: " << candidate_type_name[c->type];

			ofxICECandidate candidate;
			candidate.component=i;
			candidate.foundation=c->foundation;
			//content.transport.candidates[0].id="el0747fg11";
			candidate.ip=ipaddr;
			candidate.network=1;
			candidate.port=nice_address_get_port(&c->addr);
			candidate.priority=c->priority;
			candidate.protocol="udp";
			candidate.type = candidate_type_name[c->type];

			candidates.push_back(candidate);
		}

		g_slist_free(cands);
	}

	ofNotifyEvent(localCandidatesGathered,candidates,this);

}


void ofxNiceStream::stateChanged(guint component_id, guint state){
	ofLogNotice(logName) << "state changed for component " << component_id << " " << state_name[state];

	if (state == NICE_COMPONENT_STATE_READY) {
		ofLogNotice(logName) << "nice ready for component " << component_id;
	} else if (state == NICE_COMPONENT_STATE_FAILED) {
		ofLogError(logName) << "nice failed for component " << component_id;
	}
}

void ofxNiceStream::pairSelected(guint component_id, gchar *lfoundation,  gchar *rfoundation){
	ofLogError(logName) << "selected pair " << lfoundation << ", " <<  rfoundation << " for component " << component_id;
}
