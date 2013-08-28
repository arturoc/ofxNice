/*
 * This file is part of the Nice GLib ICE library.
 *
 * (C) 2006-2009 Collabora Ltd.
 *  Contact: Youness Alaoui
 * (C) 2006-2009 Nokia Corporation. All rights reserved.
 *  Contact: Kai Vehmanen
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Nice GLib ICE library.
 *
 * The Initial Developers of the Original Code are Collabora Ltd and Nokia
 * Corporation. All Rights Reserved.
 *
 * Contributors:
 *   Dafydd Harries, Collabora Ltd.
 *   Youness Alaoui, Collabora Ltd.
 *   Kai Vehmanen, Nokia
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * the GNU Lesser General Public License Version 2.1 (the "LGPL"), in which
 * case the provisions of LGPL are applicable instead of those above. If you
 * wish to allow use of your version of this file only under the terms of the
 * LGPL and not to allow others to use your version of this file under the
 * MPL, indicate your decision by deleting the provisions above and replace
 * them with the notice and other provisions required by the LGPL. If you do
 * not delete the provisions above, a recipient may use your version of this
 * file under either the MPL or the LGPL.
 */

#ifndef _NICE_CONNCHECK_H
#define _NICE_CONNCHECK_H

/* note: this is a private header to libnice */

#include "agent.h"
#include "stream.h"
#include "stun/stunagent.h"
#include "stun/usages/timer.h"

#define NICE_CANDIDATE_PAIR_MAX_FOUNDATION        NICE_CANDIDATE_MAX_FOUNDATION*2

typedef enum
{
  NICE_CHECK_WAITING = 1,   /**< waiting to be scheduled */
  NICE_CHECK_IN_PROGRESS,   /**< conn. checks started */
  NICE_CHECK_SUCCEEDED,     /**< conn. succesfully checked */
  NICE_CHECK_FAILED,        /**< no connectivity, retransmissions ceased */
  NICE_CHECK_FROZEN,        /**< waiting to be scheduled to WAITING */
  NICE_CHECK_CANCELLED,     /**< check cancelled */
  NICE_CHECK_DISCOVERED     /**< a valid candidate pair not on check list */
} NiceCheckState;

typedef struct _CandidateCheckPair CandidateCheckPair;

struct _CandidateCheckPair
{
  NiceAgent *agent;         /* back pointer to owner */
  guint stream_id;
  guint component_id;
  NiceCandidate *local;
  NiceCandidate *remote;
  gchar foundation[NICE_CANDIDATE_PAIR_MAX_FOUNDATION];
  NiceCheckState state;
  gboolean nominated;
  gboolean controlling;
  gboolean timer_restarted;
  guint64 priority;
  GTimeVal next_tick;       /* next tick timestamp */
  StunTimer timer;
  uint8_t stun_buffer[STUN_MAX_MESSAGE_SIZE];
  StunMessage stun_message;
};

int conn_check_add_for_candidate (NiceAgent *agent, guint stream_id, Component *component, NiceCandidate *remote);
int conn_check_add_for_local_candidate (NiceAgent *agent, guint stream_id, Component *component, NiceCandidate *local);
void conn_check_free_item (gpointer data, gpointer user_data);
void conn_check_free (NiceAgent *agent);
gboolean conn_check_schedule_next (NiceAgent *agent);
int conn_check_send (NiceAgent *agent, CandidateCheckPair *pair);
gboolean conn_check_prune_stream (NiceAgent *agent, Stream *stream);
gboolean conn_check_handle_inbound_stun (NiceAgent *agent, Stream *stream, Component *component, NiceSocket *udp_socket, const NiceAddress *from, gchar *buf, guint len);
gint conn_check_compare (const CandidateCheckPair *a, const CandidateCheckPair *b);
void conn_check_remote_candidates_set(NiceAgent *agent);

#endif /*_NICE_CONNCHECK_H */
