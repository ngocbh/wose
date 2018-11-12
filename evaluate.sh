#!/bin/bash

export PYTHON=python3
export PYPY=python3

DICTIONARY='data/WordList41K.txt'
RANKING_MULTI='tmp/iter_0_quality_multi-words.txt'
RANKING_UNIGRAMS='tmp/iter_0_quality_unigrams.txt'
RANKING_SALIENT='tmp/iter_0_quality_salient.txt'

mkdir -p experiment

${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_MULTI} --output "experiment/evaluation_multi-words.txt"
${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_UNIGRAMS} --output "experiment/evaluation_unigrams.txt"
${PYPY} tools/evaluations/evaluate_ranking.py --dict ${DICTIONARY} --rank ${RANKING_SALIENT} --output "experiment/evaluation_salient.txt"
