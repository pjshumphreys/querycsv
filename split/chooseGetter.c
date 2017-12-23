void (*chooseGetter(int encoding))(FILE *, long *, int *, int *) {
  switch(encoding) {
    case ENC_UNKNOWN:
    case ENC_UTF8: {
      return &getCodepointsUTF8;
    } break;

    case ENC_CP437: {
      return &getCodepointsCP437;
    } break;

    case ENC_CP850: {
      return &getCodepointsCP850;
    } break;

    case ENC_CP1047: {
      return &getCodepointsCP1047;
    } break;

    case ENC_CP1252: {
      return &getCodepointsCP1252;
    } break;

    /*
    case ENC_UTF16LE: {
      return &getCodepointsUTF16LE;
    } break;

    case ENC_UTF16BE: {
      return &getCodepointsUTF16BE;
    } break;

    case ENC_UTF32LE: {
      return &getCodepointsUTF32LE;
    } break;

    case ENC_UTF32BE: {
      return &getCodepointsUTF32BE;
    } break;
    */

    case ENC_PETSCII: {
      return &getCodepointsPetscii;
    } break;

    case ENC_MAC: {
      return &getCodepointsMac;
    } break;

    case ENC_ATARIST: {
      return &getCodepointsAtariST;
    } break;
  }

  /* if all else fails, just try using
    UTF-8 (It's probably that anyway) */
  return &getCodepointsUTF8;
}
