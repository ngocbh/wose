#!/bin/bash

make
./train_toy.sh 
exit

export PYTHON=python
export PYPY=python
if type "pypy" > /dev/null; then
	export PYPY=pypy
fi

RAW_TEXT='data/raw-corpus-5K.txt'
AUTO_LABEL=0
WORDNET_NOUN=0
DATA_LABEL='data/labeledword-50.txt'
KNOWLEDGE_BASE='data/wiki_labels_quality.txt'
KNOWLEDGE_BASE_LARGE='data/wiki_labels_all.txt'

STOPWORD_LIST='data/stopwords.txt'
SUPPORT_THRESHOLD=10

OMP_NUM_THREADS=4
DISCARD_RATIO=0.00
MAX_ITERATION=5

NEED_UNIGRAM=0
ALPHA=0.85

./bin/predict_quality results/feature_table_0.csv ${DATA_LABEL} results/ranking.csv outsideSentence,log_occur_feature,constant,frequency 0 TRAIN results/random_forest_0.model
