#ifndef IC_CLI_H
#define IC_CLI_H

int cli_insert(int nargs, char** args);
int cli_get(int nargs, char** args);
int cli_update(int nargs, char** args);
int cli_help(int nargs, char** args);
int cli_demo(int nargs, char** args);

#endif