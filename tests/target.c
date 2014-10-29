/*
 * target.c
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

#include "target.h"

/*
 * One blocking transport for one target socket.
 */
void bt(void *handle, Payload *p);

/*
 * End of elaboration for this Model.
 */
void end_of_elaboration(Model *model);

Target *create_example_target(void)
{
  Target *target = malloc(sizeof(Target));

  model_init(&target->model, "Example_target");
  target->slave_socket = socket_target_create("slave_socket");
  model_add_socket(&target->model, (Socket *)target->slave_socket);
  socket_target_register_b_transport(target->slave_socket, &target->model, bt);
  model_register_end_of_elaboration((Model *)target, end_of_elaboration);

  return target;
}

void destroy_target(Target *target)
{
  model_destroy(&target->model);
  free(target);
}

void end_of_elaboration(Model *model)
{
  Target *target = (Target *)model;
  size_t i;

  printf("Target end of elaboration.\n");

  for (i = 0; i < 0xFF; i++)
  {
    target->data[i] = 0xAA55AA55;
  }
}

void bt(void *handle, Payload *p)
{
  GenericPayload *gp = (GenericPayload *)p;
  Target *target = (Target *)handle;
  uint64_t address = payload_get_address(gp);
  uint64_t value;

  if (address > 0xFF)
  {
    payload_set_response_status(gp, ADDRESS_ERROR_RESPONSE);
    return;
  }

  switch (payload_get_command(gp))
  {
    case READ:
      printf("Target get a read command @0x%8.8X\n", (unsigned int)address);
      payload_set_value(gp, target->data[address]);
      payload_set_response_status(gp, OK_RESPONSE);
    break;
    case WRITE:
      value = payload_get_value(gp);
      printf("Target get a write command 0x%8.8X @0x%8.8X\n",
             (unsigned int)value, (unsigned int)address);
      target->data[address] = value;
      payload_set_response_status(gp, OK_RESPONSE);
    break;
    default:
      printf("Target doesn't recognize the command.\n");
      payload_set_response_status(gp, ADDRESS_ERROR_RESPONSE);
    break;
  }
}

