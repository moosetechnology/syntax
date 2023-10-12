
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


/* -------------------------------------------------------------------------
 * ouputs a tag
 */
SXML_TYPE_LIST json_tag( SXML_TYPE_TEXT tag) {
  return JSON_KQ_ ("tag", tag);
}


/* -------------------------------------------------------------------------
 * ouputs a Fortran file
 * - file_name: path of the file
 * - program_units contained in the file
 */
SXML_TYPE_LIST json_program_file( SXML_TYPE_TEXT file_name,
				  SXML_TYPE_LIST program_units) {
  return SXML_LL(
    JSON_KQ_ ("file", file_name),
    JSON_KU (
      "program_units",
      JSON_ARRAY( program_units )));
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
	json_tag(tag),
	(header == NULL ? JSON_KQ_ ("name", "null") : header),
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

  return SXML_LL(
    JSON_KQ_ ("name", name),
    JSON_KU_ (
      "parameters",
      JSON_ARRAY( parameters) ));
}

/* -------------------------------------------------------------------------
 * outputs a generic statement of given tag
 * - tag: the precise kinf of statement (ex: "continue_statement")
 * - Location of the statement
 */
SXML_TYPE_LIST json_abstract_statement( SXML_TYPE_TEXT tag,
					SXML_TYPE_LIST location) {

  return SXML_LL(
    json_tag(tag),
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
 * outputs an labeled_statement
 * - label (optional)
 * - statement
 */
SXML_TYPE_LIST json_labeled_statement( SXML_TYPE_TEXT label,
				       SXML_TYPE_LIST statement) {
  if (label == NULL) {
    return JSON_MAP( statement);
  }
  else {
    return JSON_MAP( 
      SXML_LL(
        JSON_KQ_ ("label", label), 
	statement) );
  }
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
        json_tag("type_reference"),
	JSON_KQ ("name", name) ));
  }
  else {
    return JSON_MAP(
      SXML_LLL (
        json_tag("type_reference"),
	JSON_KQ_ ("name", name),
	JSON_KQ ("len_specification", len_specification) ));
  }
}


/* -------------------------------------------------------------------------
 * outputs a pause/stop_statement (with optional "argument")
 * - pause_stop: whether it is a pause_statement or a stop_statement
 * - argument of the PAUSE/STOP
 * - Location of the statement
 */
SXML_TYPE_LIST json_pause_stop_statement(SXML_TYPE_TEXT pause_stop, 
					 SXML_TYPE_LIST argument,
					 SXML_TYPE_LIST location) {
  if (argument == NULL) {
    return json_abstract_statement( pause_stop, location);
  }
  else {
    return SXML_LTL(
      json_abstract_statement( pause_stop, location),
      ",\n",
      argument);
  }
}


/* -------------------------------------------------------------------------
 * outputs a call_statement
 * - name of the procedure called
 * - Location of the statement
 * - list of arguments
 */
SXML_TYPE_LIST json_call_statement( SXML_TYPE_TEXT name,
				    SXML_TYPE_LIST location,
				    SXML_TYPE_LIST arguments) {

  return SXML_LTLL(
    json_abstract_statement( "call_statement", location),
    ",\n",
    JSON_KQ_ ("name", name),
    JSON_KU(
      "arguments",
      JSON_ARRAY( arguments)) );
}


/* -------------------------------------------------------------------------
 * check that the first element of the list contains "/"
 */
SXBOOLEAN isSlash( SXML_TYPE_LIST l) {
  return (l->TEXT[0] == '/') && (l->TEXT[1] == '\0');
}



/* -------------------------------------------------------------------------
 * creates a common_part
 * - name: optional name of the common_part
 * - declarators of the common_part
 */
SXML_TYPE_LIST output_common_part( SXML_TYPE_TEXT name,
				 SXML_TYPE_LIST declarators) {
  if (name == NULL) {
    return JSON_MAP(
      JSON_KU ( "declarators", JSON_ARRAY( declarators)) );
  }
  else {
    return JSON_MAP(
      SXML_LL (
        JSON_KQ_( "name", name),
	JSON_KU ( "declarators", JSON_ARRAY( declarators)) ));
  }
}


/* -------------------------------------------------------------------------
 * outputs a common_statement.
 * common_statment come unstructured in a list of /name/ decls , /name/decls
 * - common_parts: list of the common_parts
 * - Location of the statement
 */
SXML_TYPE_LIST output_common_statement( SXML_TYPE_LIST common_parts,
				      SXML_TYPE_LIST location) {

  return SXML_LTL(
    json_abstract_statement( "common_statement", location),
    ",\n",
    JSON_KU ("common_parts", JSON_ARRAY( common_parts)) );
}



/* -------------------------------------------------------------------------
 * adding a Common_part to a possibly empty list of common_parts
 */
SXML_TYPE_LIST add_common_part( SXML_TYPE_LIST common,
				SXML_TYPE_TEXT common_part_name,
				SXML_TYPE_LIST common_part_declarators) {
  if (common == NULL) {
    // it is the first common_part
    return output_common_part( common_part_name, common_part_declarators);
  }
  else {
    // adding common_part to 'common'
    return SXML_LTL(
      common,
      ",\n",
      output_common_part( common_part_name, common_part_declarators) );
  }
}


