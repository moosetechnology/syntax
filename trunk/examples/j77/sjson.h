
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
 * outputs a JSON key/value pair where value is a text:
 *   quoted(K) ":" quoted(value)
 */

SXML_TYPE_LIST JSON_KT (SXML_TYPE_TEXT K, SXML_TYPE_TEXT V)
{
  return SXML_TTTTT( "\"", K, "\":\"", V, "\"");
}

/* -------------------------------------------------------------------------
 * same as preceeding with a comma at the end:
 *   quoted(K) ":" quoted(value) ","
 */

SXML_TYPE_LIST JSON_KTV (SXML_TYPE_TEXT K, SXML_TYPE_TEXT V)
{
  return SXML_TTTTT( "\"", K, "\" : \"", V, "\",");
}

/* -------------------------------------------------------------------------
 * outputs a JSON key/value pair where value is a list:
 *   quoted(K) ":" value
 */

SXML_TYPE_LIST JSON_KL (SXML_TYPE_TEXT K, SXML_TYPE_LIST V)
{
  return SXML_TTTL( "\"", K, "\" : ", V);
}

/* -------------------------------------------------------------------------
 * ouputs a type_statement (variable declaration)
 * - name of the type
 * - Location of the statement
 * - list of variables
 */
SXML_TYPE_LIST json_type_statement( SXML_TYPE_TEXT type, SXML_TYPE_LIST location, SXML_TYPE_LIST variables) {

  return JSON_MAP(
      SXML_LTL(
        JSON_KL(
          "type_statement",
            JSON_MAP(
              SXML_LL(
                JSON_KTV("type",type),
		JSON_KL( "declarators", JSON_ARRAY( variables) )))),
        ",\n",
	location ));
}  
