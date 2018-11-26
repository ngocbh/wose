#!/bin/bash

export PYTHON=python3
export PYPY=python3

DICTIONARY='data/WordList76K.txt'
RANKING_MULTI='experiment/server/tmp/final_quality_multi-words.txt'
RANKING_UNIGRAMS='experiment/server/tmp/final_quality_unigrams.txt'
RANKING_SALIENT='experiment/server/tmp/final_quality_salient.txt'
EVALUATE_RANKING=0
EVALUATE_SEGMENTATION=1
mkdir -p experiment

if [ $EVALUATE_RANKING -eq 1 ]
then 
	${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_MULTI} --threshold 0.5 --outbase "evaluation_multi-words.txt" --outdir "experiment/"
	${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_UNIGRAMS} --threshold 0.5 --outbase "evaluation_unigrams.txt" --outdir "experiment/"
	${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_SALIENT} --threshold 0.5 --outbase "evaluation_salient.txt" --outdir "experiment/"
fi

if [ $EVALUATE_SEGMENTATION -eq 1 ]
then 
	./segment.sh
	${PYPY} tools/evaluations/evaluate_segmentation.py --input model/segmented_test.txt --base data/segmented-test.txt --outdir experiment/ --outbase evaluation_segmentation.txt
fi
