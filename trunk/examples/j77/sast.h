/* -------------------------------------------------------------------------
 * ouputs line and column  position
 */
SXML_TYPE_LIST ast_location( SXML_TYPE_TEXT position,
            SXUINT line,
            SXUINT column) {
  return JSON_KU(
    position,
    JSON_MAP (
      SXML_LL (
        JSON_KQ_ ("line", SXML_UINT(line)),
  JSON_KQ ("column", SXML_UINT(column)) )));
}


/* -------------------------------------------------------------------------
 * ouputs line and column start position
 */
SXML_TYPE_LIST ast_start_location( SXUINT line,
            SXUINT column) {
  return ast_location( "start_pos", line, column);
}



/* -------------------------------------------------------------------------
 * ouputs line and column end position
 */
SXML_TYPE_LIST ast_end_location( SXUINT line,
          SXUINT column,
          SXML_TYPE_TEXT string) {
  return ast_location( "end_pos", line, column + strlen(string) - 1);
}


/* -------------------------------------------------------------------------
 * ouputs a tag
 */
SXML_TYPE_LIST ast_tag( SXML_TYPE_TEXT tag) {
  return JSON_KQ_ ("tag", tag);
}


/* -------------------------------------------------------------------------
 * creates a name
 */
SXML_TYPE_LIST ast_name( SXML_TYPE_TEXT name) {
  return JSON_KQ_ ("name", name);
}


/* -------------------------------------------------------------------------
 * ouputs a Fortran file
 * - file_name: path of the file
 * - program_units contained in the file
 */
