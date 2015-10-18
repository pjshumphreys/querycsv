/*
  dos/windows implementation of dirent.h functions using those available in watcom.

  all filenames returned are in UTF-8 encoding


  ino_t  d_ino       //file serial number
  char   d_name[]    //name of entry




  struct _wfinddatai64_t {
    unsigned  attrib;
    time_t    time_create; //-1 for FAT file systems
    time_t    time_access; //-1 for FAT file systems
    time_t    time_write;
    __int64   size; //64-bit size info
    wchar_t   name[_MAX_PATH];
  };  
*/


DIR *opendir(const char *name) {
  DIR * retval = NULL;
  wchar_t* wide;
  int len;
  struct _wfinddatai64_t temp2;
  
  if((retval = (DIR *)malloc(sizeof(DIR)) == NULL) {
    return NULL;
  }

  len = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
  
  if((wide = (wchar_t*)malloc(sizeof(wchar_t)*len)) == NULL) {
    free(retval);
    return NULL;
  }
  
  MultiByteToWideChar(CP_UTF8, 0, name, -1, wide, len);
  
  retval->d_handle = _wfindfirsti64(name, &temp2);

  free(wide);
  
  if(retval->d_handle != -1) {
    WideCharToMultiByte(CP_UTF8, 0, temp2.name, -1, retval->d_name, MAX_UTF8_PATH, NULL, NULL);

    retval->d_type  = temp2.attrib;
    retval->d_ctime = temp2.time_create;
    retval->d_atime = temp2.time_access;
    retval->d_mtime = temp2.time_write;
    retval->d_size  = temp2.size;
    retval->d_first = TRUE; //first file
  }

  return retval;
}

struct dirent *readdir(DIR * inval) {
  int rc;
  struct _wfinddatai64_t temp2;

  if(inval->d_handle == -1) {
    return NULL;
  }

  if(inval->d_first == TRUE) {
    inval->d_first = FALSE;
    return inval;
  }

  if(_wfindnexti64(inval->d_handle, &temp2) == -1) {
    return NULL;
  }
  
  WideCharToMultiByte(CP_UTF8, 0, temp2.name, -1, inval->d_name, MAX_UTF8_PATH, NULL, NULL);

  inval->d_type  = temp2.attrib;
  inval->d_ctime = temp2.time_create;
  inval->d_atime = temp2.time_access;
  inval->d_mtime = temp2.time_write;
  inval->d_size  = temp2.size;

  return inval;
}

int closedir(DIR * inval) {
  _findclose(inval->d_handle);
  free(inval);
  return 0;
}
