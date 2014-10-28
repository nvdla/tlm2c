/*
 * model.c
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

#include "tlm2c/model.h"
#include <stdlib.h>
#include <string.h>

/*
 * Head of the model list.
 */
Model *head = NULL;

/*
 * Private function from method.c
 */
Method *method_insert_head(Method *element, Method *head);
void method_set_model(Method *method, Model *model);

/*
 * Private functions from socket.c
 */
void socket_prefix_name(Socket *socket, char *prefix);
Socket *socket_insert_head(Socket *element, Socket *head);
void socket_set_model(Socket *socket, Model *model);

void default_end_of_elaboration(Model *model);

void model_init(Model *model, const char *name)
{
  memset(model, 0, sizeof(Model));
  model->name = strdup(name);
  model->end_of_elaboration = default_end_of_elaboration;

  model->next = head;
  head = model;
}

void model_add_method(Model *model, Method *method)
{
  model->methods = method_insert_head(method, model->methods);
  method_set_model(method, model);
}

void model_destroy(Model *model)
{
  Model *current, *prev;
  current = head;
  prev = NULL;

  while (current != model)
  {
    prev = current;
    current = current->next;
  }

  if (prev)
  {
    prev->next = current->next;
  }

  method_destroy(model->methods);
  socket_destroy_list(model->sockets);

  free(model->name);
}

void model_register_end_of_elaboration(Model *model, eoe_cb cb)
{
  model->end_of_elaboration = cb;
}

void default_end_of_elaboration(Model *model)
{
  /* Nothing to do. */
}

void model_add_socket(Model *model, Socket *socket)
{
  model->sockets = socket_insert_head(socket, model->sockets);
  socket_prefix_name(socket, model->name);
  socket_set_model(socket, model);
}

void model_end_of_elaboration(Model *model)
{
  model->end_of_elaboration(model);
}

void model_notify(Model *model)
{
  method_notification(model->methods);
}

Model *get_models(void)
{
  return head;
}
