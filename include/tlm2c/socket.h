/*
 * socket.h
 *
 * Copyright (C) 2014, GreenSocs Ltd.
 *
 * Developped by Konrad Frederic <fred.konrad@greensocs.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * Linking GreenSocs code, statically or dynamically with other modules
 * is making a combined work based on GreenSocs code. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders, GreenSocs
 * Ltd, give you permission to combine GreenSocs code with free software
 * programs or libraries that are released under the GNU LGPL, under the
 * OSCI license, under the OCP TLM Kit Research License Agreement or
 * under the OVP evaluation license.You may copy and distribute such a
 * system following the terms of the GNU GPL and the licenses of the
 * other code concerned.
 *
 * Note that people who make modified versions of GreenSocs code are not
 * obligated to grant this special exception for their modified versions;
 * it is their choice whether to do so. The GNU General Public License
 * gives permission to release a modified version without this exception;
 * this exception also makes it possible to release a modified version
 * which carries forward this exception.
 *
 */

#ifndef SOCKET_H
#define SOCKET_H

typedef struct Socket Socket;
typedef struct TargetSocket TargetSocket;
typedef struct InitiatorSocket InitiatorSocket;

#include "genericPayload.h"
#include "dmi.h"
#include "model.h"

/*
 * function pointer type.
 */
typedef void (*blocking_transport)(void *handle, Payload *p);
typedef int (*get_direct_mem_ptr)(void *handle, Payload *p, DMIData *d);

/**
 * \func InitiatorSocket *socket_initiator_create(const char name).
 */
InitiatorSocket *socket_initiator_create(const char *name);

/**
 * \func TargetSocket *socket_target_create(const char *name).
 */
TargetSocket *socket_target_create(const char *name);

void socket_destroy_list(Socket *socket);

/**
 * \func b_transport(..).
 */
void b_transport(InitiatorSocket *master, Payload *p);

int tlm2c_get_direct_mem_ptr(InitiatorSocket *master, Payload *p,
                             DMIData *dmi);

/**
 * \func tlm2c_bind(InitiatorSocket *, TargetSocket *)
 */
void tlm2c_bind(InitiatorSocket *master, TargetSocket *slave);

void socket_target_register_b_transport(TargetSocket *target, void *handle,
                                        blocking_transport bt);

void tlm2c_socket_target_register_dmi(TargetSocket *target,
                                      get_direct_mem_ptr dp);

Socket *tlm2c_socket_get_by_name(const char *name);

#endif /* !SOCKET_H */
