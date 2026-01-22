#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct heap {
  void * * heap;
  int size;
};

void heap_create(struct heap * * h, int size) {
  assert(0 < size);
  int real_size = 1;
  while (0 < real_size && real_size < size) {
    real_size <<= 1;
  }
  *h = malloc(sizeof(struct heap));
  (*h)->heap = malloc(sizeof((*h)->heap) * real_size);
  memset((*h)->heap, 0, sizeof((*h)->heap) * real_size);
  (*h)->size = real_size;
}

void heap_insert(struct heap * h, void * element, int (*greater_than)(void *, void *)) {
  assert(NULL != h);
  assert(NULL != element);
  assert(NULL != greater_than);
  int i = 1;
  while (NULL != h->heap[i - 1]) {
    if (greater_than(element, h->heap[i - 1])) {
      i = 2 * i + 1;
    } else {
      i *= 2;
    }
  }
  if (h->size >= i) {
    h->heap[i - 1] = element;
  } else {
    fprintf(stderr, "not enough space\n");
  }
}

void heap_traverse(struct heap * h, void (*print)(void *)) {
  assert(NULL != h);
  assert(NULL != print);
  for (int i = 0; h->size > i; ++i) {
    if (NULL != h->heap[i]) {
      print(h->heap[i]);
    }
  }
}

/* AUX */

struct record {
  const char * name;
  const char * address;
};

int record_greater_than(void * a, void * b) {
  assert(NULL != a);
  assert(NULL != b);
  return 0 < strcmp(((struct record *)a)->name, ((struct record *)b)->name);
}

void record_print(void * p) {
  assert(NULL != p);
  struct record * r = (struct record *)(p);
  assert(NULL != r->name);
  assert(NULL != r->address);
  printf("name: %s, address: %s\n", r->name, r->address);
}

/* IMPL */

int main(int argc, char ** argv) {
  int result = 0;

  struct heap * my_heap = NULL;
  heap_create(&my_heap, /* make sure it is always a power of 2 */ 16);

  {
    struct record * my_record = malloc(sizeof(struct record));
    my_record->address = "São Paulo";
    my_record->name = "Daniel Prado";
    heap_insert(my_heap, my_record, record_greater_than);
  }

  {
    struct record * my_record = malloc(sizeof(struct record));
    my_record->address = "São José do Rio Preto";
    my_record->name = "Augusto Mendes";
    heap_insert(my_heap, my_record, record_greater_than);
  }

  {
    struct record * my_record = malloc(sizeof(struct record));
    my_record->address = "Itapira";
    my_record->name = "Leonel Silva";
    heap_insert(my_heap, my_record, record_greater_than);
  }

  heap_traverse(my_heap, record_print);
  return result;
}
