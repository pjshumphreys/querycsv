/* getValue can be passed a scalar expression's abstract syntax */
/* tree and it will evaluate it and turn it into a literal string of text characters. */
/* the evaluated value must be freed later though. */
void getValue(
    struct expression *expressionPtr,
    struct resultColumnParam *match
) {
  struct expression *calculatedField;
  struct resultColumn *column;
  struct resultColumnValue *field;
  double temp1;
  double temp2;
  long temp3;

  MAC_YIELD

  switch(expressionPtr->type) {
    case EXP_COLUMN: {
      /* get the value of the first instance in the result set of */
      /* this input column (it should have just been filled out with a */
      /* value for the current record) */

      field = &(match->ptr[
          ((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->
          firstResultColumn->resultColumnIndex
        ]);

      if(field->isNull) {
        expressionPtr->isNull = TRUE;
        expressionPtr->value = mystrdup("");
      }
      else {
        expressionPtr->isNull = FALSE;
        stringGet((unsigned char **)(&(expressionPtr->value)), field);
      }
    } break;

    case EXP_LITERAL: {
      expressionPtr->value = mystrdup((char *)expressionPtr->unionPtrs.voidPtr);
    } break;

    case EXP_CALCULATED: {
      calculatedField = ((struct expression*)(expressionPtr->unionPtrs.voidPtr));

      getValue(calculatedField, match);

      expressionPtr->isNull = calculatedField->isNull;
      expressionPtr->value = calculatedField->value;

      calculatedField->value = NULL;
    } break;

    case EXP_GROUP: {
      column = (struct resultColumn *)(expressionPtr->unionPtrs.voidPtr);
      if(column->groupingDone) {
        field = &(match->ptr[column->resultColumnIndex]);

        if(field->isNull == FALSE) {
          expressionPtr->isNull = FALSE;
          stringGet((unsigned char **)(&(expressionPtr->value)), field);
          break;
        }
      }
      else if(column->groupText != NULL) {
        expressionPtr->isNull = FALSE;
        expressionPtr->value = mystrdup(column->groupText);
        break;
      }

      expressionPtr->isNull = TRUE;
      expressionPtr->value = mystrdup("");
    } break;

    case EXP_UPLUS: {
      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      if(expressionPtr->unionPtrs.leaves.leftPtr->isNull) {
        expressionPtr->isNull = TRUE;
        expressionPtr->value = mystrdup("");
      }
      else {
        expressionPtr->isNull = FALSE;
        temp1 = strctod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
        ftostr(&(expressionPtr->value), temp1);
      }

      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
    } break;

    case EXP_UMINUS: {
      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      if(expressionPtr->unionPtrs.leaves.leftPtr->isNull) {
        expressionPtr->isNull = TRUE;
        expressionPtr->value = mystrdup("");
      }
      else {
        expressionPtr->isNull = FALSE;
        temp1 = ctof(0);
        temp2 = strctod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);

        if(fcmp(temp2, temp1)) {
          ftostr(&(expressionPtr->value), fneg(temp2));
        }
        else {
          (expressionPtr->value) = mystrdup("0");
        }
      }

      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
    } break;

    case EXP_CASE: {
      getCaseValue(
        expressionPtr,
        match
      );
    } break;

    default: {
      if(expressionPtr->type > EXP_CONCAT) {
        expressionPtr->isNull = FALSE;
        expressionPtr->value = mystrdup("");
        break;
      }

      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      getValue(
          expressionPtr->unionPtrs.leaves.rightPtr,
          match
        );

      if(
          expressionPtr->unionPtrs.leaves.leftPtr->isNull ||
          (expressionPtr->unionPtrs.leaves.rightPtr->isNull &&
          expressionPtr->type != EXP_LIMITS)
        ) {
        expressionPtr->isNull = TRUE;
        expressionPtr->value = mystrdup("");
      }
      else {
        expressionPtr->isNull = FALSE;

        switch(expressionPtr->type) {
          case EXP_LIMITS:
            expressionPtr->value = NULL;
            /* don't cleanup the two parameters just yet as the values will be used later in the EXP_SLICE case */
          return;

          case EXP_SLICE:
            calculatedField = expressionPtr->unionPtrs.leaves.rightPtr;
            temp3 = atol(calculatedField->unionPtrs.leaves.rightPtr->value);

            if(temp3 == 0 && (!(calculatedField->unionPtrs.leaves.rightPtr->isNull))) {
              expressionPtr->value = mystrdup("");
            }
            else {
              /* perform the substring extraction */
              strSlice(
                &(expressionPtr->value),
                expressionPtr->unionPtrs.leaves.leftPtr->value,
                (int)atol(calculatedField->unionPtrs.leaves.leftPtr->value),
                (int)temp3
              );
            }

            /* free up the child values from the EXP_LIMITS case now */
            freeAndZero(calculatedField->unionPtrs.leaves.leftPtr->value);
            freeAndZero(calculatedField->unionPtrs.leaves.rightPtr->value);
          break;

          case EXP_PLUS:
            temp1 = strctod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
            temp2 = fadd(temp1, strctod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
            ftostr(&(expressionPtr->value), temp2);
          break;

          case EXP_MINUS:
            temp1 = strctod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
            temp2 = fsub(temp1, strctod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
            ftostr(&(expressionPtr->value), temp2);
          break;

          case EXP_MULTIPLY:
            temp1 = strctod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
            temp2 = fmul(temp1, strctod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
            ftostr(&(expressionPtr->value), temp2);
          break;

          case EXP_DIVIDE:
            temp1 = ctof(0);
            temp2 = strctod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL);

            if(fcmp(temp2, temp1)) {
              ftostr(
                  &(expressionPtr->value),
                  fdiv(strctod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL), temp2)
                );
            }
            else {
              (expressionPtr->value) = mystrdup("Infinity");
            }
          break;

          case EXP_CONCAT:
            d_sprintf(
                &(expressionPtr->value),
                S_STRING S_STRING,
                expressionPtr->unionPtrs.leaves.leftPtr->value,
                expressionPtr->unionPtrs.leaves.rightPtr->value
              );
          break;
        }
      }

      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
      freeAndZero(expressionPtr->unionPtrs.leaves.rightPtr->value);
    } break;
  }
}
