
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
  return SXML_TTTL ("\"", K, "\" : ", V);
}

/* -------------------------------------------------------------------------
 * same as previous one with a comma at the end:
 *   quoted(K) ":" value ","
 */

SXML_TYPE_LIST JSON_KU_ (SXML_TYPE_TEXT K, SXML_TYPE_LIST V)
{
  return SXML_TTTLT ("\"", K, "\" : ", V, ",\n");
}
