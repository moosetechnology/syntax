
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
  return SXML_TTTLT ("\"", K, "\" : ", V, "\",\n");
}

/* -------------------------------------------------------------------------
 * ouputs a program_unit
 * - tag: main/function/subroutine/block_data
 * - header line (name, possible parameters, return type, ...)
 * - Location of the header
 * - end location of the program unit
 * - list of statements
 */
SXML_TYPE_LIST json_program_unit( SXML_TYPE_TEXT tag,
				  SXML_TYPE_LIST header,
				  SXML_TYPE_LIST location,
				  SXML_TYPE_LIST end_location,
				  SXML_TYPE_LIST statement_list) {

  return JSON_MAP(
      SXML_LLLTLTL(
	JSON_KQ_ ("tag", tag),
	header,
	location,
        ",\n",
	end_location,
        ",\n",
	JSON_KU ("statement_list", JSON_ARRAY( statement_list)) ));
}


/* -------------------------------------------------------------------------
 * outputs header of a function or subroutine
 * - name
 * - parameters
 */
SXML_TYPE_LIST subprogram_header( SXML_TYPE_TEXT name,
				  SXML_TYPE_LIST parameters) {

  return SXML_LLT(
    JSON_KQ_ ("name", name),
    JSON_KU (
      "parameters",
      JSON_ARRAY( parameters) ),
    ",\n");
}

/* -------------------------------------------------------------------------
 * outputs a generic statement of given tag
 * - tag: the precise kinf of statement (ex: "continue_statement")
 * - Location of the statement
 */
SXML_TYPE_LIST json_abstract_statement( SXML_TYPE_TEXT tag,
					SXML_TYPE_LIST location) {

  return SXML_LL(
    JSON_KQ_ ("tag", tag),
    location );
}

/* -------------------------------------------------------------------------
 * outputs an unknown_statement (a statement we are not yet dealing with)
 * - rule recognizing the statement
 * - Location of the statement
 */
SXML_TYPE_LIST json_unknown_statement( SXML_TYPE_TEXT rule,
				       SXML_TYPE_LIST location) {

  return SXML_LL(
    JSON_KQ_ ("rule", rule),
    json_abstract_statement( "unknown_statement", location) );
}

/* -------------------------------------------------------------------------
 * outputs a type_statement (variable declaration)
 * - type_reference
 * - Location of the statement
 * - list of variables
 */
SXML_TYPE_LIST json_type_statement( SXML_TYPE_LIST type_reference,
				    SXML_TYPE_LIST location,
				    SXML_TYPE_LIST variables) {

  return SXML_LTLTL(
    json_abstract_statement ( "type_statement", location),
    ",\n",
    JSON_KU ("type", type_reference),
    ",\n",
    JSON_KU ( "declarators", JSON_ARRAY( variables) ));
}

/* -------------------------------------------------------------------------
 * ouputs a type_reference (typically in a variable declaration statement)
 * - name of the type
 * - len_specification
 */
SXML_TYPE_LIST json_type_reference( SXML_TYPE_TEXT name,
				    SXML_TYPE_TEXT len_specification) {

  if (len_specification == NULL) {
    return JSON_MAP(
      SXML_LL (
	JSON_KQ_ ("tag", "type_reference"),
	JSON_KQ ("name", name) ));
  }
else {
  return JSON_MAP(
    SXML_LLL (
      JSON_KQ_ ("tag", "type_reference"),
      JSON_KQ_ ("name", name),
      JSON_KQ ("len_specification", len_specification) ));
 }
}
