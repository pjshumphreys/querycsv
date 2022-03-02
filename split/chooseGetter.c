void (*chooseGetter(int encoding))(FILE *, long *, int *, int *) {
  switch(encoding) {
    case ENC_UNKNOWN:
    case ENC_UTF8:
      return &getCodepointsUTF8;

    case ENC_CP437:
      return &getCodepointsCP437;

    case ENC_CP850:
      return &getCodepointsCP850;

    case ENC_CP1047:
      return &getCodepointsCP1047;

    case ENC_CP1252:
      return &getCodepointsCP1252;

    case ENC_UTF16LE:
      return &getCodepointsUtf16Le;

    case ENC_UTF16BE:
      return &getCodepointsUtf16Be;
    /*

    case ENC_UTF32LE:
      return &getCodepointsUTF32LE;

    case ENC_UTF32BE:
      return &getCodepointsUTF32BE;
    */

    case ENC_PETSCII:
      return &getCodepointsPetscii;

    case ENC_MAC:
      return &getCodepointsMac;

    case ENC_ATARIST:
      return &getCodepointsAtariST;

    case ENC_BBC:
      return &getCodepointsBBC;

    case ENC_ZX:
      return &getCodepointsZX;

    case ENC_TSW:
      return &getCodepointsTSW;

    case ENC_ASCII:
      return &getCodepointsAscii;
  }

  /* if all else fails, just try using
    UTF-8 (It's probably that anyway) */
  return &getCodepointsUTF8;
}
