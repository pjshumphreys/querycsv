struct resultTree *tree_single(struct resultTree *oldroot, int dir) {
  struct resultTree *newroot = oldroot->link[!dir];

  oldroot->link[!dir] = newroot->link[dir];
  newroot->link[dir] = oldroot;

  oldroot->type = TRE_RED;
  newroot->type = TRE_BLACK;

  return newroot;
}

struct resultTree *tree_double(struct resultTree *root, int dir) {
  root->link[!dir] = tree_single(root->link[!dir], !dir);

  return tree_single(root, dir);
}

/* red black tree insertion code. based upon that found at http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx */
int tree_insert(
    struct qryData *query,
    struct resultColumnValue *columns,
    struct resultTree **root
) {
  struct resultTree *newResult = NULL;

  struct resultTree head; /* False tree root */

  struct resultTree *g, *t;     /* Grandparent & parent */
  struct resultTree *p, *q;     /* Iterator & parent */
  int dir = 0, last = 0, dir2 = 0;

  MAC_YIELD

  if(root == NULL) {
    /* Huh? no memory location was specified to hold the tree? */
    /* Just exit and let the author of the calling function figure out their mistake */
    return FALSE;
  }

  head.type = TRE_BLACK;

  reallocMsg((void**)&newResult, sizeof(struct resultTree));

  newResult->parent = NULL;
  newResult->link[0] = NULL;
  newResult->link[1] = NULL;
  newResult->columns = columns;
  newResult->type = TRE_RED;

  if(*root == NULL) {
    /* No entries have been inserted at all. */
    /* Just insert the data into a new node */
    *root = newResult;
  }
  else {
    /* navigate down the tree, and insert the new data into the correct place within it */

    /* Set up helpers */
    t = &head;
    g = p = NULL;
    q = t->link[1] = *root;

    do {
      if(q == NULL) {
        /* Insert new node at the bottom */
        p->link[dir] = q = newResult;
      }
      else if(isRed(q->link[0]) && isRed(q->link[1])) {
        /* Color flip */
        q->type = TRE_RED;
        q->link[0]->type = TRE_BLACK;
        q->link[1]->type = TRE_BLACK;
      }

      /* Fix red violation */
      if(isRed(q) && isRed(p)) {
        dir2 = t->link[1] == g;

        if(q == p->link[last]) {
          t->link[dir2] = tree_single(g, !last);
        }
        else {
          t->link[dir2] = tree_double(g, !last);
        }
      }

      /* Stop if found */
      if (q->columns == columns) {
        break;
      }

      last = dir;
      dir = recordCompare(columns, q->columns, query) < 1;

      /* Update helpers */
      if(g != NULL) {
        t = g;
      }

      g = p, p = q;
      q = q->link[dir];
    } while(1);

    /* Update root */
    *root = head.link[1];
  }

  /* Make root black */
  (*root)->type = TRE_BLACK;

  return TRUE;
}
