/*
 * genericPayload.c
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tlm2c/genericPayload.h"

typedef struct InitiatorSocket InitiatorSocket;

struct Payload
{
  char *type;
  InitiatorSocket *source;
};

struct GenericPayload
{
  Payload base;
  uint64_t address;
  uint64_t value;
  uint8_t size;
  int dmi_allowed;
  Command cmd;
  ResponseStatus rep;
};

/*
 * Private functions.
 */
void payload_set_source_initiator(Payload *p, InitiatorSocket *source)
{
  p->source = source;
}

InitiatorSocket *payload_get_source_initiator(Payload *p)
{
  return p->source;
}

/*
 * Public functions.
 */
GenericPayload *payload_create(void)
{
  GenericPayload *p;

  p = (GenericPayload *)malloc(sizeof(GenericPayload));

  /* Initialisation of the new payload. */
  memset(p, 0, sizeof(GenericPayload));
  p->base.type = strdup("GenericPayload");

  return p;
}

void payload_destroy(GenericPayload *p)
{
  free(p->base.type);
  free(p);
}

void payload_set_address(GenericPayload *p, uint64_t address)
{
  p->address = address;
}

uint64_t payload_get_address(GenericPayload *p)
{
  return p->address;
}

void payload_set_value(GenericPayload *p, uint64_t value)
{
  p->value = value;
}

uint64_t payload_get_value(GenericPayload *p)
{
  return p->value;
}

void payload_set_size(GenericPayload *p, uint8_t size)
{
  p->size = size;
}

uint8_t payload_get_size(GenericPayload *p)
{
  return p->size;
}

void payload_allow_dmi(GenericPayload *p, int allowed)
{
  p->dmi_allowed = allowed;
}

int payload_dmi_allowed(GenericPayload *p)
{
  return p->dmi_allowed;
}

void payload_set_command(GenericPayload *p, Command cmd)
{
  p->cmd = cmd;
}

Command payload_get_command(GenericPayload *p)
{
  return p->cmd;
}

void payload_set_response_status(GenericPayload *p, ResponseStatus rep)
{
  p->rep = rep;
}

ResponseStatus payload_get_response_status(GenericPayload *p)
{
  return p->rep;
}

