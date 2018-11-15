#!/bin/bash

export PYTHON=python3
export PYPY=python3

DICTIONARY='data/WordList76K.txt'
RANKING_MULTI='experiment/server/tmp/iter_0_quality_multi-words.txt'
RANKING_UNIGRAMS='experiment/server/tmp/iter_0_quality_unigrams.txt'
RANKING_SALIENT='experiment/server/tmp/iter_0_quality_salient.txt'

mkdir -p experiment

${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_MULTI} --threshold 0.5 --outbase "evaluation_multi-words.txt" --outdir "experiment/"
${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_UNIGRAMS} --threshold 0.5 --outbase "evaluation_unigrams.txt" --outdir "experiment/"
${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_SALIENT} --threshold 0.5 --outbase "evaluation_salient.txt" --outdir "experiment/"
