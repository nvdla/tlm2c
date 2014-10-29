/*
 * test.c
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

/*
 * This is a test program to be sure everything is working properly.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tlm2c/tlm2c.h"
#include "initiator.h"
#include "target.h"

int main(int argc, char **argv)
{
  int ret = EXIT_FAILURE;

  Initiator *initiator = create_example_initiator();
  Target *target = create_example_target();

  tlm2c_bind(initiator->master_socket, target->slave_socket);

  model_end_of_elaboration((Model *)initiator);
  model_end_of_elaboration((Model *)target);
  notify(initiator);

  if (!initiator->test_fail)
  {
    printf("Test succeed!\n");
    ret = EXIT_SUCCESS;
  }
  else
  {
    printf("Test failed!\n");
  }

  destroy_initiator(initiator);
  destroy_target(target);

  return ret;
}
