/*
 * Copyright 2016, gRPC Authors All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "internal.h"
#include "cgrpc.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cgrpc_handler *cgrpc_handler_create_with_server(cgrpc_server *server) {
  cgrpc_handler *handler = (cgrpc_handler *) malloc(sizeof (cgrpc_handler));
  memset(handler, 0, sizeof(cgrpc_handler));
  handler->server = server;
  grpc_metadata_array_init(&(handler->request_metadata_recv));
  grpc_call_details_init(&(handler->call_details));
  handler->completion_queue = grpc_completion_queue_create(NULL, NULL, NULL);
  return handler;
}

void cgrpc_handler_destroy(cgrpc_handler *h) {
  grpc_completion_queue_shutdown(h->completion_queue);
  cgrpc_completion_queue_drain(h->completion_queue);
  grpc_completion_queue_destroy(h->completion_queue);
  grpc_metadata_array_destroy(&(h->request_metadata_recv));
  grpc_call_details_destroy(&(h->call_details));
  if (h->server_call) {
    //grpc_call_destroy(h->server_call);
  }
  free(h);
}

const char *cgrpc_handler_host(cgrpc_handler *h) {
  return (const char *) GRPC_SLICE_START_PTR(h->call_details.host);
}

const char *cgrpc_handler_method(cgrpc_handler *h) {
  return (const char *) GRPC_SLICE_START_PTR(h->call_details.method);
}

const char *cgrpc_handler_call_peer(cgrpc_handler *h) {
  return grpc_call_get_peer(h->server_call);
}

cgrpc_call *cgrpc_handler_get_call(cgrpc_handler *h) {
  cgrpc_call *call = (cgrpc_call *) malloc(sizeof(cgrpc_call));
  memset(call, 0, sizeof(cgrpc_call));
  call->call = h->server_call;
  return call;
}

cgrpc_completion_queue *cgrpc_handler_get_completion_queue(cgrpc_handler *h) {
  return h->completion_queue;
}

grpc_call_error cgrpc_handler_request_call(cgrpc_handler *h,
                                           cgrpc_metadata_array *metadata,
                                           long tag) {
  return grpc_server_request_call(h->server->server,
                                  &(h->server_call),
                                  &(h->call_details),
                                  metadata,
                                  h->completion_queue,
                                  h->server->completion_queue,
                                  cgrpc_create_tag(tag));
}


