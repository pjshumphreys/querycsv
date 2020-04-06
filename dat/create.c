#define _GNU_SOURCE  /* for asprintf */
#include "../hash2in0.h"

struct treeNode {
  int32_t arrayOffset;
  int32_t state;
  int32_t fileOffset;

  struct treeNode* left;
  struct treeNode* right;
  struct treeNode* parent;
};

struct treeNode* createTree(int32_t lowerBound, int32_t upperBound, struct treeNode* parent) {
  int32_t leftLB, leftUB, rightLB, rightUB, current;
  struct treeNode* retval = malloc(sizeof(struct treeNode));

  leftLB = lowerBound;
  rightUB = upperBound;
  current = ((upperBound-lowerBound)/2)+lowerBound;
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

unsigned char shortVar;

void walkTree(struct treeNode* theTree, int32_t state, FILE *fp) {
  int32_t fileOffset = 0;
  int32_t j;

  struct treeNode* current = theTree;

  for( ; ; ) {
    if(current->state != state) {
      switch(state) {
        case 1: {
          current->fileOffset = fileOffset;
          fileOffset += ((3+(hash2[current->arrayOffset].length))*sizeof(int32_t))+sizeof(unsigned char);
        } break;

        case 2: {
          i = (int32_t)(hash2[current->arrayOffset].codepoint);
          /* printf("%d\n", i); */
          fwrite(&i, sizeof(int32_t), 1, fp);

          if(current->left) {
            i = current->left->fileOffset;
          }
          else {
            i = -1;
          }

          /* printf("%d\n", i); */
          fwrite(&i, sizeof(int32_t), 1, fp);

          if(current->right) {
            i = current->right->fileOffset;
          }
          else {
            i = -1;
          }

          /* printf("%d\n", i); */
          fwrite(&i, sizeof(int32_t), 1, fp);

          shortVar = (unsigned char)(hash2[current->arrayOffset].length);
          /* printf("%d\n", (int)(shortVar)); */
          fwrite(&shortVar, 1, 1, fp);

          for(j = 0; j != shortVar; j++) {
            i = (int32_t)(hash2[current->arrayOffset].codepoints[j]);
            /* printf("%d\n", i); */
            fwrite(&i, sizeof(int32_t), 1, fp);
          }

          /* printf("--------------------------------------\n", i); */


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

int main(int argc, char *argv[]) {
  FILE *fp;
  struct treeNode* theTree = NULL;
  char * command = NULL;
  int j = 0;

  fp = fopen("hash2dat.h", "wb");

  for(j = 0; hash2[j].codepoint < 0x100; j++) {
    fprintf(fp, j == 0 ? "static const int\n  hash2_%d[] = {" : ",\n  hash2_%d[] = {", j+1);

    for(int k = 0; k < hash2[j].length; k++) {
      fprintf(fp, "0x%04x%s", hash2[j].codepoints[k], k == hash2[j].length-1 ? ", 0}" : ", ");
    }
  }

  fprintf(fp, ";\n\nstatic const int * hash2_[96] = {");

  j = 0;

  for(int i = 160; i < 256; i++) {
    if(hash2[j].codepoint == i) {
      fprintf(fp, i == 160 ? "\n  { hash2_%-2d }  /* 0x%02x */" : ",\n  { hash2_%-2d }  /* 0x%02x */", j+1, i);
      j++;
    }
    else {
      fprintf(fp, i == 160 ? "\n  { NULL     }  /* 0x%02x */" : ",\n  { NULL     }  /* 0x%02x */", i);
    }
  }

  fprintf(fp, "\n};");

  fclose(fp);


  /* find the offset of the first codepoint32_t past U+00FF */
  for(j = 0; hash2[j].codepoint < 0x100; j++) {

  }

  theTree = createTree(j, HASH2SIZE-1, NULL);

  /* populate the tree with 'file' offsets */
  walkTree(theTree, 1, NULL);

  /* write out the tree to the file and free it as we go */
  fp = fopen("qrycsv00.ovl", "wb");

  walkTree(theTree, 2, fp);

  fclose(fp);

  freeAndZero(command);

  return 0;
}
