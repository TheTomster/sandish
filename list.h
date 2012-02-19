#pragma once

typedef struct list * list_handle;

list_handle list_new(void);

void list_delete(list_handle l);

void list_push(list_handle l, void * item);

void * list_get(list_handle l, unsigned int index);

unsigned int list_size(list_handle l);
