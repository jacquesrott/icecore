#include <stdlib.h>

#include "document.h"


const unsigned int MAX_DOCUMENTS = 1000;


void document_delete(ic_Document* document) {
    free(document);
}


ic_DocumentIndex* document_index_create() {
    ic_DocumentIndex* document_index = malloc(sizeof(*document_index));

    document_index->documents = calloc(MAX_DOCUMENTS, sizeof(*document_index->documents));

    return document_index;
}

size_t document_index_documents_size(ic_DocumentIndex* document_index) {
    return sizeof(document_index->documents) / sizeof(*document_index->documents);
}

void document_index_delete(ic_DocumentIndex* document_index) {
    for (int i = document_index_documents_size(document_index); i >= 0 ; --i) {
        document_delete(document_index->documents[i]);
    }
    free(document_index->documents);
    free(document_index);
}
