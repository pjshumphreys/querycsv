#define _XOPEN_SOURCE 500
#include "querycsv.h"

struct fileEntry {
  struct fileEntry *left;
  struct fileEntry *right;
  struct fileEntry *parent;
  char *name;
  int isFolder;
  time_t modified;
  off_t size;
};

/* get file statistics for a given filename and parent folder path.
We also duplicate the file name as the copy in the dirent structure will get overwritten when calling this fuunction for the next file found in the folder */
int statdir(
    char *dir,
    struct dirent *direp,
    char **name,
    struct stat *buf
  ) {

  char* temp;
  char* temp2;
  int retval;
  int nameLen = strlen(direp->d_name) + 1;

  temp = (char*)malloc(nameLen);

  if(temp == NULL) {
    *name = NULL;
    return -1;
  }

  sprintf(temp, "%s", direp->d_name);

  temp2 = (char*)malloc(strlen(dir)+nameLen);

  if(temp2 == NULL) {
    free(temp);
    *name = NULL;
    return -1;
  }

  sprintf(temp2, "%s/%s", dir, temp);

  retval = stat(temp2, buf);
  free(temp2);

  if(retval != 0) {
    free(temp);
    *name = NULL;
    return -1;
  }

  *name = temp;

  return retval;
}

/*insert the name of a file or folder into our binary tree structure.
This process will effectively sort the directory entries */
void insert(
    struct fileEntry ** root,
    char * name,
    int isFolder,
    time_t modified,
    off_t size,
    int sortBy,
    int sortDescending
  ) {

  int comparison;
  struct fileEntry * currentNode;
  struct fileEntry * newNode;

  if(root == NULL) {
    //Huh? no memory location was specified to hold the tree?
    //Just exit and let the author of the calling function figure out their mistake
    return;
  }

  if((newNode = malloc(sizeof(struct fileEntry))) == NULL) {
    return;
  }

  newNode->parent = NULL;
  newNode->left = NULL;
  newNode->right = NULL;
  newNode->name = name;
  newNode->isFolder = isFolder,
  newNode->modified = modified,
  newNode->size = size;

  if(*root == NULL) {
    //No entries have been inserted at all.
    //Just insert the data into a new node
    *root = newNode;
    return;
  }
  else {
    //navigate down the tree, and insert the new data into the correct place within it

    //start at the top
    currentNode = *root;

    for( ; ; ) {
      comparison = 0;

      if(currentNode->isFolder && !isFolder) {
        comparison = sortDescending?-1:1;
      }
      else if(!(currentNode->isFolder) && isFolder) {
        comparison = sortDescending?1:-1;
      }
      else if(comparison == 0) {
        switch(sortBy) {
          case 2: //size
            if(currentNode->size != size) {
              if(currentNode->size > size) {
                comparison = -1;
              }
              else {
                comparison = 1;
              }
            }
          break;

          case 3: //date
            if(currentNode->modified != modified) {
              if(currentNode->modified > modified) {
                comparison = -1;
              }
              else {
                comparison = 1;
              }
            }
          break;


        }

        if(comparison == 0) {
          //name
          comparison = strCompare(
            (unsigned char **)&(currentNode->name),
            (unsigned char **)&(name),
            10,
            (void (*)(void))&getUnicodeChar,
            (void (*)(void))&getUnicodeChar
          );
        }
      }

      if(sortDescending) {
        comparison = -comparison;
      }

      if(comparison < 1) {
        if(currentNode->left == NULL) {
          newNode->parent = currentNode;
          currentNode->left = newNode;
          return;
        }
        else {
          currentNode = currentNode->left;
        }
      }
      else {
        if(currentNode->right == NULL) {
          newNode->parent = currentNode;
          currentNode->right = newNode;
          return;
        }
        else {
          currentNode = currentNode->right;
        }
      }
    }
  }
}

