#!/bin/bash
make
if [ $? -ne 0 ]
then
	echo "Error???"
	exit
fi

green="---"
reset="---"
PYTHON=python3
PREPROCESSING=1

MODEL=${MODEL:- "model"}
RAW_TEXT=${RAW_TEXT:-data/test.txt}
NAMED_TEXT=${NAMED_TEXT:-tmp/named_test.txt}
TEXT_TO_SEG=${TEXT_TO_SEG:-tmp/tokenized_test.txt}
SMOOTH_TEXT=${SHAPE_TEXT:-tmp/smooth_test.txt}
PUNCTUATION_MAPPING="data/punctuation_mapping.txt"

SEGMENTATION_MODEL=${MODEL}/segmentation.model
TOKEN_MAPPING=${MODEL}/token_mapping.txt

if [ ${PREPROCESSING} -eq 1 ] 
then 
	echo ${green}===BEGIN===Mapping text===${reset}
	time ${PYTHON} src/preprocessing/token_mapping.py --mode map \
												 --input ${RAW_TEXT} \
												 --pm ${PUNCTUATION_MAPPING} \
												 --tm ${TOKEN_MAPPING} \
												 --outdir tmp/ \
												 --outbase test

	echo ${green}===END=====Mapping text===${reset}
fi

./bin/wose_segment --seg-file $TEXT_TO_SEG \
				   --token-map $TOKEN_MAPPING \
				   --model $SEGMENTATION_MODEL 

${PYTHON} src/preprocessing/token_mapping.py --mode translate \
											 --input ${NAMED_TEXT} \
											 --tf tmp/tokenized_segmented_sentences.txt \
											 --smooth ${SMOOTH_TEXT} \
											 --outdir model/ \
											 --outbase test