SXML_TYPE_LIST ast_program_file( SXML_TYPE_TEXT file_name,
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
SXML_TYPE_LIST ast_program_unit( SXML_TYPE_TEXT tag,
          SXML_TYPE_LIST header,
          SXML_TYPE_LIST location,
          SXML_TYPE_LIST end_location,
          SXML_TYPE_LIST statement_list) {

  return JSON_MAP(
      SXML_LLLTLTL(
  ast_tag(tag),
  (header == NULL ? JSON_KQ_ ("name", "null") : header),
  location,
        ",\n",
  end_location,
        ",\n",
  JSON_KU ("statement_list", JSON_ARRAY( statement_list)) ));
}


/* -------------------------------------------------------------------------
 * outputs header of a program_unit (some, e.g. main_program,
 * don't have parameters, but we still output empty 'parameters')
 * Is that a problem ?
 * - name
 * - parameters
 */
SXML_TYPE_LIST ast_program_unit_header( SXML_TYPE_TEXT name,
          SXML_TYPE_LIST parameters) {

  return SXML_LL(
    JSON_KQ_ ("name", name),
    JSON_KU_ (
      "parameters",
      JSON_ARRAY( parameters) ));
}




/* -------------------------------------------------------------------------
 * adds a return type to a function_header
 * - function_header
 * - return_type
 */
SXML_TYPE_LIST ast_add_return_type(SXML_TYPE_LIST function_header,
            SXML_TYPE_LIST return_type) {
  return SXML_LL(
    function_header,
    JSON_KU_ ("return_type", return_type) );
}


/* -------------------------------------------------------------------------
 * outputs a generic statement of given tag
 * - tag: the precise kinf of statement (ex: "continue_statement")
 * - Location of the statement
 */
SXML_TYPE_LIST ast_abstract_statement( SXML_TYPE_TEXT tag,
          SXML_TYPE_LIST location) {

  return SXML_LL(
    ast_tag(tag),
    location );
}


/* -------------------------------------------------------------------------
 * outputs an unknown_statement (a statement we are not yet dealing with)
 * - rule recognizing the statement
 * - Location of the statement
 */
SXML_TYPE_LIST ast_unknown_statement( SXML_TYPE_TEXT rule,
               SXML_TYPE_LIST location) {

  return SXML_LL(
    JSON_KQ_ ("rule", rule),
    ast_abstract_statement( "unknown_statement", location) );
}

/* -------------------------------------------------------------------------
 * outputs an unknown_parameter (a parameter we are not yet dealing with)
 * - rule recognizing the parameter
 * - Location of the parameter
 */
SXML_TYPE_LIST ast_unknown_parameter( SXML_TYPE_TEXT rule,
               SXML_TYPE_LIST location) {

  return JSON_MAP(
    ast_unknown_statement(rule, location) 
    );
}

/* -------------------------------------------------------------------------
 * outputs an labeled_statement
 * - label (optional)
 * - statement
 */
SXML_TYPE_LIST ast_labeled_statement( SXML_TYPE_TEXT label,
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
 * outputs a format statement
 * - label (optional)
 * - format statement
 */
SXML_TYPE_LIST ast_format_statement( SXML_TYPE_LIST format) {
  return SXML_LL(
        ast_tag ( "format_statement"), 
        JSON_KU  ("specification", JSON_ARRAY(format))
        );
}

/* -------------------------------------------------------------------------
 * outputs a type_statement (variable declaration)
 * - type_reference
 * - Location of the statement
 * - list of variables
 */
SXML_TYPE_LIST ast_type_statement( SXML_TYPE_LIST type_reference,
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST variables) {

  return SXML_LTLTL(
    ast_abstract_statement ( "type_statement", location),
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
SXML_TYPE_LIST ast_type_reference( SXML_TYPE_LIST name,
            SXML_TYPE_LIST len_specification) {

  if (len_specification == NULL) {
    return JSON_MAP(
      SXML_LL (
        ast_tag("type_reference"),
  JSON_KQ ("name", name->TEXT) ));
  }
  else {
    return JSON_MAP(
      SXML_LLL (
        ast_tag("type_reference"),
  JSON_KQ_ ("name", name->TEXT),
  JSON_KQ ("len_specification", len_specification->TEXT) ));
  }
}


/* -------------------------------------------------------------------------
 * outputs a pause/stop_statement (with optional "argument")
 * - pause_stop: whether it is a pause_statement or a stop_statement
 * - argument of the PAUSE/STOP
 * - Location of the statement
 */
SXML_TYPE_LIST ast_pause_stop_statement(SXML_TYPE_TEXT pause_stop, 
           SXML_TYPE_LIST argument,
           SXML_TYPE_LIST location) {
  if (argument == NULL) {
    return ast_abstract_statement( pause_stop, location);
  }
  else {
    return SXML_LTL(
      ast_abstract_statement( pause_stop, location),
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
SXML_TYPE_LIST ast_call_statement( SXML_TYPE_TEXT name,
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST arguments) {

  return SXML_LTLL(
    ast_abstract_statement( "call_statement", location),
    ",\n",
    JSON_KQ_ ("name", name),
    JSON_KU(
      "arguments",
      JSON_ARRAY( arguments)) );
}


/* -------------------------------------------------------------------------
 * outputs an implicit_statement
 * - list of parameters
 */
SXML_TYPE_LIST ast_implicit_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "implicit_statement", location),
    ",\n",
    JSON_KU(
      "parameters",
      JSON_ARRAY( parameters)) ); 
}


/* -------------------------------------------------------------------------
 * outputs an typed element parameter of a body of an implicit statement
 * - a type
 * - a list of elements
 */
SXML_TYPE_LIST ast_implicit_body_parameter( 
            SXML_TYPE_LIST type,
            SXML_TYPE_LIST elements) {
    
    return JSON_MAP(
      SXML_LL(
        JSON_KU_("type", type),
        JSON_KU("elements", JSON_ARRAY(elements))
      ));
}


/* -------------------------------------------------------------------------
 * outputs a parameter_statement
 * - list of parameters
 */
SXML_TYPE_LIST ast_parameter_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "parameter_statement", location),
    ",\n",
    JSON_KU(
      "parameters",
      JSON_ARRAY( parameters)) ); 
}


/* -------------------------------------------------------------------------
 * outputs a parameter_statement
 * - list of parameters
 */
SXML_TYPE_LIST ast_parameter_statement_parameter(
            SXML_TYPE_TEXT name,
            SXML_TYPE_LIST constant_expression) {
    
    return JSON_MAP(
      SXML_LL(
        JSON_KQ_("name", name),
        JSON_KU("expression", constant_expression))  
    );
}


/* -------------------------------------------------------------------------
 * outputs a data_statement
 * - list of parameters
 */
SXML_TYPE_LIST ast_data_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "data_statement", location),
    ",\n",
    JSON_KU(
      "parameters",
      JSON_ARRAY( parameters)) ); 
}


/* -------------------------------------------------------------------------
 * outputs a parameter of a data_statement. parameter consists of:
 * - nlist: List of variables, arrays, array elements, substrings, and implied DO lists separated by commas
 * - clist: List of constants
 */
SXML_TYPE_LIST ast_data_statement_parameter(
            SXML_TYPE_LIST nlist,
            SXML_TYPE_LIST clist) {
    
    return JSON_MAP(
      SXML_LL(
        JSON_KU_("nlist", JSON_ARRAY(nlist)),
        JSON_KU("clist", JSON_ARRAY(clist)))  
    );
}


/* -------------------------------------------------------------------------
 * outputs a function_statement
 * - function name
 * - list of parameters
 * - function expression
 */