//in order traverse the binary tree we have created,
//calling the callback and freeing used memory as we go
void walkAndCleanup(
    struct fileEntry **root,
    int callback
  ) {

  struct fileEntry *currentNode;
  struct fileEntry *parentNode;

  void (*addListEntryPtr)(char *, int, time_t, off_t) = (void (*)(char *, int, time_t, off_t))callback;

  if(root == NULL || *root == NULL) {
    return;
  }

  currentNode = *root;

  for( ; ; ) {
    if(currentNode->left) {
      currentNode = currentNode->left;
      continue;
    }

    if((currentNode->name) != NULL) {
      addListEntryPtr(currentNode->name, currentNode->isFolder, currentNode->modified, currentNode->size);
      freeAndZero(currentNode->name);
    }

    if(currentNode->right) {
      currentNode = currentNode->right;
      continue;
    }

    parentNode = currentNode->parent;

    if(parentNode != NULL) {
      if(parentNode->left == currentNode) {
        currentNode = parentNode;
        free(currentNode->left);
        currentNode->left = NULL;
      }
      else {
        currentNode = parentNode;
        free(currentNode->right);
        currentNode->right = NULL;
      }
    }
    else {
      free(currentNode);
      *root = NULL;
      return;
    }
  }
}

//calls the specified callback for every file or sub folder in the specified folder.
//the folder entries are returned in a sorted order that can be specified by parameters
//the function returns true if the folder could sucessfully be read from, otherwise it returns false
int getEntries(
    char* path,
    int sortBy,
    int sortDescending,
    int callback
  ) {

  DIR *dirp;
  struct dirent *direntp;
  struct stat buf;
  struct fileEntry *root = NULL;
  int success = FALSE;
  char * nameCopy;

  dirp = opendir(path);

  if(dirp) {
    errno = 0;
    while( (direntp = readdir(dirp)) != NULL ) {
      success = TRUE;
      nameCopy = NULL;

      if(
          strncmp(".", direntp->d_name, 2) == 0 ||
          strncmp("..", direntp->d_name, 3) == 0
        ) {
        continue;
      }

      if(statdir(path, direntp, &nameCopy, &buf) == 0) {
        if(S_ISDIR(buf.st_mode)) {
          insert(&root, nameCopy, 1, buf.st_mtime, 0, sortBy, sortDescending);
        }
        else if(S_ISREG(buf.st_mode)) {
          insert(&root, nameCopy, 0, buf.st_mtime, buf.st_size, sortBy, sortDescending);
        }
        else {
          freeAndZero(nameCopy);
        }
      }
    }

    closedir(dirp);
  }

  walkAndCleanup(&root, callback);

  return success;
}

/* special function that creates a file if and only if it does not already exist. Otherwise it returns errno */
int addFile(char *path) {
  int fd;
  errno = 0;

  if((fd = open(path, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR)) < 0) {
    return errno;
  }
  else {
    close(fd);
  }

  return 0;
}

int folderExists(char* folderName) {
  struct stat sb;

  return stat(folderName, &sb) == 0 && S_ISDIR(sb.st_mode);
}

int unlink_cb(
    const char *fpath,
    const struct stat *sb,
    int typeflag,
    struct FTW *ftwbuf
  ) {

  int rv = (S_ISDIR(sb->st_mode))?rmdir(fpath):unlink(fpath);

  if (rv) {
    perror(fpath);
  }

  return rv;
}

int rmrf(char *path) {
  return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

jmp_buf env;
int exitval;

/*the real entry point for the html5 version of querycsv. we can't use main as it's treated specially on emscripten and so can only be called once (on page load which we don't want) */
int realmain(int argc, char **argv);
/*
int realmain(int argc, char **argv) {
  int i;

  printf("Hello world!\nProgram arguments: %d\n", argc);

  for(i = 0; i < argc; i++) {
    printf("%d: %s\n", i, argv[i]);
  }

  //I believe this type coercion is valid in emscripten.
  //It should always print 0
  printf("%d\n", (int)argv[argc]);

  return 0;
}
//*/

void exit_emcc(int status) {
  exitval = status;

  longjmp(env, 1);
}

int wrapmain(char *path, char* filename) {
  static char *argv2[3];
  int jumpCode = 0;

  retval = 0;

  chdir(path);

  argv2[0] = "querycsv";
  argv2[1] = filename;
  argv2[2] = NULL;

  jumpCode = setjmp(env);

  if(jumpCode == 0) {
    exitval = realmain(2, (char**)argv2);
  }

  return exitval;
}
