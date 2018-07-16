void cleanup_query(struct qryData *query) {
  MAC_YIELD

  cleanup_columnReferences(query->columnReferenceHashTable);
  cleanup_resultColumns(query->firstResultColumn);
  cleanup_orderByClause(query->groupByClause);
  cleanup_orderByClause(query->orderByClause);
  cleanup_expression(query->joinsAndWhereClause);
  cleanup_inputTables(query->firstInputTable);

  if(query->dateString) {
    free(query->dateString);
  }

  if(query->outputFileName) {
    fclose(query->outputFile);

    #ifdef MPW_C
      /* Set the fourcc type and creator codes for macos.
         the function "fsetfileinfo" can be used to change
         the creator and type code for a file.
      */
      switch(query->outputEncoding) {
        case ENC_UTF16LE:
        case ENC_UTF16BE:
        case ENC_UTF32LE:
        case ENC_UTF32BE: {
          /*
          * SUE (the carbon version) is the only editor I know
          * that can read unicode text on macos 8.6 in practice.
          */
          fsetfileinfo_absolute(query->outputFileName, 'SUE ', 'utxt');
        } break;

        default: {
          fsetfileinfo_absolute(query->outputFileName, 'SUE ', 'TEXT');
        }
      }
    #endif

    free(query->outputFileName);
  }
}