SXML_TYPE_LIST ast_function_statement( SXML_TYPE_LIST location,
            SXML_TYPE_TEXT name,
            SXML_TYPE_LIST parameters,
            SXML_TYPE_LIST expression) {
    
    return SXML_LTLLL(
    ast_abstract_statement( "function_statement", location),
    ",\n",
    JSON_KQ_("name", name),  
    JSON_KU_(
      "parameters",
      JSON_ARRAY( parameters)),
    JSON_KU("expression", expression)
    ); 
}


/* -------------------------------------------------------------------------
 * outputs a dimension_statement
 * - list of array declarators
 */
SXML_TYPE_LIST ast_dimension_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST array_declarators) {
    
    return SXML_LTL(
    ast_abstract_statement( "dimension_statement", location),
    ",\n",
    JSON_KU(
      "array_declarators",
      JSON_ARRAY( array_declarators))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs an equivalence_statement
 * - list of lists of variable names, array element names, array names, and character substring names separated by commas  
 */
SXML_TYPE_LIST ast_equivalence_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "equivalence_statement", location),
    ",\n",
    JSON_KU(
      "parameters",
      JSON_ARRAY( parameters))
    ); 
}

/* -------------------------------------------------------------------------
 * outputs a parameter of a equivalence_statement
 * - list of variable names, array element names, array names, and character substring names separated by commas  
 */
SXML_TYPE_LIST ast_equiv_entity_list(
            SXML_TYPE_LIST entity_list) {
    
    return JSON_ARRAY(entity_list);
}


/* -------------------------------------------------------------------------
 * outputs a constant
 */
SXML_TYPE_LIST ast_constant( SXML_TYPE_TEXT constant_type,
            SXML_TYPE_TEXT constant) {
  return JSON_KQ( constant_type, constant);
}


/* -------------------------------------------------------------------------
 * outputs a literal_expression.
 */
SXML_TYPE_LIST ast_literal_expression( SXML_TYPE_TEXT literal) {

  return JSON_MAP (
    SXML_LL(
      ast_tag( "literal_expression"),
      JSON_KQ( "literal", literal) ));
}


/* -------------------------------------------------------------------------
 * outputs a variable_expression.
 */
SXML_TYPE_LIST ast_variable_expression( SXML_TYPE_TEXT variable) {

  return JSON_MAP (
    SXML_LL(
      ast_tag( "variable_expression"),
      JSON_KQ( "variable", variable) ));
}


/* -------------------------------------------------------------------------
 * outputs a parenthesis_expression (expresison between parentheses)
 */
SXML_TYPE_LIST ast_parenthesis_expression( SXML_TYPE_LIST expression) {

  return JSON_MAP (
    SXML_LL(
      ast_tag( "parenthesis_expression"),
      JSON_KU( "expression", expression) ));
}


/* -------------------------------------------------------------------------
 * outputs a unary_expression.
 * - unary_operator
 * - expression
 */
SXML_TYPE_LIST ast_unary_expression( SXML_TYPE_TEXT unary_operator,
              SXML_TYPE_LIST expression) {

  return JSON_MAP (
    SXML_LLL(
      ast_tag( "unary_expression"),
      JSON_KQ_( "operator", unary_operator),
      JSON_KU ( "expression", expression) ));
}


/* -------------------------------------------------------------------------
 * outputs a binary_expression.
 */
SXML_TYPE_LIST ast_binary_expression( SXML_TYPE_LIST lhs_expression,
               SXML_TYPE_TEXT binary_operator,
               SXML_TYPE_LIST rhs_expression) {

  return JSON_MAP (
    SXML_LLLL(
      ast_tag( "binary_expression"),
      JSON_KU_ ( "lhs", lhs_expression),
      JSON_KQ_( "operator", binary_operator),
      JSON_KU ( "expression", rhs_expression) ));
}


/* -------------------------------------------------------------------------
 * outputs a variable declarator
 * - name of the variable
 * - dimension_declarator
 */
SXML_TYPE_LIST ast_variable_declarator( SXML_TYPE_TEXT variable,
           SXML_TYPE_LIST dimension_declarator) {
 return JSON_MAP(
   SXML_LLL(
     ast_tag( "variable_declarator"),
     JSON_KQ_( "variable", variable),
     JSON_KU (  "dimension_declarators", JSON_ARRAY( dimension_declarator)) ) );
}


/* -------------------------------------------------------------------------
 * adds a len_specifier in a json_map for a variable_declarator
 * needs to insert a new element in before the last element of the list
 * (which contains "}" closing a json_map)
 */
SXML_TYPE_LIST ast_add_declarator_len( SXML_TYPE_LIST variable_declarator,
           SXML_TYPE_LIST len_specifier) {
  SXML_LL(
    JSON_KQ_ ("len_specifier", len_specifier->TEXT),
    variable_declarator->SUCC );

  return variable_declarator;
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_lower_upper_bound(SXML_TYPE_LIST lower_bound,
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
