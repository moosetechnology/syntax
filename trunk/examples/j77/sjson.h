
/* -------------------------------------------------------------------------
 * outputs a JSON array:
 *   "[" LIST "]"
 */

SXML_TYPE_LIST JSON_ARRAY (SXML_TYPE_LIST L)
{
  if (L == NULL) {
    return SXML_T( "[ ]\n");
  }
  else {
    return SXML_TLT( "[\n", L, "]\n");
  }
}

/* -------------------------------------------------------------------------
 * outputs a JSON map:
 *   "{" LIST "}"
 */

SXML_TYPE_LIST JSON_MAP (SXML_TYPE_LIST L)
{
  if (L == NULL) {
    return SXML_T( "{ }\n");
  }
  else {
    return SXML_TLT( "{\n", L, "}\n");
  }
}

/* -------------------------------------------------------------------------
 * outputs a JSON key/value pair where value is a text:
 *   quoted(K) ":" quoted(value)
 */

SXML_TYPE_LIST JSON_KT (SXML_TYPE_TEXT K, SXML_TYPE_TEXT V)
{
  return SXML_TTTTT( "\"", K, "\":\"", V, "\"\n");
}

/* -------------------------------------------------------------------------
 * same as preceeding with a comma at the end:
 *   quoted(K) ":" quoted(value) ","
 */

SXML_TYPE_LIST JSON_KTV (SXML_TYPE_TEXT K, SXML_TYPE_TEXT V)
{
  return SXML_TTTTT( "\"", K, "\":\"", V, "\",\n");
}

/* -------------------------------------------------------------------------
 * outputs a JSON key/value pair where value is a list:
 *   quoted(K) ":" value
 */

SXML_TYPE_LIST JSON_KL (SXML_TYPE_TEXT K, SXML_TYPE_LIST V)
{
  return SXML_TTTL( "\"", K, "\":\n", V);
}
