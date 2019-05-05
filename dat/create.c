#include "hash2in0.h"

struct treeNode {
  int arrayOffset;
  int state;
  int fileOffset;

  struct treeNode* left;
  struct treeNode* right;
  struct treeNode* parent;
};

struct treeNode* createTree(int lowerBound, int upperBound, struct treeNode* parent) {
  int leftLB, leftUB, rightLB, rightUB, current;
  struct treeNode* retval = malloc(sizeof(struct treeNode));

  leftLB = lowerBound;
  rightUB = upperBound;
  current = (upperBound-lowerbound)/2)+lowerBound;
  retval->parent = parent;
  retval->arrayOffset = current;
  retval->state = 0;

  leftUB = current-1;
  rightLB = current+1;

  if(leftLB <= leftUB) {
    retval->left = createTree(leftLB, leftUB, retval);
  }
  else {
    retval->left = NULL;
  }

  if(rightLB <= rightUB) {
    retval->right = createTree(rightLB, rightUB, retval);
  }
  else {
    retval->right = NULL;
  }

  return retval;
}

void walkTree(struct treeNode* theTree, int state, FILE *fp) {
  int fileOffset = 0;

  struct treeNode* current = theTree;

  for( ; ; ) {
    if(current->state != state) {
      switch(state) {
        case 1: {
          current->fileOffset = fileOffset;
          fileOffset += ((3+(hash2[current->arrayOffset].length))*sizeof(int))+sizeof(short);
        } break;

        case 2: {
          i = (int)(hash2[current->arrayOffset])

          fwrite(&(i, sizeof(int), 1, fp);
        } break;
      }

      current->state = state;
    }

    if(current->left) {
      if(current->left->state != state) {
        current = current->left;
        continue;
      }
      else if(state == 2) {
        freeAndZero(current->left);
      }
    }

    if(current->right) {
      if(current->right->state != state) {
        current = current->right;
        continue;
      }
      else if(state == 2) {
        freeAndZero(current->right);
      }
    }

    if(current->parent) {
      current = current->parent;

      continue;
    }
    else if(state == 2) {
      free(current);
    }

    return;
  }
}

  /* fwrite(&i, sizeof(int), 1, fp); */

int main(int argc, char *argv[]) {
  FILE *fp;
  struct treeNode* theTree = NULL;

  /* find the offset of the first codepoint past U+00FF */
  for(i = 0; hash2[i].codepoint < 0x100; i++) {}

  theTree = createTree(i, HASH2SIZE-1, NULL);

  /* populate the tree with 'file' offsets */
  walkTree(theTree, 1, NULL);

  /* write out the tree to the file and free it as we go */
  fp = fopen("hash2.dat", "w");

  walkTree(theTree, 2, fp);

  fclose(fp);

  return 0;
}