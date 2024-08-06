
/* -------------------------------------------------------------------------
 * outputs a JSON array:
 *   "[" LIST "]"
 */
SXML_TYPE_LIST JSON_ARRAY (SXML_TYPE_LIST L)
{
  if (L == NULL) {
    return SXML_T( "[]");
  }
  else {
    return SXML_TLT( "[", L, "]");
  }
}

/* -------------------------------------------------------------------------
 * outputs a JSON map:
 *   "{" LIST "}"
 */
SXML_TYPE_LIST JSON_MAP (SXML_TYPE_LIST L)
{
  if (L == NULL) {
    return SXML_T( "{ }");
  }
  else {
    return SXML_TLT( "{", L, "}");
  }
}

/* -------------------------------------------------------------------------
 * outputs a JSON key/value pair where value must be Quoted:
 *   quoted(K) ":" quoted(value)
 */
SXML_TYPE_LIST JSON_KQ (SXML_TYPE_TEXT K, SXML_TYPE_TEXT V)
{
  return SXML_TTTTT( "\"", K, "\":\"", V, "\"");
}

/* -------------------------------------------------------------------------
 * same as previous one with a comma at the end:
 *   quoted(K) ":" quoted(value) ","
 */

SXML_TYPE_LIST JSON_KQ_ (SXML_TYPE_TEXT K, SXML_TYPE_TEXT V)
{
  return SXML_TTTTT ("\"", K, "\" : \"", V, "\",\n");
}

/* -------------------------------------------------------------------------
 * outputs a JSON key/value pair where value remains Unquoted:
 *   quoted(K) ":" value
 */

SXML_TYPE_LIST JSON_KU (SXML_TYPE_TEXT K, SXML_TYPE_LIST V)
{
  if (V == NULL){
    return SXML_TTT ("\"", K, "\" : {}");
  }

  return SXML_TTTL ("\"", K, "\" : ", V);
}

/* -------------------------------------------------------------------------
 * same as previous one with a comma at the end:
 *   quoted(K) ":" value ","
 */

SXML_TYPE_LIST JSON_KU_ (SXML_TYPE_TEXT K, SXML_TYPE_LIST V)
{
  if (V == NULL){
    return SXML_TTT ("\"", K, "\" : {},\n");
  }

  return SXML_TTTLT ("\"", K, "\" : ", V, ",\n");
}


/* -------------------------------------------------------------------------
 * Special characters to be quoted in JSON
 */
int isSpecialChar( char c)
{
  return (c == '\\') || (c == '"');
}


/* -------------------------------------------------------------------------
 * puts a backslash (\) before special charcaters in 'constant' so that
 * JSON will ignore it.
 * Special characters are double quote (") and backslash (\)
 */

SXML_TYPE_TEXT escapeSpecialChar( SXML_TYPE_TEXT constant)
{
  int count = 0;
  SXML_TYPE_TEXT str;
  SXML_TYPE_TEXT ret;
  SXML_TYPE_TEXT copy;

  for ( str = constant ;  *str != '\0' ; str++) {
    if( isSpecialChar(*str) ) {
      count++;
    }
  }

  if (count == 0) {
    ret = constant;
  }
  else {
    ret = malloc(strlen(constant) + count + 1);
    if (ret == NULL) {
      SXML_OOM_ERROR ("out of memory in escapeSpecialChar()");
    }

    copy = ret;
    for ( str = constant ;  *str != '\0' ; str++, copy++) {
      if( isSpecialChar(*str) ) {
	*copy = '\\';
	copy++;
      }
      *copy = *str;
    }
    *copy = '\0';
  }

  return ret;
}
