// Represents a node in a doubly-linked list
struct dllnode {
  char *data;
  struct dllnode *next;
  struct dllnode *prev;
};

// node_create(str) creates a node. Caller must free the returned node.
// effect: allocates heap memory
// requires: str must be a valid C string
// time: O(n), where n is the length of str
static struct dllnode *node_create(const char *str) {
  assert(str);
  struct dllnode *node = malloc(sizeof(struct dllnode));
  assert(node);
  node->data = malloc(sizeof(char) * (strlen(str) + 1));
  assert(node->data);
  strcpy(node->data, str);
  node->next = NULL;
  node->prev = NULL;
  return node;
}

// node_destroy(noden) frees all resources used by node; node becomes invalid
//   after this call.
// effects: frees heap memory
//          invalidates node
// time: O(1)
static void node_destroy(struct dllnode *node) {
  assert(node);
  free(node->data);
  free(node);
}

// === LIST STRUCT AND FUNCTIONS =============================================

struct dllist {
  struct dllnode *first;
  struct dllnode *last;
  int length;
};

struct dllist *list_create() {
  struct dllist *new_list = malloc(sizeof(struct dllist));
  assert(new_list);
  new_list->first = NULL;
  new_list->last = NULL;
  new_list->length = 1;
  return new_list;
}

void list_destroy(struct dllist *list) {
  assert(list);
  struct dllnode *p = list->first;
  while (p) {
    struct dllnode *temp= p;
    p = p->next;
    node_destroy(temp);
  }
  free(list);
}

void list_append(struct dllist *list, const char *str) {
  assert(list);
  assert(str);
  struct dllnode *cur = list->first;
  if (cur == NULL) {
    list->first = node_create(str);
    list->last = list->first;
  } else /* (if cur != NULL) */ {
    struct dllnode *append_node = node_create(str);
    assert(append_node);
    append_node->prev = list->last;
    list->last->next = append_node;
    append_node->next = NULL;
    list->last = append_node;
    ++list->length;
  }
}

void list_prepend(struct dllist *list, const char *str) {
  assert(list);
  assert(str);
  struct dllnode *cur = list->first;
  if (cur == NULL) {
    list->first = node_create(str);
    list->last = list->first;
  } else /* (if cur != NULL) */ {
    struct dllnode *prepend_node = node_create(str);
    assert(prepend_node);
    list->first->prev = prepend_node;
    prepend_node->next = list->first;
    prepend_node->prev = NULL;
    list->first = prepend_node;
    ++list->length;
  }
}

void list_insert_at(struct dllist *list, const char *str, int idx) {
  assert(list);
  assert(idx >= 0 && idx <= list->length);
  if (idx == list->length) {
    list_append(list, str);
    return;
  } 
  if (idx == 0) {
    list_prepend(list, str);
    return;
  }
  struct dllnode *p = list->first;
  int i = 1;
  while (i < idx) {
    p = p->next;
    ++i;
  }
  struct dllnode *insert_node = node_create(str);
  insert_node->next = p->next;
  p->next->prev = insert_node;
  insert_node->prev = p;
  p->next = insert_node;
  ++list->length;
}

char *list_remove_at(struct dllist *list, int idx) {
  assert(list);
  assert(idx >= 0 && idx < list->length);
  assert(list_is_empty(list) == false);
  if (idx == list->length - 1) {
    return list_remove_back(list);
  } 
  if (idx == 0) {
    return list_remove_front(list);
  }
  struct dllnode *p = list->first;
  int i = 0;
  while (p && i != idx) {
    p = p->next;
    ++i;
  }
  char *retval = p->data;
  p->next->prev = p->prev;
  p->prev->next = p->next;
  free(p);
  --list->length;
  return retval;
}

char *list_remove_front(struct dllist *list) {
  assert(list);
  assert(list_is_empty(list) == false);
  char *retval = 0;
  struct dllnode *p = list->first;
  retval = p->data;
  list->first = p->next;
  p->next->prev = NULL;
  free(p);
  --list->length;
  return retval;
}

char *list_remove_back(struct dllist *list) {
  assert(list);
  assert(list_is_empty(list) == false);
  char *retval = 0;
  struct dllnode *p = list->last;
  retval = p->data;
  list->last = p->prev;
  p->prev->next = NULL;
  free(p);
  --list->length;
  return retval;
}

const char *list_peek_front(const struct dllist *list) {
  assert(list);
  assert(list_is_empty(list) == false);
  const char *first_item = list->first->data;
  return first_item;
}

const char *list_peek_back(const struct dllist *list) {
  assert(list);
  assert(list_is_empty(list) == false);
  const char *last_item = list->last->data;
  return last_item;
}

bool list_is_empty(const struct dllist *list) {
  assert(list);
  bool num = (list->first == NULL) + (list->last == NULL);
  return num;
}

void list_print(const struct dllist *list, int direction) {
  assert(list);
  if (direction == ITER_FRONT_TO_BACK) {
    printf("[FRONT]:");
    struct dllnode *p = list->first;
    while (p) {
      printf(" %s ->", p->data);
      p = p->next;
    } 
    printf(" [BACK]\n");
  } else {
    printf("[BACK]:");
    struct dllnode *p = list->last;
    while (p) {
      printf(" %s ->", p->data);
      p = p->prev;
    } 
    printf(" [FRONT]\n");
  }
}

// === ITERATOR STRUCT, CONSTS, AND FUNCTIONS ================================

struct iter {
  struct dllnode *cur;
  int dir;
};

const int ITER_FRONT_TO_BACK = 0;
const int ITER_BACK_TO_FRONT = 1;

struct iter *iter_get(const struct dllist *list, int dir) {
  assert(list);
  assert(list_is_empty(list) == false);
  assert(dir == ITER_FRONT_TO_BACK || dir == ITER_BACK_TO_FRONT);
  struct iter *iter_new;
  iter_new = malloc(sizeof(struct iter));
  if (dir == ITER_FRONT_TO_BACK) {
    iter_new->cur = list->first;
    iter_new->dir = ITER_FRONT_TO_BACK;
  }
  else {
    iter_new->cur = list->last;
    iter_new->dir = ITER_BACK_TO_FRONT;
  }
  return iter_new;
}

const char *iter_current(const struct iter *e) {
  assert(e);
  const char *retval  = e->cur->data;
  return retval;
}

bool iter_next(struct iter *e) {
  assert(e);
  assert(e->cur);
  if (e->dir == ITER_FRONT_TO_BACK) {
    if (e->cur->next == NULL) {
      free(e);
      return false;
    } else {
      e->cur = e->cur->next;
    }
  } else {
    if (e->cur->prev == NULL) {
      free(e);
      return false;
    } else {
      e->cur = e->cur->prev;
    }
  }   
  return true;
}