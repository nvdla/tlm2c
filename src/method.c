/*
 * method.c
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

#include "tlm2c/method.h"
#include "tlm2c/environment.h"
#include <string.h>
#include <stdlib.h>

/*
 * Private.
 */
Method *method_insert_head(Method *element, Method *head);

struct Method
{
  int init;
  uint64_t next_time_ns;
  notify_cb cb;
  Model *model;
  Method *next;
};

Method *method_create(notify_cb cb, int init)
{
  Method *ret;

  ret = (Method *)malloc(sizeof(Method));
  memset(ret, 0, sizeof(Method));

  ret->cb = cb;
  ret->init = init;
  return ret;
}

void method_destroy(Method *method)
{
  Method *next;
  Method *current = method;

  while (current != NULL)
  {
    next = current->next;
    free(current);
    current = next;
  }
}

void method_notify(Method *method, uint64_t time_ns)
{
  method->next_time_ns = env_get_time_ns(get_env()) + time_ns;
  env_request_notify(get_env(), time_ns);
}

void method_notification(Method *method)
{
  Method *current = method;

  while (current != NULL)
  {
    if (env_get_time_ns(get_env()) == current->next_time_ns)
    {
      current->next_time_ns = 0;
      current->cb(current->model);
    }
    current = current->next;
  }
}

/*
 * Private.
 */

Method *method_insert_head(Method *element, Method *head)
{
  element->next = head;
  return element;
}

void method_set_model(Method *method, Model *model)
{
  method->model = model;
}
