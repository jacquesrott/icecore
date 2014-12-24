#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>

#include "cli.h"

#define ICECORE_OPTIONS "hv"
#define ICECORE_VERSION "0.1.0a"

static struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {"", no_argument, NULL, '\0'},
};

void show_help() {
    printf(
        "icecore %s\n"
        "\n"
        "OPTIONS\n"
        "  --help, -h        Print this help message\n"
        "  --version, -v     Print the icecore version number\n"
        "\n"
        "COMMANDS\n"
        "  insert\n"
        "  update <id>\n"
        "  get <id>\n"
        "  delete <id>\n"
        "\n"
        "For more information about icecore visit https://github.com/jacquesrott/icecore/\n"
    , ICECORE_VERSION);
}

int main(int argc, char** argv) {
    int opt_index;
    int c;
    
    while ((c = getopt_long(argc, argv, ICECORE_OPTIONS, long_options, &opt_index)) != -1) {
        switch (c) {
            case 'h':
                show_help();
                return 0;
            case 'v':
                printf(ICECORE_VERSION "\n");
                return 0;
            case '?':
                if (optopt == 'i' || optopt == 'g') {
                    printf("Option -%c requires an argument\n", optopt);
                }
                else if (isprint(optopt)) {
                    printf("Unknown option -%c\n", optopt);
                }
                else {
                    printf("Unknown option character \\x%x\n", optopt);
                }
                return 1;
        }
    }
    
    if (optind == argc) {
        printf("No command given, try %s --help\n", argv[0]);
        return 1;
    }
    char* command = argv[optind];
    char** args = argv + optind + 1;
    int nargs = argc - optind - 1;
    if (strcmp(command, "insert") == 0) {
        return cli_insert(nargs, args);
    }
    if (strcmp(command, "update") == 0) {
        return cli_update(nargs, args);
    }
    if (strcmp(command, "get") == 0) {
        return cli_get(nargs, args);
    }
    if (strcmp(command, "help") == 0) {
        return cli_help(nargs, args);
    }
    if (strcmp(command, "demo") == 0) {
        return cli_demo(nargs, args);
    }
    printf("Unknown command: %s\n", command);
    return 1;
}


