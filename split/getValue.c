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

  MAC_YIELD

  expressionPtr->isNull = FALSE;

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
        stringGet((unsigned char **)(&(expressionPtr->value)), field, match->params);
      }
    } break;

    case EXP_LITERAL: {
      expressionPtr->value = mystrdup((char *)expressionPtr->unionPtrs.voidPtr);
    } break;

    case EXP_CALCULATED: {
      calculatedField = ((struct expression*)(expressionPtr->unionPtrs.voidPtr));

      getValue(calculatedField, match);

      expressionPtr->isNull = calculatedField->isNull;
      expressionPtr->value = mystrdup(calculatedField->value);

      freeAndZero(calculatedField->value);
    } break;

    case EXP_GROUP: {
      column = (struct resultColumn *)(expressionPtr->unionPtrs.voidPtr);
      if(column->groupingDone) {
        field = &(match->ptr[column->resultColumnIndex]);

        if(field->isNull == FALSE) {
          stringGet((unsigned char **)(&(expressionPtr->value)), field, match->params);
          break;
        }
      }
      else if(column->groupText != NULL) {
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

    default: {
      if(expressionPtr->type > EXP_CONCAT) {
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
          expressionPtr->unionPtrs.leaves.rightPtr->isNull
        ) {
        expressionPtr->isNull = TRUE;
        expressionPtr->value = mystrdup("");
      }
      else {
        switch(expressionPtr->type){
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
