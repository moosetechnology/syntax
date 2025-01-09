# Syntax 

Syntax is a compiler generator from the Inria team [Convecs](https://convecs.inria.fr).

This repository is a fork of the official SVN repository of [Syntax](https://sourcesup.renater.fr/projects/syntax/) with a particular focus on the Fortran grammar.

This work was possible thanks to the valuable help of [Hubert Garavel](https://convecs.inria.fr/people/Hubert.Garavel/) and Pierre Boullier of the [Convecs](https://convecs.inria.fr) Inria team.

The official version of Syntax repository can be checked out with SVN:
``` bash
svn checkout https://subversion.renater.fr/anonscm/svn/syntax/
```

## Compilation

To compile Syntax run
``` bash
sh trunk/etc/bin/sxmake
```

To compile Fortran-77 grammar:
``` bash
sh trunk/etc/bin/sxmake j77
```

To compile Esope grammar
``` bash
sh trunk/etc/bin/sxmake esope77
```

## Cleaning

To remove temporary Syntax files:

``` bash
sh trunk/etc/bin/sxmake full-clean
```

## Running Syntax

- Run Syntax on a Fortran-77 test file with json ouput (here on `examples/j77/test/nist/FM001.FOR` file)

``` bash
cd syntax/trunk/examples/j77
sh bin/f77.out -json test/nist/FM001.FOR

#for 64arch architecture use:
sh bin.arm64/f77.out -json test/nist/FM001.FOR
```

- Run Syntax on a Esope test file with json ouput

``` bash
cd syntax/trunk/examples/esope77
sh bin/f77.out -json test/nist/FM001.FOR

#for 64arch architecture use:
sh bin.arm64/f77.out -json test/nist/FM001.FOR
```
 
## Testing Fortran-77 output

Look into `trunk/examples/j77/bin` for testing tools
- `tests.sh` runs syntax against the unit-tests and compare generated results with the ones from `test-references` folder.
  - with the `-b` flag it executes also heavy NIST tests
  - with the `-v` flag it shows the diff between the test and its reference
- `test-oracle.sh` - generate new test references. It needs to be used after you change a test or generate a new one
- `metrics.sh` shows the % of rules covered

# Parsing Esope on Framatome infrastructure

The branch `esope-framatome` has been created to run Syntax on Framatome infrustructure:
- it supresses call to SVN in the makefile
- it contains scripts `compile-syntax.sh` and `run-test-esope.sh` to compile Syntax and to run the necessary tests

To get results of Esope parsing:
1. Prepare the configuration file (e.g `project.conf`) that contains the name of the project, project root directory path, sources and include diredctories paths, the list of file extensions to parse and the preprocessing options:

```
project_name="project1"
project_root_dir="/absolute/path/to/project1"
g_inc_dirs="relative/path/to/dir_inc1 relative/path/to/dir_inc2 relative/path/to/dir_inc2"
src_dirs="relative/path/to/dir1 relative/path/to/dir2 relative/path/to/dir3 relative/path/to/dirN"
extensions="f fc F e ec F inc"
preprocessing_options="-P traditional -I /path/to/include1 -I /path/to/include2"
```

Note: the extensions are case-insensitive. 
   
2. Run the `compile-syntax.sh` from the syntax root folder and then the `run-test-esope.sh` script.

``` bash
cd syntax
sh compile-syntax.sh
sh run-test-esope.sh
```
The last script will create in the root foolder of Syntax the `test-results` folder with two files:
- `${project_name}_results.txt` which contains information about the errors occured during the parsing - for each error it stores a filename, line of code with error +1 line  above and below and the error message.
- `${project_name}_stats.txt` which contains the statistical information - total amount of files being parsed, amount of files parsed without errors, percentage of successfully parsed files.
 



