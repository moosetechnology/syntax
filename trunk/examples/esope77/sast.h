/* -------------------------------------------------------------------------
 * ouputs line and column  position
 */
SXML_TYPE_LIST ast_location( SXML_TYPE_TEXT position_name,
            SXUINT line,
            SXUINT column) {
  return JSON_KU(
    position_name,
    JSON_MAP (
      SXML_LL (
        JSON_KQ_ ("line", SXML_UINT(line)),
        JSON_KQ ("column", SXML_UINT(column)) )));
}

/* -------------------------------------------------------------------------
 * ouputs line, column position with comma after
 */
SXML_TYPE_LIST ast_location_( SXML_TYPE_TEXT position_name,
            SXUINT line,
            SXUINT column) {
  return SXML_LT(
    ast_location(position_name, line, column),
    ",\n"
  );
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
 * ouputs a tag followed by a comma
 */
SXML_TYPE_LIST ast_tag_( SXML_TYPE_TEXT tag) {
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
      ast_tag_(tag),
      (header == NULL
       ? SXML_L(
	  JSON_KU_ ("prog_unit_parameters", JSON_ARRAY(NULL)))
       : header),
      location,
      ",\n",
      end_location,
      ",\n",
      JSON_KU ("statement_list", JSON_ARRAY( statement_list)) 
    )
  );
}


/* -------------------------------------------------------------------------
 * - name
 */
SXML_TYPE_LIST ast_mainprogram_unit_header( SXML_TYPE_LIST name) {

  return SXML_L(
    JSON_KU_ ("name", name)
  );
}


/* -------------------------------------------------------------------------
 * - name
 * - parameters
 */
