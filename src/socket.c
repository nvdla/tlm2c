/*
 * socket.c
 *
 * Copyright (C) 2014, GreenSocs Ltd.
 *
 * Developed by Konrad Frederic <fred.konrad@greensocs.com>
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

#include "tlm2c/socket.h"
#include "tlm2c/spinlock.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

tlm2c_spinlock socket_list_lock = 0;
Socket *socket_list_head = NULL;

/*
 * Private function from payload.
 */
void payload_set_source_initiator(Payload *p, InitiatorSocket *source);
InitiatorSocket *payload_get_source_initiator(Payload *p);

/*
 * Default functions for the target socket.
 */
void default_blocking_transport(void *handle, Payload *p);
int default_get_direct_mem_ptr(void *handle, Payload *p, TLM2CDMIData *d);
void default_invalidate_direct_mem_ptr(void *handle, uint64_t start,
                                                     uint64_t end);

struct Socket
{
  char *name;
  Socket *bound;      /*< The socket bound to this one. */
  Model *model;
  Socket *next;       /*< Next in the module list.      */
  Socket *next_list;  /*< Next in the complete list.    */
};

struct InitiatorSocket; /* To avoid circular reference issue */

struct TargetSocket
{
  struct Socket base;
  InitiatorSocket *is; /*< Other side socket (initiator) */
  blocking_transport bt;
  void *handle; /*< Handle associated to get_direct_mem_ptr and b_transport */
  get_direct_mem_ptr dp;
};

struct InitiatorSocket
{
  struct Socket base;
  TargetSocket *ts; /*< Other side socket (target) */
  void *handle; /*<  Handle associated to the invalidate cb */
  invalidate_direct_mem_ptr idp; /**< DMI invalidate callback */
};

void tlm2c_socket_init(Socket *socket, const char *name)
{
  tlm2c_spinlock_lock(&socket_list_lock);
  socket->name = strdup(name);
  socket->next_list = socket_list_head;
  socket_list_head = socket;
  socket->bound = NULL;
  tlm2c_spinlock_unlock(&socket_list_lock);
}

InitiatorSocket *socket_initiator_create(const char *name)
{
  InitiatorSocket *s;

  s = (InitiatorSocket *)malloc(sizeof(InitiatorSocket));
  memset(s, 0, sizeof(InitiatorSocket));
  tlm2c_socket_init(&s->base, name);
  s->idp = default_invalidate_direct_mem_ptr;
  return s;
}

TargetSocket *socket_target_create(const char *name)
{
  TargetSocket *s;

  s = (TargetSocket *)malloc(sizeof(TargetSocket));
  memset(s, 0, sizeof(TargetSocket));

  tlm2c_socket_init(&s->base, name);
  s->bt = default_blocking_transport;
  s->dp = default_get_direct_mem_ptr;
  return s;
}

static void tlm2c_socket_destroy(Socket *socket)
{
  Socket *current = socket_list_head;

  if (!socket)
  {
    return;
  }

  tlm2c_spinlock_lock(&socket_list_lock);
  if (socket == socket_list_head)
  {
    socket_list_head = socket_list_head->next_list;
  }
  else
  {
    while (current != NULL)
    {
      if (current->next == socket)
      {
        current->next_list = socket->next_list;
        break;
      }
      current = current->next_list;
    }
  }
  free(socket);
  tlm2c_spinlock_unlock(&socket_list_lock);
}

void socket_destroy_list(Socket *socket)
{
  Socket *next;
  Socket *current = socket;

  while (current != NULL)
  {
    next = current->next;
    tlm2c_socket_destroy(current);
    current = next;
  }
}

/*
 * This is the default function when no blocking transport has been
 * registered.
 */
void default_blocking_transport(void *handle, Payload *p)
{
  const char *target_socket_name;
  target_socket_name = payload_get_source_initiator(p)->ts->base.name;

  fprintf(stderr, "error: no blocking function has been registered for this"
                  " target socket: %s\n", target_socket_name);
  abort();
}

/*
 * This is the default function when no get_direct_mem_ptr has been
 * registered.
 */
int default_get_direct_mem_ptr(void *handle, Payload *p, TLM2CDMIData *d)
{
  return 0;
}

/*
 * This is the default function when no invalidate_direct_mem_ptr has been
 * registered.
 *
 * @param handle handle
 * @param start invalidate start address
 * @param end invalidate end address
 */
void default_invalidate_direct_mem_ptr(void *handle, uint64_t start,
                                                     uint64_t end)
{
}

void b_transport(InitiatorSocket *master, Payload *p)
{
  /*
   * Keep the source initiator so we can report an error.
   */
  payload_set_source_initiator(p, master);
  master->ts->bt(master->ts->handle, p);
}

int tlm2c_get_direct_mem_ptr(InitiatorSocket *master, Payload *p,
                             TLM2CDMIData *dmi)
{
  /*
   * Keep the source initiator so we can report an error.
   */
  payload_set_source_initiator(p, master);
  return master->ts->dp(master->ts->handle, p, dmi);
}

void tlm2c_memory_invalidate_direct_mem_ptr(TargetSocket *target,
                                            uint64_t start,
                                            uint64_t end)
{
    target->is->idp(target->is->handle, start, end);
}

void tlm2c_bind(InitiatorSocket *master, TargetSocket *slave)
{
  if (master->base.bound)
  {
    fprintf(stderr, "error: initiator %s already bound.\n",
            master->base.name);
    abort();
  }
  if (slave->base.bound)
  {
    fprintf(stderr, "error: target %s already bound.\n", slave->base.name);
    abort();
  }

  master->ts = slave;
  slave->is = master;
  master->base.bound = (Socket *)slave;
  slave->base.bound = (Socket *)master;
}

Socket *tlm2c_socket_get_bound_socket(Socket *socket)
{
  return socket->bound;
}

void socket_target_register_b_transport(TargetSocket *target,
                                        void *handle,
                                        blocking_transport bt)
{
  target->bt = bt;
  target->handle = handle;
}

void tlm2c_socket_target_register_dmi(TargetSocket *target,
                                      get_direct_mem_ptr dp)
{
  target->dp = dp;
}

void tlm2c_socket_initiator_register_invalidate_direct_mem_ptr(
    InitiatorSocket *slave,
    void *handle,
    invalidate_direct_mem_ptr idp)
{
    slave->idp = idp;
    slave->handle = handle;
}

/*
 * Private.
 */
void socket_prefix_name(Socket *socket, char *prefix)
{
  size_t len = strlen(prefix) + strlen(socket->name) + 2;
  char *new_name = (char *)malloc(len);

  sprintf(new_name, "%s.%s", prefix, socket->name);
  free(socket->name);
  socket->name = new_name;
}

void socket_set_model(Socket *socket, Model *model)
{
  socket->model = model;
}

Model *tlm2c_socket_get_model(Socket *socket)
{
  return socket->model;
}

Socket *socket_insert_head(Socket *element, Socket *head)
{
  element->next = head;
  return element;
}

Socket *tlm2c_socket_get_by_name(const char *name)
{
  Socket *current;

  tlm2c_spinlock_lock(&socket_list_lock);
  current = socket_list_head;
  while (current != NULL)
  {
    if (strcmp(current->name, name) == 0)
      break;
    current = current->next_list;
  }

  tlm2c_spinlock_unlock(&socket_list_lock);
  return current;
}

const char *tlm2c_socket_get_name(Socket *socket)
{
  return socket->name;
}
