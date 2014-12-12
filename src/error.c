#include "error.h"


const char* error_string(ice_t code) {
    switch (code) {
        case ICE_OK:
            return "Ok";
        case ICE_DOCUMENT_DOES_NOT_EXIST:
            return "Document does not exist";
        case ICE_VERSION_DOES_NOT_EXIST:
            return "Version does not exist";
        case ICE_VERSION_SKIPPED:
            return "Version skipped";
        default:
            return "Unknown";
    }
}
