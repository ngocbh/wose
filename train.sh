#!/bin/bash
make
if [ $? -ne 0 ]
then
    echo "Error???"
    exit
fi

green="---"
reset="---"

PYTHON=python
PREPROCESSING=0

LABEL_METHOD="DPDN"
MAX_POSITIVE=-1

RAW_CORPUS="data/raw-corpus-5K.txt"
PUNCTUATION_MAPPING="data/punctuation_mapping.txt"
STOPWORDS="data/stopwords.txt"
ALL_WORDS=data/WordList41K.txt
QUALITY_WORDS=data/WordList32K.txt

TOKEN_MAPPING="tmp/token_mapping.txt"
SHAPE_CORPUS="tmp/shape_corpus.txt"
TOKENIZED_CORPUS="tmp/tokenized_corpus.txt"
TOKENIZED_STOPWORD="tmp/tokenized_stopwords.txt"
TOKENIZED_QUALITY_WORD="tmp/tokenized_quality_words.txt"
TOKENIZED_ALL_WORD="tmp/tokenized_all_words.txt"

if [ ${PREPROCESSING} -eq 1 ] 
then 
	rm -r tmp
	mkdir -p tmp
	echo ${green}===BEGIN===Mapping raw corpus===${reset}
	time ${PYTHON} src/preprocessing/token_mapping.py --mode map \
												 --input ${RAW_CORPUS} \
												 --pm ${PUNCTUATION_MAPPING} \
												 --tm ${TOKEN_MAPPING} \
												 --outdir tmp/ \
												 --outbase corpus \
												 --extract yes

	echo ${green}===END=====Mapping raw corpus===${reset}
	echo ""
	echo ${green}===BEGIN===Mapping stopwords===${reset}
	time ${PYTHON} src/preprocessing/token_mapping.py --mode map \
												 --input ${STOPWORDS} \
												 --pm ${PUNCTUATION_MAPPING} \
												 --tm ${TOKEN_MAPPING} \
												 --outbase stopwords \
												 --outdir tmp/
	echo ${green}===END===Mapping stopwords===${reset}
	echo ""
	echo ${green}===BEGIN===Mapping quality words===${reset}
	time ${PYTHON} src/preprocessing/token_mapping.py --mode map \
												 --input ${QUALITY_WORDS} \
												 --pm ${PUNCTUATION_MAPPING} \
												 --tm ${TOKEN_MAPPING} \
												 --outbase quality_words \
												 --outdir tmp/
	echo ${green}===END===Mapping quality words===${reset}
	echo ""
	echo ${green}===BEGIN===Mapping all words===${reset}
	time ${PYTHON} src/preprocessing/token_mapping.py --mode map \
												 --input ${ALL_WORDS} \
												 --pm ${PUNCTUATION_MAPPING} \
												 --tm ${TOKEN_MAPPING} \
												 --outbase all_words \
												 --outdir tmp/
	echo ${green}===END===Mapping all words===${reset}
	echo ""
fi
./bin/wose_train --corpus ${TOKENIZED_CORPUS} \
				 --stopwords ${TOKENIZED_STOPWORD} \
				 --shape ${SHAPE_CORPUS} \
				 --max-positive ${MAX_POSITIVE} \
				 --label-method ${LABEL_METHOD}
