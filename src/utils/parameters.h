#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "../utils/utils.h"

// [NOTE!!] If you have a really large input, please uncomment the following define.
// #define LARGE

#ifdef LARGE
    typedef long long TOTAL_TOKENS_TYPE;
    typedef long long PATTERN_ID_TYPE;
    typedef long long TOKEN_ID_TYPE;
    typedef long long INDEX_TYPE; // sentence id
    typedef int POSITION_INDEX_TYPE; // position inside a sentence
#else
    typedef int TOTAL_TOKENS_TYPE;
    typedef int PATTERN_ID_TYPE;
    typedef int TOKEN_ID_TYPE;
    typedef int INDEX_TYPE; // sentence id
    typedef short int POSITION_INDEX_TYPE; // position inside a sentence
#endif

typedef unsigned long long ULL;
typedef map<string,int> MAP_S2I;

string TEXT_TO_SEG_FILE = "data/test.txt";
string SEGMENTATION_MODEL = "model/segmentation.model";

int MIN_SUP = 30;
int MAX_LEN = 6;
int MAX_POSITIVE = -1;
int DISCARD = 0.05;
int ITERATIONS = 2;
int NEG_POOL_SIZE = 3000;
int POS_POOL_SIZE = -1;
float SEGMENT_MULTI_WORD_QUALITY_THRESHOLD = 0.8;
float SEGMENT_SINGLE_WORD_QUALITY_THRESHOLD = 0;

bool INTERMEDIATE = true;

string RAW_CORPUS_FILE = "tmp/tokenized_corpus.txt";
string STOPWORD_FILE = "tmp/tokenized_stopwords.txt";
string SHAPE_CORPUS_FILE = "tmp/shape_corpus.txt";
string ALL_WORD_FILE = "tmp/tokenized_all_words.txt";
string QUALITY_WORD_FILE = "tmp/tokenized_quality_words.txt";
string TOKEN_MAPPING_FILE = "tmp/token_mapping.txt";
string LABEL_FILE = "tmp/tokenized_labeled_words.txt";
string LABEL_METHOD = ""; // E: Expert label , D: Distant training , P: Positive, N: Negative 

#endif 
