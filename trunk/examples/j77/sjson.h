
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
 * ouputs a program_unit
 * - tag: main/function/subroutine/block_data
 * - header line (name, possible parameters, return type, ...)
 * - Location of the header
 * - list of statements
 */
SXML_TYPE_LIST json_program_unit( SXML_TYPE_TEXT tag,
				  SXML_TYPE_LIST header,
				  SXML_TYPE_LIST location,
				  SXML_TYPE_LIST statement_list) {

  return JSON_MAP(
      SXML_LLLTL(
	JSON_KTV( "tag", tag),
	header,
	JSON_KL("statement_list", JSON_ARRAY( statement_list)),
        ",\n",
	location ));
}

/* -------------------------------------------------------------------------
 * ouputs an unknown_statement (a statement we are not yet dealing with)
 * - rule recognizing the statement
 * - Location of the statement
 */
SXML_TYPE_LIST json_unknown_statement( SXML_TYPE_TEXT rule,
				    SXML_TYPE_LIST location) {

  return JSON_MAP(
      SXML_LLL(
	JSON_KTV( "tag", "unknown_statement"),
	JSON_KTV("rule", rule),
	location ));
}

/* -------------------------------------------------------------------------
 * ouputs a type_statement (variable declaration)
 * - type_reference
 * - Location of the statement
 * - list of variables
 */
SXML_TYPE_LIST json_type_statement( SXML_TYPE_LIST type_reference,
				    SXML_TYPE_LIST location,
				    SXML_TYPE_LIST variables) {

  return JSON_MAP(
      SXML_LLTLTL(
	JSON_KTV( "tag", "type_statement"),
	JSON_KL("type", type_reference),
        ",\n",
	JSON_KL( "declarators", JSON_ARRAY( variables) ),
        ",\n",
	location ));
}

/* -------------------------------------------------------------------------
 * ouputs a type_reference (typically in a variable declaration statement)
 * - name of the type
 * - len_specification
 */
SXML_TYPE_LIST json_type_reference( SXML_TYPE_TEXT name,
				    SXML_TYPE_LIST len_specification) {

  return JSON_MAP(
      SXML_LL (
	JSON_KTV( "tag", "type_reference"),
	JSON_KT("name", name) ));
}
