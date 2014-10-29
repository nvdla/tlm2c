/*
 * initiator.c
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

#include "initiator.h"

Initiator *create_example_initiator(void)
{
  Initiator *initiator = malloc(sizeof(Initiator));

  model_init(&initiator->model, "Example_initiator");
  initiator->master_socket = socket_initiator_create("master_socket");
  model_add_socket(&initiator->model, (Socket *)initiator->master_socket);

  return initiator;
}

void destroy_initiator(Initiator *initiator)
{
  model_destroy(&initiator->model);
  free(initiator);
}

void notify(Initiator *initiator)
{
  GenericPayload *p = payload_create();

  /*
   * Just model a WRITE to 0x00000000 which should be accepted.
   */
  printf("Initiator writes 0xDEADBEEF @0x00000000\n");
  payload_set_address(p, 0x00000000);
  payload_set_value(p, 0xDEADBEEF);
  payload_set_size(p, 4);
  payload_set_command(p, WRITE);
  b_transport(initiator->master_socket, (Payload *)p);

  switch (payload_get_response_status(p))
  {
    case OK_RESPONSE:
      printf("Response status: OK\n");
    break;
    case ADDRESS_ERROR_RESPONSE:
      printf("Response status: ADDRESS ERROR\n");
      initiator->test_fail = 1;
    break;
    default:
      printf("Unknown response..\n");
      initiator->test_fail = 1;
    break;
  }

  /*
   * Read the value @0x00000000 which should be 0xDEADBEEF.
   */
  printf("Initiator reads @0x00000000\n");
  payload_set_value(p, 0);
  payload_set_command(p, READ);
  b_transport(initiator->master_socket, (Payload *)p);

  switch (payload_get_response_status(p))
  {
    case OK_RESPONSE:
      printf("Response status: OK\n");
    break;
    case ADDRESS_ERROR_RESPONSE:
      printf("Response status: ADDRESS ERROR\n");
      initiator->test_fail = 1;
    break;
    default:
      printf("Unknown response..\n");
      initiator->test_fail = 1;
    break;
  }
  printf("Value read by the initiator: 0x%8.8X\n",
         (unsigned int)payload_get_value(p));
  if (payload_get_value(p) != 0xDEADBEEF)
  {
    printf("Wrong value..\n");
    initiator->test_fail = 1;
  }

  /*
   * Read the value @0x00000001 which should be 0xAA55AA55.
   */
  printf("Initiator reads @0x00000001\n");
  payload_set_address(p, 0x00000001);
  payload_set_value(p, 0);
  payload_set_command(p, READ);
  b_transport(initiator->master_socket, (Payload *)p);

  switch (payload_get_response_status(p))
  {
    case OK_RESPONSE:
      printf("Response status: OK\n");
    break;
    case ADDRESS_ERROR_RESPONSE:
      printf("Response status: ADDRESS ERROR\n");
      initiator->test_fail = 1;
    break;
    default:
      printf("Unknown response..\n");
      initiator->test_fail = 1;
    break;
  }
  printf("Value read by the initiator: 0x%8.8X\n",
         (unsigned int)payload_get_value(p));
  if (payload_get_value(p) != 0xAA55AA55)
  {
    printf("Wrong value..\n");
    initiator->test_fail = 1;
  }

  /*
   * Try to access address outside range.
   */
  printf("Initiator reads @0x00001000\n");
  payload_set_address(p, 0x1000);
  b_transport(initiator->master_socket, (Payload *)p);

  switch (payload_get_response_status(p))
  {
    case OK_RESPONSE:
      printf("Response status: OK\n");
      initiator->test_fail = 1;
    break;
    case ADDRESS_ERROR_RESPONSE:
      printf("Response status: ADDRESS ERROR\n");
    break;
    default:
      printf("Unknown response..\n");
      initiator->test_fail = 1;
    break;
  }

  payload_destroy(p);
}

