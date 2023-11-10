# Syntax 

Syntax is a compiler generator from the Inria team [Convecs](https://convecs.inria.fr).

This repository is a fork of the official SVN repository of [Syntax](https://sourcesup.renater.fr/projects/syntax/) with a particular focus on the Fortran grammar.

This work was possible thanks to the valuable help of [Hubert Garavel](https://convecs.inria.fr/people/Hubert.Garavel/) and Pierre Boullier of the [Convecs](https://convecs.inria.fr) Inria team.

## Getting it

You can clone this repository, or get the official version from the SVN repository of Syntax:
``` bash
svn checkout https://subversion.renater.fr/anonscm/svn/syntax/
```

## To compile it

To compile everything (all grammars in `syntax/trunk/examples`)
``` bash
cd syntax/trunk
sh etc/bin/sxmake
```

There are two versions of the Fortran grammar (in `trunk/examples` directory):
- f77 -- the bare grammar
- j77 -- Fortran grammar with semantic actions to output an AST in JSON format

- Compile the Fortran grammar only
``` bash
cd syntax/trunk
sh etc/bin/sxmake f77 
sh etc/bin/sxmake j77 
```

- Clean (if something is messed-up)
``` bash
cd syntax/trunk
sh etc/bin/sxmake full-clean
```

## To run it

- Run Syntax on a test file with json ouput (here on `examples/j77/test/nist/FM001.FOR` file)
``` bash
cd syntax/trunk/examples/j77
sh bin/f77.out -json test/nist/FM001.FOR
```

## To test it
Look into `trunk/examples/j77/bin` for testing tools
- `tests.sh` runs syntax against the unit-tests and compare generated results with the ones from `test-references` folder.
  - with the `-b` flag it executes also heavy NIST tests
  - with the `-v` flag it shows the diff between the test and its reference
- `test-oracle.sh` - generate new test references. It needs to be used after you change a test or generate a new one
- `metrics.sh` shows the % of rules covered

## Syntax F77 essential files

- `f77.semc` -- Grammar in SEMC format
- `trunk/incl/sxml.h` -- for adding new multi-argument functions used in semantic actions for JSON output
- `syntax/trunk/incl/sxunix.h` -- for functions like `sxcurrent_parsed_file_name`, `sxcurrent_parsed_column`, etc
- `f77_main.c` and `f77_analyse.c` -- for parsing process 
