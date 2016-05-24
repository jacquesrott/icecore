#ifndef IC_ERROR_H
#define IC_ERROR_H


typedef enum {
    ICE_OK = 0,
    ICE_DOCUMENT_DOES_NOT_EXIST,
    ICE_VERSION_DOES_NOT_EXIST,
    ICE_VERSION_SKIPPED,
    ICE_SERVER_ERROR,
} ice_t;

const char* error_string(ice_t code);

#endif
