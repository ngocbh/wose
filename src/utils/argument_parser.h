#ifndef __ARGUMENT_PARSER_H__
#define __ARGUMENT_PARSER_H__

#include "../utils/utils.h"
#include "../utils/parameters.h"


void parseArgument(int argc, char* argv[])
{
	SEGMENTATION_MODEL = "";

    for (int i = 1; i < argc; ++ i) {
        if (!strcmp(argv[i], "--min_sup")) {
            fromString(argv[++ i], MIN_SUP);
        } else if (!strcmp(argv[i], "--max_len")) {
            fromString(argv[++ i], MAX_LEN);
        } else if (!strcmp(argv[i],"--corpus")) {
            fromString(argv[++i ], RAW_CORPUS_FILE);
        } else if (!strcmp(argv[i],"--stopwords")) {
            fromString(argv[++i ], STOPWORD_FILE);
        } else if (!strcmp(argv[i],"--shape")) {
            fromString(argv[++i ], SHAPE_CORPUS_FILE);
        } else if (!strcmp(argv[i],"--label-method")) {
            fromString(argv[++i ], LABEL_METHOD);
        } else if (!strcmp(argv[i],"--label-file")) {
            fromString(argv[++i ], LABEL_FILE);
        } else if (!strcmp(argv[i],"--max-positive")) {
            fromString(argv[++i ], MAX_POSITIVE);
        } else if (!strcmp(argv[i],"--seg-file")) {
            fromString(argv[++i ], TEXT_TO_SEG_FILE);
        } else if (!strcmp(argv[i],"--token-map")) {
            fromString(argv[++i ], TOKEN_MAPPING_FILE);
        } else if (!strcmp(argv[i],"--model")) {
            fromString(argv[++i ], SEGMENTATION_MODEL);
        } else {
            fprintf(stderr, "[Warning] Unknown Parameter: %s\n", argv[i]);
        }
    }
}

#endif