/* -------------------------------------------------------------------------
 * parse a common_satetment declaration to export it
 * common_statment are in an unstructured flat list of
 *   / name / decls , / name / decls
 */
SXML_TYPE_LIST json_common_statement( SXML_TYPE_LIST common_declaration,
				      SXML_TYPE_LIST location) {
  SXML_TYPE_LIST common = NULL;
  SXML_TYPE_LIST common_part_declarators = NULL;
  SXML_TYPE_TEXT common_part_name = NULL;
  SXML_TYPE_LIST l;

  l = SXML_HEAD (common_declaration);
  if (! isSlash(l)) {
    // 1st common_part is unnammed, get 1st declarator
    common_part_declarators = SXML_T( l->TEXT);
    l = SXML_SUCC (l, common_declaration);
  }

  for ( ; l != NULL ; l = SXML_SUCC (l, common_declaration) ) {

    if (isSlash(l)) {
      if (common_part_declarators != NULL) {
	common = add_common_part(
          common,
	  common_part_name,
	  common_part_declarators);
      }

      l = SXML_SUCC (l, common_declaration);  // skip 1st "/"
      if  (isSlash(l)) {
	common_part_name = "//";
      }
      else {
	common_part_name = l->TEXT;  // get common_part name
	l = SXML_SUCC (l, common_declaration);  // skip common_part name
      }

      l = SXML_SUCC (l, common_declaration);  // skip 2nd "/"
      // 1st declarator after "/.../" (in new common_part)
      common_part_declarators = SXML_T( l->TEXT);
    }

    else {
      // next declarator for this common_part
      common_part_declarators = SXML_LTT(
        common_part_declarators,
	", " ,
	l->TEXT);
    }
  }

  // last common_part
  common = add_common_part(
    common,
    common_part_name,
    common_part_declarators);

  return output_common_statement( common, location);
}


/* -------------------------------------------------------------------------
 * outputs a literal_expression.
 */
SXML_TYPE_LIST json_literal_expression( SXML_TYPE_TEXT literal) {

  return JSON_MAP (
    SXML_LL(
      json_tag( "literal_expression"),
      JSON_KQ( "literal", literal) ));
}


/* -------------------------------------------------------------------------
 * outputs a variable_expression.
 */
SXML_TYPE_LIST json_variable_expression( SXML_TYPE_TEXT variable) {

  return JSON_MAP (
    SXML_LL(
      json_tag( "variable_expression"),
      JSON_KQ( "variable", variable) ));
}


/* -------------------------------------------------------------------------
 * outputs a parenthesis_expression (expresison between parentheses)
 */
SXML_TYPE_LIST json_parenthesis_expression( SXML_TYPE_LIST expression) {

  return JSON_MAP (
    SXML_LL(
      json_tag( "parenthesis_expression"),
      JSON_KU( "expression", expression) ));
}


/* -------------------------------------------------------------------------
 * outputs a unary_expression.
 * - unary_operator
 * - expression
 */
SXML_TYPE_LIST json_unary_expression( SXML_TYPE_TEXT unary_operator,
				      SXML_TYPE_LIST expression) {

  return JSON_MAP (
    SXML_LLL(
      json_tag( "unary_expression"),
      JSON_KQ_( "operator", unary_operator),
      JSON_KU ( "expression", expression) ));
}


/* -------------------------------------------------------------------------
 * outputs a binary_expression.
 */
SXML_TYPE_LIST json_binary_expression( SXML_TYPE_LIST lhs_expression,
				       SXML_TYPE_TEXT binary_operator,
				       SXML_TYPE_LIST rhs_expression) {

  return JSON_MAP (
    SXML_LLLL(
      json_tag( "binary_expression"),
      JSON_KU ( "lhs", lhs_expression),
      JSON_KQ_( "operator", binary_operator),
      JSON_KU ( "expression", rhs_expression) ));
}


/* -------------------------------------------------------------------------
 * outputs a variable declarator
 * - name of the variable
 * - dimension_declarator
 */
SXML_TYPE_LIST json_variable_declarator( SXML_TYPE_TEXT variable,
					 SXML_TYPE_LIST dimension_declarator) {
 return JSON_MAP(
   SXML_LLL(
     json_tag( "variable_declarator"),
     JSON_KQ_( "variable", variable),
     JSON_KU (  "dimension_declarators", JSON_ARRAY( dimension_declarator)) ) );
}


/* -------------------------------------------------------------------------
 * adds a len_specifier in a json_map for a variable_declarator
 * needs to insert a new element in before the last element of the list
 * (which contains "}" closing a json_map)
 */
SXML_TYPE_LIST json_add_declarator_len( SXML_TYPE_LIST variable_declarator,
					 SXML_TYPE_TEXT len_specifier) {
  SXML_LL(
    JSON_KQ_ ("len_specifier", len_specifier),
    variable_declarator->SUCC );

  return variable_declarator;
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST json_lower_upper_bound(SXML_TYPE_LIST lower_bound,
				      SXML_TYPE_LIST upper_bound) {
  if (upper_bound == NULL) {
    return JSON_MAP(
      JSON_KU("lower_bound", lower_bound) );
  }
  else {
    return JSON_MAP(
      SXML_LL(
        JSON_KU_("lower_bound", lower_bound),
        JSON_KU ("upper_bound", upper_bound) ));
  }

}