SXML_TYPE_LIST ast_subprogram_unit_header( SXML_TYPE_LIST name,
          SXML_TYPE_LIST parameters) {

  return SXML_LL(
    JSON_KU_ ("name", name),
    JSON_KU_ (
      "prog_unit_parameters",
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
    ast_tag_(tag),
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
 * outputs a labeled_statement.
 * Note: the label is part of the statement
 * - label (optional)
 * - statement
 */
SXML_TYPE_LIST ast_labeled_statement( SXML_TYPE_TEXT label,
               SXML_TYPE_LIST location,
               SXML_TYPE_LIST statement) {
  if (label == NULL) {
    return JSON_MAP( statement);
  }
  else {
    return JSON_MAP( 
      SXML_LLL(
        JSON_KQ_ ( "label", label),
        location, 
	      statement
      ) 
    );
  }
}


/* -------------------------------------------------------------------------
 * outputs a format statement
 * - format statement
 */
SXML_TYPE_LIST ast_format_statement( SXML_TYPE_LIST location,
	    SXML_TYPE_LIST format) {
  if(format == NULL) {
    return SXML_L(ast_abstract_statement( "format_statement", location));
  }
  else {
    return SXML_LTL(
      ast_abstract_statement( "format_statement", location),
	    ",\n",
      JSON_KU("format_specifiers", JSON_ARRAY(format))
    );
  }
}


/* -------------------------------------------------------------------------
 * outputs a parenthesized group of format specifications
 * - fmt list
 */
SXML_TYPE_LIST ast_parenthesized_fmt( SXML_TYPE_LIST location,
            SXML_TYPE_LIST fmt_spec) {

  return JSON_MAP(
    SXML_LLTL(
      ast_tag_ ("parenthesized_format_specifier"),
      location,
      ",\n",
      JSON_KU("format_specifiers_list", JSON_ARRAY(fmt_spec))
    )
  );
}

/* -------------------------------------------------------------------------
 * outputs a format specification
 * - repeat factor
 * - descriptor
 * - slash (present or absent)
 */
SXML_TYPE_LIST ast_fmt_specification( SXML_TYPE_LIST repeat_factor,
            SXML_TYPE_LIST descriptor,
            SXML_TYPE_LIST slash ) {
  
  /* don't want to list all 9 possible cases, will list only those occuring in the grammar */
  
  if (repeat_factor == NULL){
    if (descriptor == NULL){
      return JSON_MAP(
        SXML_LL(
          ast_tag_ ("format_specifier"),
          JSON_KU("carriage_or_termination", slash)
        )
      );
    }

    if (slash == NULL){
      return JSON_MAP(
        SXML_LL(
          ast_tag_("format_specifier"),
          JSON_KU("descriptor", descriptor)
        )
      );
    }

    return JSON_MAP(
      SXML_LLTL(
        ast_tag_ ("format_specifier"),
        JSON_KU("descriptor", descriptor),
        ",\n",
        JSON_KU("carriage_or_termination", slash)
      )
    );
  }

  if (slash == NULL){
    return JSON_MAP(
      SXML_LLTL(
        ast_tag_ ("format_specifier"),
        JSON_KU("repeat_factor", repeat_factor),
        ",\n",
        JSON_KU("descriptor", descriptor)
      )
    );
  }

  return JSON_MAP(
    SXML_LLTLTL(
      ast_tag_ ("format_specifier"),
      JSON_KU("repeat_factor", repeat_factor),
      ",\n",
      JSON_KU("descriptor", descriptor),
      ",\n",
      JSON_KU("carriage_or_termination", slash)
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a format statement edit descriptor
 * - descriptor type (repeatable/nonrepeatable)
 * - descriptor purpose (blank/slash/position/scale/sign control, char/int/float/logic edit, terminate)
 */
SXML_TYPE_LIST ast_edit_descriptor( SXML_TYPE_LIST location,
          SXML_TYPE_TEXT descriptor_type,
          SXML_TYPE_TEXT descriptor_purpose,
          SXML_TYPE_LIST descriptor) {

    return JSON_MAP(
      SXML_LLTLL(
        ast_tag_ ( descriptor_type),
        location,
        ",\n", 
        JSON_KQ_("descriptor_purpose", descriptor_purpose),
        JSON_KU("value", descriptor)
      )
    );
}



/* -------------------------------------------------------------------------
 * outputs a slash editor of format statement ("/" or "//")
 * - descriptor
 */
SXML_TYPE_LIST ast_format_slash_editing( SXML_TYPE_LIST location,
            SXML_TYPE_TEXT slash) {
  return JSON_MAP(
    SXML_LLTL(
      ast_tag_ ( "format_slash_editing"), 
      location,
      ",\n",
      JSON_KQ  ("slash_edit_value", slash)
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a termination control of format statement (":")
 * - descriptor
 */
SXML_TYPE_LIST ast_format_termination_control( SXML_TYPE_LIST location) {
  return JSON_MAP(
    SXML_LLTL(
      ast_tag_( "format_termination_control"), 
      location,
      ",\n",
      JSON_KQ("termination_control_value", ":")
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a repeat factor of a repeatable edit descriptor of format statement
 * - descriptor
 */
SXML_TYPE_LIST ast_format_repeat_factor( SXML_TYPE_LIST location,
            SXML_TYPE_TEXT repeat_factor) {
  return JSON_MAP(
    SXML_LLTL(
      ast_tag_( "descriptor_repeat_factor"), 
      location,
      ",\n",
      JSON_KQ("repeat_factor_value", repeat_factor)
    )
  );
}


/* -------------------------------------------------------------------------
 * 
 */
SXML_TYPE_LIST ast_scale_float_editing( SXML_TYPE_LIST location,
            SXML_TYPE_LIST scale_factor,
            SXML_TYPE_LIST repeat_factor,
            SXML_TYPE_LIST float_editing) {
  
  SXML_TYPE_LIST scale_factor_assured;

  if (scale_factor == NULL)
    scale_factor_assured = SXML_T("\"P\"");
  else 
    scale_factor_assured = scale_factor;
  
  if (repeat_factor == NULL)
    return JSON_MAP(
      SXML_LLTLL(
        ast_tag_ ("scale_control_float_editing"),
        location,
        ",\n", 
        JSON_KU_("scale_factor", scale_factor_assured),
        JSON_KU("float_editing", float_editing)
      )
    );

  else 
    return JSON_MAP(
      SXML_LLTLLL(
        ast_tag_ ("scale_control_float_editing"),
        location,
        ",\n", 
        JSON_KU_("scale_factor", scale_factor_assured),
        JSON_KU_("repeat_factor", repeat_factor),
        JSON_KU("float_editing", float_editing)
      )
    ); 
}

/* -------------------------------------------------------------------------
 * outputs a type_statement (variable declaration)
 * - type_reference
 * - Location of the statement
 * - list of variables
 */
SXML_TYPE_LIST ast_scale_factor( SXML_TYPE_LIST sign,
            SXML_TYPE_TEXT factor) {

  if (sign == NULL)
    return SXML_QUOTED_LIST(SXML_TT(factor,"P"));

  else   
    return JSON_MAP(
    SXML_LL(
      JSON_KU_("sign", sign),
      JSON_KU ("factor_value", SXML_QUOTED_LIST(SXML_TT(factor,"P")))
    )
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
    ast_abstract_statement ( "variable_declaration_statement", location),
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
SXML_TYPE_LIST ast_type_reference( SXML_TYPE_LIST location,
  SXML_TYPE_LIST name,
  SXML_TYPE_LIST len_specification) {

  if (len_specification == NULL) {
    return JSON_MAP(
      SXML_LLTL (
        ast_tag_("type_reference"),
        location,
        ",\n",
        JSON_KQ ("name", name->TEXT) 
      )
    );
  }
  else {
    return JSON_MAP(
      SXML_LLTLL (
        ast_tag_("type_reference"),
        location,
        ",\n",
        JSON_KQ_ ("name", name->TEXT),
        JSON_KU ("len_specification", len_specification)
      )
    );
  }
}


/* -------------------------------------------------------------------------
 * outputs a pause/stop/return_statement (with optional "argument")
 * - pause_stop: whether it is a pause/stop ore return_statement
 * - argument of the PAUSE/STOP/RETURN
 * - Location of the statement
 */
SXML_TYPE_LIST ast_abstract_statement_with_argument (SXML_TYPE_TEXT statement_name, 
           SXML_TYPE_LIST argument,
           SXML_TYPE_LIST location) {
  if (argument == NULL) {
    return ast_abstract_statement( statement_name, location);
  }
  else {
    return SXML_LTL(
      ast_abstract_statement( statement_name, location),
      ",\n",
      JSON_KU("argument", argument)
    );
  }
}





/* -------------------------------------------------------------------------
 * outputs a call_statement
 * - name of the procedure called
 * - Location of the statement
 * - list of arguments
 */
SXML_TYPE_LIST ast_call_statement( SXML_TYPE_LIST name,
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST arguments) {

  return SXML_LTLL(
    ast_abstract_statement( "call_statement", location),
    ",\n",
    JSON_KU_ ("name", name),
    JSON_KU(
      "arguments",
      JSON_ARRAY( arguments)) );
}


/* -------------------------------------------------------------------------
 * outputs an actual argument with return specifier
 * - return specifier
 * - argument
 */
SXML_TYPE_LIST ast_alternate_return_specifier( SXML_TYPE_LIST location,
            SXML_TYPE_TEXT return_specifier,
            SXML_TYPE_LIST argument) {

  return JSON_MAP(
    SXML_LTLL(
      ast_abstract_statement( "alternate_return_specifier", location),
      ",\n",
      JSON_KQ_ ("return_specifier", return_specifier),
      JSON_KU ("label", argument)
    )
  );
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
      JSON_KU("implicit_parameters",JSON_ARRAY( parameters)) 
    ); 
}

/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_implicit_none_statement( SXML_TYPE_LIST location,
  SXML_TYPE_LIST location_none) {
    
    return SXML_LL(
      location_none,
      ast_abstract_statement( "implicit_none_statement", location)
      ); 
}


/* -------------------------------------------------------------------------
 * outputs an typed element parameter of a body of an implicit statement
 * - a type
 * - a list of elements
 */
SXML_TYPE_LIST ast_implicit_body_parameter( SXML_TYPE_LIST location,
            SXML_TYPE_LIST type,
            SXML_TYPE_LIST elements) {
    
    return JSON_MAP(
      SXML_LLTLL(
        ast_tag_("implicit_type_rule"),
        location,
        ",\n",
        JSON_KU_("type", type),
        JSON_KU("implicit_elements", JSON_ARRAY(elements))
      ));
}

/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_implicit_element(SXML_TYPE_LIST location,
              SXML_TYPE_TEXT left,
              SXML_TYPE_TEXT right
              ) {
  return JSON_MAP(
    SXML_LLTLL(
      ast_tag_("implicit_range"),
      location,
      ",\n",
      JSON_KQ_("implicit_range_lower", left),
      JSON_KQ("implicit_range_upper", right)
      )
  );
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
        "constant_declarations",
        JSON_ARRAY( parameters)) ); 
}


/* -------------------------------------------------------------------------
 * outputs a parameter assignment
 * - symbolic name assigned
 * - constant expression
 */
SXML_TYPE_LIST ast_parameter_assign(SXML_TYPE_LIST location,
            SXML_TYPE_LIST name,
            SXML_TYPE_LIST constant_expression) {
    
    return JSON_MAP(
      SXML_LTLL(
        location,
        ",\n",
        JSON_KU_("symbolic_name", name),
        JSON_KU("expression", constant_expression))  
    );
}


/* -------------------------------------------------------------------------
 * outputs a data_statement
 * - list of parameters
 */
SXML_TYPE_LIST ast_data_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST groups) {
    
    return SXML_LTL(
      ast_abstract_statement( "data_statement", location),
      ",\n",
      JSON_KU(
        "data_groups",
        JSON_ARRAY( groups)) ); 
}


/* -------------------------------------------------------------------------
 * outputs a parameter of a data_statement. parameter consists of:
 * - nlist: List of variables, arrays, array elements, substrings, and implied DO lists separated by commas
 * - clist: List of constants
 */
SXML_TYPE_LIST ast_data_statement_group(
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST nlist,
            SXML_TYPE_LIST clist) {
    
    return JSON_MAP(
      SXML_LLTLL(
        ast_tag_("data_group"),
        location,
        ",\n",
        JSON_KU_("data_group_variables_list", JSON_ARRAY(nlist)),
        JSON_KU("data_group_constants_list", JSON_ARRAY(clist)))  
    );
}


/* -------------------------------------------------------------------------
 * outputs a variable from data statement variables list
 */
SXML_TYPE_LIST ast_data_statement_variable(
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST variable) {
    
    return JSON_MAP(
      SXML_LLTL(
        ast_tag_("variable"),
        location,
        ",\n",
        JSON_KU("variable_name", variable)
      )  
    );
}


/* -------------------------------------------------------------------------
 * outputs a value from data statement constant list
 * - number of successive occurrences of the constant
 * - constant
 */
SXML_TYPE_LIST ast_data_statement_constant(SXML_TYPE_LIST location,
            SXML_TYPE_LIST occurence,
            SXML_TYPE_LIST constant) {
    
    if (occurence == NULL)
      occurence = SXML_T("{}");
    
    return JSON_MAP(
      SXML_LLTLL(
        ast_tag_("data_group_constant"),
        location,
        ",\n",
        JSON_KU_("occurence", occurence),
        JSON_KU("constant", constant)
      )  
    );
}


/* -------------------------------------------------------------------------
 * outputs a function_statement
 * - function name
 * - list of parameters
 * - function expression
 */
SXML_TYPE_LIST ast_function_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST name,
            SXML_TYPE_LIST parameters,
            SXML_TYPE_LIST expression) {
    
    return SXML_LTLLL(
      ast_abstract_statement( "function_statement", location),
      ",\n",
      JSON_KU_("name", name),  
      JSON_KU_(
        "prog_unit_parameters",
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
            SXML_TYPE_LIST equiv_groups) {
    
    return SXML_LTL(
      ast_abstract_statement( "equivalence_statement", location),
      ",\n",
      JSON_KU(
        "equivalence_groups",
        JSON_ARRAY( equiv_groups))
      ); 
}


/* -------------------------------------------------------------------------
 * outputs a "group" of a equivalent entities
 * - list of variable names, array element names, array names, and character substring names separated by commas  
 */
SXML_TYPE_LIST ast_equiv_group(SXML_TYPE_LIST location,
            SXML_TYPE_LIST entity_list) {
    
    return JSON_MAP(
      SXML_LLTL(
        ast_tag_("equivalence_group"),
        location,
        ",\n",
        JSON_KU("equivalence_group_elements", JSON_ARRAY(entity_list))
      )
    );
}


/* -------------------------------------------------------------------------
 * outputs an equivalent entities
 */
SXML_TYPE_LIST ast_equiv_entity(SXML_TYPE_LIST location,
            SXML_TYPE_LIST entity) {
    
    return JSON_MAP(
      SXML_LLTL(
        ast_tag_("equivalence_entity"),
        location,
        ",\n",
        JSON_KU("equivalence_entity", entity)
      )
    );
}


/* -------------------------------------------------------------------------
 * outputs a common_statement
 * - common_body = a list of pairs [name]/list of variable names, array names, and array declarators 
 */
SXML_TYPE_LIST ast_common_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST common_body) {
    
    return SXML_LTL(
      ast_abstract_statement( "common_statement", location),
      ",\n",
      JSON_KU(
        "common_body",
        JSON_ARRAY( common_body))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs a group of elements in a statement boy. Consists of:
 * - group name (optional)
 * - one element of nlist: List of variables, arrays, array elements, substrings, and implied DO lists separated by commas
 */
SXML_TYPE_LIST ast_common_block_group(SXML_TYPE_LIST location,
            SXML_TYPE_LIST common_block_name,
            SXML_TYPE_LIST common_element_name) {

    if (common_block_name == NULL){
      return JSON_MAP(
        SXML_LLTL(
          ast_tag_("common_group"),
          location,
          ",\n",
          JSON_KU("common_element_name", common_element_name) 
        )
      );
    }

    return JSON_MAP(
      SXML_LLTLL(
        ast_tag_("common_group"),
        location,
        ",\n",
        JSON_KU_("common_block_name", common_block_name),
        JSON_KU("common_element_name", common_element_name) 
      )
    );
}


/* -------------------------------------------------------------------------
 * outputs an external statement
 * - proc_list: names of external procedure, dummy procedure, or block data routine.
 */
SXML_TYPE_LIST ast_external_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST proc_list) {
    
    return SXML_LTL(
    ast_abstract_statement( "external_statement", location),
    ",\n",
    JSON_KU(
      "proc_list",
      JSON_ARRAY( proc_list))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs an intrinsic statement
 * - proc_list: list of function names  
 */
SXML_TYPE_LIST ast_intrinsic_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST fun_list) {
    
    return SXML_LTL(
    ast_abstract_statement( "intrinsic_statement", location),
    ",\n",
    JSON_KU(
      "fun_list",
      JSON_ARRAY( fun_list))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs a save statement
 * - var_list: list of names of an array, variables, or common blocks (enclosed in slashes), occurring in a subprogram  
 */
SXML_TYPE_LIST ast_save_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST var_list) {
    
    return SXML_LTL(
    ast_abstract_statement( "save_statement", location),
    ",\n",
    JSON_KU(
      "save_statement_namelist",
      JSON_ARRAY( var_list))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs a save statement
 * - var_list: list of names of an array, variables, or common blocks (enclosed in slashes), occurring in a subprogram  
 */
SXML_TYPE_LIST ast_save_statement_namelist_element( SXML_TYPE_LIST location,
            SXML_TYPE_TEXT guarded,
            SXML_TYPE_LIST element) {
    
    return JSON_MAP(
      SXML_LLTLL(
        ast_tag_("save_statement_namelist_element"),
        location,
        ",\n",
        JSON_KQ_("guarded", guarded),
        JSON_KU("element_value", element)
      )
    ); 
}


/* -------------------------------------------------------------------------
 * outputs an assignment_statement
 * - left part (assignee)
 * - right part (assigned)
 */
SXML_TYPE_LIST ast_assignment_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST left,
            SXML_TYPE_LIST right) {
    
    return SXML_LTLL(
      ast_abstract_statement( "assignment_statement", location),
      ",\n",
      JSON_KU_("left",left),
      JSON_KU("right",right)
    ); 
}

/* -------------------------------------------------------------------------
 * outputs an option in a control_info_elem
 * similar to assignment_statement without the location
 */
SXML_TYPE_LIST ast_io_option(
            SXML_TYPE_LIST left,
            SXML_TYPE_LIST right) {
    
    return JSON_MAP(
      SXML_LLL(
        ast_tag_( "control_info_option"),
	JSON_KU_( "symbolic_name", left),
	JSON_KU( "right", JSON_ARRAY( right))
      ) ); 
}


/* -------------------------------------------------------------------------
 * outputs a goto_statement
 * - type (unconditional, computed, assigned)
 * - label(s)
 * - variable or expression to which goto branches
 */
SXML_TYPE_LIST ast_goto_statement( SXML_TYPE_LIST location,
            SXML_TYPE_TEXT type,
            SXML_TYPE_LIST labels,
            SXML_TYPE_LIST var) {

  char *tag;
  tag = malloc( strlen( type) + 16);
  if (tag == NULL) {
    fprintf(stderr, "Could not allocate memory in ast_goto_statement\n");
    exit(0);
  }
  strcpy(tag, type);
  strcpy( tag+strlen(type), "_goto_statement");

  return SXML_LTLL(
    ast_abstract_statement( tag, location),
    ",\n",
    JSON_KU_(
      "labels",
      JSON_ARRAY( labels)),
    JSON_KU(
      "var",
      JSON_ARRAY( var))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs an arithmetic_if_statement
 * - conditional expression
 * - label s1 of executable statements
 * - label s2 of executable statements
* - label s3 of executable statements
 */
SXML_TYPE_LIST ast_arithmetic_if_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST expression,
            SXML_TYPE_LIST s1,
            SXML_TYPE_LIST s2,
            SXML_TYPE_LIST s3
            ) {
    
    return SXML_LTLL(
    ast_abstract_statement( "arithmetic_if_statement", location),
    ",\n",
    JSON_KU_(
      "expression",
      JSON_ARRAY( expression)),
    JSON_KU(
      "labels",
      JSON_ARRAY( SXML_LTLTL( s1, ",\n", s2, ",\n", s3)))
    );
}


/* -------------------------------------------------------------------------
 * outputs a write_statement
 * - control_info_list of parameters UNIT, FMT, IOSTAT, REC, ERR
 * - io_list: list of variables 
 */
SXML_TYPE_LIST ast_write_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST control_info_list,
            SXML_TYPE_LIST io_list) {

    return SXML_LTLL(
    ast_abstract_statement( "write_statement", location),
    ",\n",
    JSON_KU_(
      "control_info_list",
      JSON_ARRAY( control_info_list)),
    JSON_KU(
      "io_list",
      JSON_ARRAY( io_list))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs a read_statement
 * - format identifier  
 * - control_info_list of parameters UNIT, FMT, IOSTAT, REC, ERR, etc
 * - io_list: list of variables [optional]
 * format and control_info_list are mutually exclusive
 */
SXML_TYPE_LIST ast_read_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST format,
            SXML_TYPE_LIST control_info_list,
            SXML_TYPE_LIST io_list) {

  if (format == NULL) {
    return SXML_LTLL(
      ast_abstract_statement( "read_statement", location),
      ",\n",
      JSON_KU_(
        "control_info_list",
	JSON_ARRAY( control_info_list)),
      JSON_KU(
        "io_list",
	JSON_ARRAY( io_list))
    );
  }
  else {
    return SXML_LTLL(
      ast_abstract_statement( "read_statement", location),
      ",\n",
      JSON_KU_(
        "format",
	JSON_ARRAY( format)),
      JSON_KU(
        "io_list",
	JSON_ARRAY( io_list))
    );
  }
}


/* -------------------------------------------------------------------------
 * outputs a print_statement
 * - io_list: list of variables
 * - format identifier  
 */
SXML_TYPE_LIST ast_print_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST format,
            SXML_TYPE_LIST io_list) {
    
    return SXML_LTLL(
      ast_abstract_statement( "print_statement", location),
      ",\n",
      JSON_KU_(
	"format",
	JSON_ARRAY( format)),
      JSON_KU(
	"io_list",
	JSON_ARRAY( io_list))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs a rewind_statement
 * - parameters: UNIT, [IOSTAT], [ERR]
 */
SXML_TYPE_LIST ast_rewind_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "rewind_statement", location),
    ",\n",
    JSON_KU(
      "control_info_list",
      JSON_ARRAY( parameters))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs a backspace_statement
 * - parameters: UNIT, [IOSTAT], [ERR]
 */
SXML_TYPE_LIST ast_backspace_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "backspace_statement", location),
    ",\n",
    JSON_KU(
      "control_info_list",
      JSON_ARRAY( parameters))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs an endfile_statement
 * - parameters: UNIT, [IOSTAT], [ERR]
 */
SXML_TYPE_LIST ast_endfile_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "endfile_statement", location),
    ",\n",
    JSON_KU(
      "control_info_list",
      JSON_ARRAY( parameters))
    ); 
}

/* -------------------------------------------------------------------------
 * outputs an open_statement
 * - parameters: UNIT, and optional parameters (FILE, ACCESS, BLANK, ERR, FORM, IOSTAT, RECL/RECORDSIZE, STATUS/TYPE)
 */
SXML_TYPE_LIST ast_open_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "open_statement", location),
    ",\n",
    JSON_KU(
      "control_info_list",
      JSON_ARRAY( parameters))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs a close_statement
 * - parameters: UNIT, and optional parameters (ERR, IOSTAT, STATUS)
 */
SXML_TYPE_LIST ast_close_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "close_statement", location),
    ",\n",
    JSON_KU(
      "control_info_list",
      JSON_ARRAY( parameters))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs an inquire_statement
 * - parameters: UNIT, and optional parameters
 */
SXML_TYPE_LIST ast_inquire_statement( SXML_TYPE_LIST location,
            SXML_TYPE_LIST parameters) {
    
    return SXML_LTL(
    ast_abstract_statement( "inquire_statement", location),
    ",\n",
    JSON_KU(
      "control_info_list",
      JSON_ARRAY( parameters))
    ); 
}


/* -------------------------------------------------------------------------
 * outputs an option of a read/open/write/rewind and other statements. possible parameters UNIT, FMT, IOSTAT, REC, ERR
 * - option name
 * - option value
 */
SXML_TYPE_LIST ast_control_info_elem(SXML_TYPE_LIST location,
            SXML_TYPE_TEXT name,
            SXML_TYPE_LIST value) {

  return JSON_MAP(
    SXML_LLTLL(
      ast_tag_( "control_info_option"),
      location,
      ",\n",
      JSON_KQ_("control_option_name", name),
      JSON_KU("control_option_value", value)
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a constant
 * location
 * constant type (int, real, dp; signed or unsigned)
 * constant value
 */
SXML_TYPE_LIST ast_constant(
            SXML_TYPE_LIST location, 
            SXML_TYPE_TEXT constant_type,
            SXML_TYPE_TEXT constant) {

  return JSON_MAP(
    SXML_LTL(
      ast_abstract_statement(constant_type, location),
      ",\n",
      JSON_KQ( "value", escapeSpecialChar(constant) )
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a signed constant
 * constant sign
 * constant object
 */
SXML_TYPE_LIST ast_constant_signed(
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST sign,
            SXML_TYPE_LIST constant) {
  return JSON_MAP(
    SXML_LLTLL(
      ast_tag_("constant_signed"),
      location,
      ",\n",
      JSON_KU_("sign", sign),
      JSON_KU( "constant", constant)
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a constant expression
 * constant expression type 
 * expression value
 */
SXML_TYPE_LIST ast_const_expression(
            SXML_TYPE_TEXT type,
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST expression) {
  
  return JSON_MAP(
    SXML_LLTL(
      ast_tag_(type),
      location,
      ",\n",
      JSON_KU( "expression", expression)
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a complex constant
 * location
 * constant type
 * constant value
 */
SXML_TYPE_LIST ast_complex_constant(
            SXML_TYPE_LIST location, 
            SXML_TYPE_LIST real_part,
            SXML_TYPE_LIST complex_part) {
  return JSON_MAP(
    SXML_LTLL(
      ast_abstract_statement("complex_constant", location),
      ",\n",
      JSON_KU_( "complex_constant_real_part", real_part),
      JSON_KU( "complex_constant_complex_part", complex_part)
    )
  );
}

/* -------------------------------------------------------------------------
 * outputs a real part of a complex constant
 * [sign]
 * constant
 */
SXML_TYPE_LIST ast_complex_constant_real_part(
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST sign,
            SXML_TYPE_LIST constant) {

  if (sign == NULL){
    return JSON_MAP(
      SXML_LTL(
        location,
        ",\n",
        JSON_KU( "value", constant)
      )
    );
  }

  return JSON_MAP(
    SXML_LTLL(
      location,
        ",\n",
      JSON_KU_( "constant_sign", sign),
      JSON_KU( "value", constant)
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a literal_expression.
 */
SXML_TYPE_LIST ast_literal_expression( SXML_TYPE_LIST location, 
            SXML_TYPE_LIST literal) {

  return JSON_MAP (
    SXML_LLTL(
      ast_tag_( "literal_expression"),
      location,
      ",\n",
      JSON_KU( "expression", literal) ));
}


/* -------------------------------------------------------------------------
 * outputs a variable_expression.
 */
SXML_TYPE_LIST ast_variable_expression( SXML_TYPE_LIST variable) {

  return JSON_MAP (
    SXML_LL(
      ast_tag_( "variable_expression"),
      JSON_KU( "variable_name", variable) ));
}


/* -------------------------------------------------------------------------
 * outputs a scalar variable_expression.
 */
SXML_TYPE_LIST ast_scalar_variable( SXML_TYPE_LIST location,
          SXML_TYPE_LIST variable) {

  return JSON_MAP (
    SXML_LLTL(
      ast_tag_( "scalar_variable"),
      location,
      ",\n",
      JSON_KU( "variable_name", variable) 
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a variable_expression (when it is unclear what is the type of variable, e.g array name of scalar var name in subroutine parameter)
 */
SXML_TYPE_LIST ast_variable( SXML_TYPE_LIST location,
          SXML_TYPE_LIST variable) {

  return JSON_MAP (
    SXML_LLTL(
      ast_tag_( "variable"),
      location,
      ",\n",
      JSON_KU( "variable_name", variable) 
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a parenthesis_expression (expresison between parentheses)
 */
SXML_TYPE_LIST ast_parenthesis_expression( SXML_TYPE_LIST expression) {

  return JSON_MAP (
    SXML_LL(
      ast_tag_( "parenthesis_expression"),
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
      ast_tag_( "unary_expression"),
      JSON_KQ_( "operator", unary_operator),
      JSON_KU ( "expression", expression) ));
}


/* -------------------------------------------------------------------------
 * outputs a binary_expression.
 */
SXML_TYPE_LIST ast_binary_expression( SXML_TYPE_LIST location,
               SXML_TYPE_LIST lhs_expression,
               SXML_TYPE_LIST binary_operator,
               SXML_TYPE_LIST rhs_expression) {

  return JSON_MAP (
    SXML_LLTLLL(
      ast_tag_( "binary_expression"),
      location,
      ",\n",
      JSON_KU_ ( "left", lhs_expression),
      JSON_KU_( "operator", binary_operator),
      JSON_KU ( "right", rhs_expression) ));
}


/* -------------------------------------------------------------------------
 * outputs a relational operator
 */
SXML_TYPE_LIST ast_operator( SXML_TYPE_LIST location,
              SXML_TYPE_TEXT tag,
              SXML_TYPE_TEXT operator) {

  return JSON_MAP (
    SXML_LLTL(
      ast_tag_(tag),
      location,
      ",\n",
      JSON_KQ("operator_value", operator) 
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs an expression.
 */
SXML_TYPE_LIST ast_expression( SXML_TYPE_TEXT tag,
  SXML_TYPE_LIST expression) {

  return JSON_MAP (
    SXML_LL(
      ast_tag_(tag),
      JSON_KU( "expression", expression) 
    )
  );
}


/* -------------------------------------------------------------------------
 * outputs a variable declarator
 * - name of the variable
 * - dimension_declarator
 */
SXML_TYPE_LIST ast_variable_declarator( SXML_TYPE_LIST location,
           SXML_TYPE_LIST variable,
           SXML_TYPE_LIST dimension_declarator) {
 return JSON_MAP(
   SXML_LLTLL(
     ast_tag_( "variable_declarator"),
     location,
     ",\n",
     JSON_KU_( "variable_name", variable),
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
    JSON_KU_ ("len_specifier", len_specifier),
    variable_declarator->SUCC );

  return variable_declarator;
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_dimension_declarator(SXML_TYPE_LIST location,
              SXML_TYPE_LIST lower_bound,
              SXML_TYPE_LIST upper_bound) {
  if (lower_bound == NULL) {
    return JSON_MAP(
      SXML_LLTL(
        ast_tag_("dimension_declarator"),
        location,
        ",\n",
        JSON_KU ("upper_bound", upper_bound) 
      )
    );
  }
  else {
    return JSON_MAP(
      SXML_LLTLL(
        ast_tag_("dimension_declarator"),
        location,
        ",\n",
        JSON_KU_("lower_bound", lower_bound),
        JSON_KU ("upper_bound", upper_bound) 
      )
    );
  }
}


/* -------------------------------------------------------------------------
 * outputs a substring
 * - character variable name 
 * - character array element name 
 * - leftmost character position of the substring 
 * - rightmost character position of the substring 
 */
SXML_TYPE_LIST ast_substring(SXML_TYPE_LIST location,
              SXML_TYPE_LIST variable,
              SXML_TYPE_LIST array,
              SXML_TYPE_LIST lower_bound,
              SXML_TYPE_LIST upper_bound) {
  
  SXML_TYPE_LIST variable_or_array;
  SXML_TYPE_LIST upper_bound_safe = upper_bound;
  SXML_TYPE_LIST lower_bound_safe = lower_bound;

  if (array == NULL) {
    variable_or_array = JSON_KU_("variable", variable);
  } 
  else 
    variable_or_array = JSON_KU_("array_element", array);

  if (upper_bound == NULL) {
    upper_bound_safe = SXML_T("\"\"");
  } 

  if (lower_bound == NULL) {
    lower_bound_safe = SXML_T("\"\"");
  }  

  return JSON_MAP(
    SXML_LLTLLL(
      ast_tag_("substring"),
      location,
      ",\n",
      variable_or_array,
      JSON_KU_("lower_bound", lower_bound_safe),
      JSON_KU ("upper_bound", upper_bound_safe)
      )
    );
}


/* -------------------------------------------------------------------------
 * outputs an array_element_name or a function reference qualified by a subscript.
 * - array name 
 * - qualifiers
 */
SXML_TYPE_LIST ast_array_element_name(SXML_TYPE_LIST location,
              SXML_TYPE_LIST name,
              SXML_TYPE_LIST qualifiers) {

  return JSON_MAP(
    SXML_LLTLL(
      ast_tag_("array_element_name"),
        location,
        ",\n",
        JSON_KU_("name", name),
        JSON_KU ("qualifiers_list", JSON_ARRAY(qualifiers))
      )
    );
}


/* -------------------------------------------------------------------------
 * ouputs a funciton_reference
 * - name of the function
 */
SXML_TYPE_LIST ast_function_reference( SXML_TYPE_LIST location,
              SXML_TYPE_LIST name) {

  return JSON_MAP(
    SXML_LLTL (
        ast_tag_("function_reference"),
        location,
        ",\n",
        JSON_KU("name", name)
    )
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_text_to_map(
              SXML_TYPE_LIST location,
              SXML_TYPE_TEXT tag,
              SXML_TYPE_TEXT obj) {
  return JSON_MAP(
    SXML_LL(
      JSON_KQ_(tag, obj),
      location
    )
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST name_to_list(
              SXML_TYPE_TEXT name) {
  return SXML_T(SXML_Q(name));
}


/* -------------------------------------------------------------------------
 * outputs a symbolic name
 */
SXML_TYPE_LIST ast_symbolic_name( SXML_TYPE_LIST location,
  SXML_TYPE_TEXT name) {

  return JSON_MAP (
    SXML_LTL(
      ast_abstract_statement( "symbolic_name", location),
      ",\n",
      JSON_KQ("name", name)
    )
  );
}


/* -------------------------------------------------------------------------
 * ouputs a logical primary
 * - primary expression
 */
SXML_TYPE_LIST ast_logical_primary( SXML_TYPE_LIST expression) {

  return JSON_MAP(
    SXML_LL (
      ast_tag_("logical_primary"),
      JSON_KU("expression", expression)
    )
  );
}


/* -------------------------------------------------------------------------
 * ouputs a logical factor
 * - primary expression
 */
SXML_TYPE_LIST ast_logical_factor( SXML_TYPE_TEXT negated,
                SXML_TYPE_LIST expression) {

  return JSON_MAP(
    SXML_LLL (
      ast_tag_("logical_factor"),
      JSON_KQ_("negated", negated),
      JSON_KU("expression", expression)
    )
  );
}


/* -------------------------------------------------------------------------
 * ouputs a data_imply_do_list
 * - dlist: list of array element names and implied DO lists
 * -- the last element of dlist is iv: implied DO variable
 * - initial value of iv
 * - limit value of iv
 * - increment value of iv (if omitted, a default value of 1 is assumed)
 */
SXML_TYPE_LIST ast_data_imply_do_list( SXML_TYPE_LIST location,
                SXML_TYPE_LIST dlist,
                SXML_TYPE_LIST do_parameters) {

  return JSON_MAP(
    SXML_LLTLTL (
      ast_tag_("data_imply_do_list"),
      location,
      ",\n",
      JSON_KU("dlist", JSON_ARRAY(dlist)),
      ",\n",
      do_parameters
    )
  );
}


/* -------------------------------------------------------------------------
 * creates a do loop
 * - description of a do loop (location, loop control, etc)
 * - statements list
 */
SXML_TYPE_LIST ast_do_loop(
            SXML_TYPE_LIST do_statement,
            SXML_TYPE_LIST statement_list,
            SXML_TYPE_LIST end_do) {

  if (statement_list == NULL){
    return SXML_LL(
      do_statement,
      JSON_KU("end_do", end_do)
    );
  }
  else {
    return SXML_LLL(
      do_statement,
      JSON_KU_("statement_list", JSON_ARRAY(statement_list)),
      JSON_KU("end_do", end_do)
    );
  }
}


/* -------------------------------------------------------------------------
 * prepares a do statement used in a do loop
 * - Location of the statement
 * - statment number
 * - variable name to use with loop control parameters
 * - loop control parameters (initial, limit, increment)
 */
SXML_TYPE_LIST ast_do_statement(
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST loop_control_location,
            SXML_TYPE_LIST statement_number,
            SXML_TYPE_LIST variable_name,
            SXML_TYPE_LIST do_parameters) {

  if (statement_number == NULL){
    return SXML_LTL(
      ast_abstract_statement( "do_statement", location),
      ",\n",
      JSON_KU_ ("loop_control", 
        JSON_MAP(
          SXML_LLL(
            loop_control_location,
            JSON_KU_("variable_name", variable_name),
            do_parameters
          )
        )
      )
    );
  }

  return SXML_LTLL(
    ast_abstract_statement( "do_statement", location),
    ",\n",
    JSON_KU_ ("statement_number", statement_number),
    JSON_KU_ ("loop_control", 
      JSON_MAP(
        SXML_LLL(
          loop_control_location,
          JSON_KU_("variable_name", variable_name),
          do_parameters
        )
      )
    )
  );
}


/* -------------------------------------------------------------------------
 * prepares a do-while statement used in a do loop
 * - Location of the statement
 * - statment number
 * - loop controll logical expression
 */
SXML_TYPE_LIST ast_do_while_statement(
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST location_while,
            SXML_TYPE_LIST statement_number,
            SXML_TYPE_LIST logical_expression) {

  if (statement_number == NULL){
    return SXML_LTLL(
      ast_abstract_statement( "do_statement", location),
      ",\n",
      location_while,
      JSON_KU_ ("loop_control_expression", logical_expression) 
    );
  }

  return SXML_LTLLL(
    ast_abstract_statement( "do_statement", location),
    ",\n",
    location_while,
    JSON_KU_ ("statement_number", statement_number),
    JSON_KU_ ("loop_control_expression", logical_expression) 
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_do_parameters(SXML_TYPE_LIST init,
              SXML_TYPE_LIST limit,
              SXML_TYPE_LIST increment) {

    if (increment == NULL){
      return SXML_LL(
        JSON_KU_("init", init),
        JSON_KU("limit", limit)
      );
    }
    else {
      return SXML_LLL(
        JSON_KU_("init", init),
	      JSON_KU_("limit", limit),
	      JSON_KU("increment", increment)
      );
    }
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_end_do(SXML_TYPE_LIST location,
              SXML_TYPE_TEXT label) {
  
    if (label == NULL) {
      return JSON_MAP(location);
    }

    return JSON_MAP(
      SXML_LTL(
        location,
        ",\n",
        JSON_KQ("label", label)
      )
    );
}


/* -------------------------------------------------------------------------
 * outputs a logical if statement
 * - Location of the statement
 * - logical expression
 * - statement to execute
 */
SXML_TYPE_LIST ast_logical_if_statement(
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST expression,
            SXML_TYPE_LIST statement) {

  return SXML_LTLL(
    ast_abstract_statement( "logical_if_statement", location),
    ",\n",
    JSON_KU_ ("expression", expression),
    JSON_KU ("statement", JSON_MAP(statement))
  );
}


/* -------------------------------------------------------------------------
 * ouputs a ELSE possibly labeled, possibly with a IF
 *   ends with a comma (part of a IF or ELSE IF node)
 * - tag : differentiate beween a simple ELSE or a ELSE IF
 * - label (optional)
 * - location of the ELSE
 */
SXML_TYPE_LIST ast_else_(
              SXML_TYPE_LIST location,
	      SXML_TYPE_TEXT tag,
	      SXML_TYPE_TEXT label) {

    if (label == NULL){
      return SXML_LLT(
        ast_tag_(tag),
	location,
	",\n"
      );      
    }
    else {
      return SXML_LLTL(
        ast_tag_(tag),  
        location,
        ",\n",
        JSON_KQ_("label", label)
      );
    }
}


/* -------------------------------------------------------------------------
 * BlockIf statement
 * - condition
 * - then_statements (optional), this is an array of statements
 * - else_block (optional), this is a ELSE node (containing statements)
 */
SXML_TYPE_LIST ast_block_if_statement(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST then_token_location,
              SXML_TYPE_LIST condition,
              SXML_TYPE_LIST then_statements,
              SXML_TYPE_LIST else_block
              ) {

  return SXML_LTLLLL(
    ast_abstract_statement("block_if_statement", location),
    ",\n",
    then_token_location,
    JSON_KU_("condition", condition),
    JSON_KU_("then_statements", JSON_ARRAY(then_statements)),
    JSON_KU("else_block", else_block)
  );
}


/* -------------------------------------------------------------------------
 * ELSE followed by BlockIf
 * - else_if_line : the line "ELSE IF (condition) THEN"
 * - then_statements, this is an array of statements
 * - else_block, this is a ELSE node (containing statements)
 */
SXML_TYPE_LIST ast_else_if_else(SXML_TYPE_LIST location,
              SXML_TYPE_LIST else_if_line,
              SXML_TYPE_LIST then_statements,
              SXML_TYPE_LIST else_block
              ) {

    return JSON_MAP(
      SXML_LTLLL (
        location,
        ",\n",
        else_if_line,
        JSON_KU_("then_statements", JSON_ARRAY(then_statements)),
        JSON_KU("else_block", else_block)
      )
    );
}


/* -------------------------------------------------------------------------
 * a line: ELSE followed by IF (condition) THEN
 * the nested statements (THEN and ELSE parts) are not here but
 *   in ast_else_if_else(...)
 * May be labeled
 */
SXML_TYPE_LIST ast_else_if_line(
              SXML_TYPE_LIST location_else,
              SXML_TYPE_LIST location_then,
              SXML_TYPE_TEXT label,
              SXML_TYPE_LIST condition
              ) {

    return SXML_LLL(
      ast_else_(location_else, "else_if", label),
      location_then,
      JSON_KU_("condition", condition)
    );
}


/* -------------------------------------------------------------------------
 * ELSE keyword in a BlockIf, part of the BlockIf
 * May be labeled
 */
SXML_TYPE_LIST ast_else_line(
              SXML_TYPE_LIST location,
	      SXML_TYPE_TEXT label) {

    return ast_else_(location, "else", label);
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_else_block(SXML_TYPE_LIST location,
              SXML_TYPE_LIST label,
              SXML_TYPE_LIST else_statements,
              SXML_TYPE_LIST end_if) {

    if (label == NULL){
      return JSON_MAP(
        SXML_LTLL(
          location,
          ",\n",
          JSON_KU_("statement_list", JSON_ARRAY(else_statements)),
          JSON_KU ("end_if", end_if)
        )
      );
    }
    else {
      return JSON_MAP(
        SXML_LTLLL(
          location,
          ",\n",
          label,
          JSON_KU_("statement_list", JSON_ARRAY(else_statements)),
          JSON_KU ("end_if", end_if)
        )
      );
    }
}


/* -------------------------------------------------------------------------
 * END IF in a BlockIf
 * May be labeled
 */
SXML_TYPE_LIST ast_end_if(
              SXML_TYPE_LIST location,
	      SXML_TYPE_TEXT label) {
  if(label == NULL) {
    return JSON_MAP(
      SXML_LL(
        ast_tag_("end_if"),
	location )
    );
  }
  else {
    return JSON_MAP(
      SXML_LLTL(
        ast_tag_("end_if"),
	location,
	",\n",
        JSON_KQ("label", label) )
    );
  }
}


/* -------------------------------------------------------------------------
 * returns a unary/binary multiple operator
 * - Location of the statement
 * - left operand (if binary)
 * - right operand
 * - first operator
 * - second operator
 */
SXML_TYPE_LIST ast_multiple_operator(
            SXML_TYPE_LIST location,
            SXML_TYPE_LIST left,
            SXML_TYPE_LIST first_operator,
            SXML_TYPE_LIST second_operator,
            SXML_TYPE_LIST right) {

  if (left == NULL){
    return JSON_MAP(
      SXML_LLTLLL(
        ast_tag_("unary_multiple_operator"),
        location,
        ",\n",
        JSON_KU_("first_operator", first_operator),
        JSON_KU_("second_operator", second_operator),
        JSON_KU("operand", right)
      )
    );
  }
  else {
    return JSON_MAP(
      SXML_LLTLLLL(
        ast_tag_("binary_multiple_operator"),
        location,
        ",\n",
        JSON_KU_("first_operator", first_operator),
        JSON_KU_("second_operator", second_operator),
        JSON_KU_("left", left),
        JSON_KU("right", right)
      )
    );
  }
}


/* -------------------------------------------------------------------------
 * expression sign
 */
SXML_TYPE_LIST ast_sign(
        SXML_TYPE_LIST location,
        SXML_TYPE_TEXT sign) {
  
  return JSON_MAP(
    SXML_LTL(
      location,
      ",\n",
      JSON_KQ("sign_value", sign)
    )
  );
}

/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_list(
        SXML_TYPE_LIST tail,
        SXML_TYPE_LIST head) {
  
  return SXML_LTL(tail, ",", head);
}

/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_label_ref(
              SXML_TYPE_LIST location,
              SXML_TYPE_TEXT value) {
  return JSON_MAP(
    SXML_LLTL(
      ast_tag_("label_reference"),
      location,
      ",\n",
      JSON_KQ("label", value)
    )
  );
}


/* -------------------------------------------------------------------------
 * asterisk
 */
SXML_TYPE_LIST ast_asterisk( SXML_TYPE_LIST location) {
  
  return JSON_MAP(
    SXML_LL(
      JSON_KQ_("tag", "asterisk"),
      location
    )
  );
}


/* ------------------------------------------------------------------------
********************************** ESOPE **********************************
------------------------------------------------------------------------ */

SXML_TYPE_LIST ast_segment(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST name,
              SXML_TYPE_LIST variables
              ) {
  return
    SXML_LTLL(
      ast_abstract_statement("segment", location),
      ",\n",
      JSON_KU_("name", name),
      JSON_KU("variables", JSON_ARRAY(variables))
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_pointeur_name(
              SXML_TYPE_LIST pointeur_name,
              SXML_TYPE_LIST segment_name
              ) {
  
  return JSON_MAP(
    SXML_LLL(
      ast_tag_("pointeur"),
      JSON_KU_("pointeur_name", pointeur_name),
      JSON_KU("segment_name", segment_name)
      )
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_pointeur_in_segment(
              SXML_TYPE_LIST pointeur
              ) {
  
  return JSON_MAP( pointeur );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_pointeur_declaration(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST declarators
              ) {
  
  return SXML_LTL(
    ast_abstract_statement("pointeur_declaration", location),
    ",\n",
    JSON_KU("declarators", JSON_ARRAY(declarators))
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_pointeur_variable(
              SXML_TYPE_LIST pointeur_name,
              SXML_TYPE_LIST attribute_name
              ) {
  
  return JSON_MAP(
    SXML_LLL(
      ast_tag_("pointeur_variable"),
      JSON_KU_("pointeur_name", pointeur_name),
      JSON_KU("attribute_name", attribute_name)
      )
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_segment_operation_segini_segact_segdes(
              SXML_TYPE_LIST location,
              SXML_TYPE_TEXT name,
              SXML_TYPE_LIST arguments
              ) {
  
  
    return SXML_LTLL(
      ast_abstract_statement("segment_operation", location),
      ",\n",
      JSON_KQ_("name", name),
      JSON_KU("arguments", JSON_ARRAY(arguments))
    
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_segment_operation_segsup_segadj_segprt(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST name,
              SXML_TYPE_LIST declarators
              ) {
  
  
    return SXML_LTLL(
      ast_abstract_statement("segment_operation", location),
      ",\n",
      JSON_KQ_("name", name->TEXT),
      JSON_KU("declarators", JSON_ARRAY(declarators))
    
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_segment_operation_segina_segind(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST name,
              SXML_TYPE_LIST copy_from,
              SXML_TYPE_LIST copy_to,
              SXML_TYPE_LIST type
              ) {
  
    return SXML_LTLLL(
      ast_abstract_statement(name->TEXT, location),
      ",\n",
      JSON_KU_("copy_from", copy_from),
      JSON_KU_("copy_to", copy_to),
      JSON_KQ("type", type->TEXT)
    
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_segment_array_length(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST name,
              SXML_TYPE_TEXT dimension
              ) {
  
    return JSON_MAP(
      SXML_LTLL(
        ast_abstract_statement("segment_array_length", location),
        ",\n", 
        JSON_KU_("name", name),
        JSON_KQ("dimension", dimension)
      )
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_pointer_to_segment_array_length(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST pointer,
              SXML_TYPE_LIST length
              ) {
  
    return JSON_MAP(
      SXML_LTLL(
        ast_abstract_statement("pointer_to_segment_array_length", location),
        ",\n", 
        JSON_KU_("pointer", pointer),
        JSON_KU("length", length)
      )
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_segment_duplication(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST right,
              SXML_TYPE_LIST left
              ) {
  
    return JSON_MAP(
      SXML_LTLL(
        ast_abstract_statement("segment_duplication", location),
        ",\n", 
        JSON_KU_("right", right),
        JSON_KU("left", left)
      )
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_segment_operation_mode(
              SXML_TYPE_LIST location,
              SXML_TYPE_TEXT name
              ) {
  
    return JSON_MAP(
      SXML_LTL(
        ast_abstract_statement("segment_operation_mode", location),
        ",\n", 
        JSON_KQ("name", name)
      )
  );
}


/* -------------------------------------------------------------------------
 */
SXML_TYPE_LIST ast_segdes_argument_parametrized(
              SXML_TYPE_LIST location,
              SXML_TYPE_LIST segment_name,
              SXML_TYPE_LIST parameters
              ) {
  
    return JSON_MAP(
      SXML_LTLL(
        ast_abstract_statement("segdes_argument_parametrized", location),
        ",\n", 
        JSON_KU_("segment_name", segment_name),
        JSON_KU("parameters", JSON_ARRAY(parameters))
      )
  );
}
