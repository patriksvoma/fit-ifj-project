# IFJ project - IFJ26 Imperative Language Compiler (FIT VUT 2026/27)
This is an implementation of a IFJ26 to IFJcode26 compiler built for the Formal Languages and Compilers class.

## Make targets
- build - Builds the executable
- check - Runs a formatting check and static analysis
- build-check - Builds the executable only after passing the check target
- test - Runs unit tests
- e2e-test - Runs end-to-end compiler tests
- interpret-test - Runs generated IFJcode26 through an external interpreter
- format - Runs a formatter on the code
- pack - Packs the archive for submission, then sends it to merlin and tests it against the official `is_it_ok.sh` script
- clean - Removes any files created during compilation

## Authors
- Patrik Švoma
- Petr Marek
- Jakub Kvasnička
- Jan Šerák
