#ifndef IC_ERROR_H
#define IC_ERROR_H

#include <stdio.h>

typedef enum {
    ICE_OK = 0,
    ICE_DOCUMENT_DOES_NOT_EXIST,
    ICE_VERSION_DOES_NOT_EXIST,
    ICE_VERSION_SKIPPED,
    ICE_SERVER_ERROR,
} ice_t;

const char* error_string(ice_t code);

#define check(e) if(e == NULL) goto error;
#define check_negative(e) if(e == -1) goto error;
#define check_ok(e) if(e != ICE_OK) goto error;

#define pcheck(e, s) if(e == NULL) { perror(s); goto error; }
#define pcheck_negative(e, s) if(e == -1) { perror(s); goto error; }

#endif
