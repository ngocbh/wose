#!/usr/bin/python
# -*- coding: utf8 -*-

from os import listdir
from os.path import isfile, join, isdir
from math import *
import sys, codecs, random
import re
import dawg
import argparse
# to print utf8
# sys.stdout.buffer.write(word.encode('utf8') + b'\n')

def jointPath(outdir,outbase):
    if ( outdir[len(outdir)-1] != '/' ):
        outdir += '/'
    return outdir + outbase

def evaluator1(args, out):
	out.write("-----=====EVALUETOR 1======-----\n")
	with codecs.open(args.input,encoding='utf8',mode='r') as inp:
		with codecs.open(args.base,encoding='utf8',mode='r') as baseIn:
			dataIn = inp.read()
			dataBI = baseIn.read()

			sentencesIn = dataIn.split('\n')
			sentencesBI = dataBI.split('\n')

			crrSen = 0
			cntSen = 0
			crrSpace = 0
			cntSpace = 0
			truePositive = 0
			positivePrediction = 0
			positiveLabel = 0

			for i in range(len(sentencesIn)):
				sentenceIn = sentencesIn[i]
				sentenceBI = sentencesBI[i]
				if ( i + 1 >= len(sentencesBI) ):
					break

				cntSpaceInSen = 0
				crrSpaceInSen = 0

				for j in range(len(sentenceIn)):
					if (sentenceIn[j] == ' ' or sentenceIn[j] == '_'):
						assert (sentenceBI[j] == ' ' or sentenceBI[j] == '_'), "error to match"
						cntSpaceInSen += 1
						if ( sentenceIn[j] == sentenceBI[j] ):
							crrSpaceInSen += 1

					if (sentenceIn[j] == '_'):
						positivePrediction += 1
						if (sentenceBI[j] == '_'):
							truePositive += 1

					if (sentenceBI[j] == '_'):
						positiveLabel += 1
				
				cntSpace += cntSpaceInSen
				crrSpace += crrSpaceInSen
				cntSen += 1
				if cntSpaceInSen == crrSpaceInSen:
					crrSen += 1

			out.write("correct space: " + str(crrSpace) + '\n')
			out.write("count space: " + str(cntSpace) + '\n')
			out.write("prediction space accuracy: " + str(crrSpace*100/float(cntSpace)) + '\n')
			out.write("\n")
			out.write("correct sentences: " + str(crrSen) + '\n')
			out.write("count sentences: " + str(cntSen) + '\n')
			out.write("prediction sentences accuracy: " + str(crrSen*100/float(cntSen)) + '\n')
			out.write("\n")
			precision = truePositive/float(positivePrediction + (positivePrediction == 0))
			recall = truePositive/float(positiveLabel + (positiveLabel == 0))
			f1score = 2*precision*recall/(precision+recall+(precision == 0 and recall == 0))
			out.write("true positive: " + str(truePositive) + '\n')
			out.write("positive prediction: " + str(positivePrediction) + '\n')
			out.write("positive label: " + str(positiveLabel) + '\n')
			out.write("precision: " + str(precision*100) + '\n')
			out.write("recall: " + str(recall*100) + '\n')
			out.write("f1 score: " + str(f1score*100) + '\n')
			


def main(args):
	out = codecs.open(jointPath(args.outdir,args.outbase),encoding='utf8',mode='w')
	evaluator1(args,out)

        
def parse_arguments(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', type=str, help='your prediction',default='ranking_log.txt')
    parser.add_argument('--base', type=str, help='your output data set',default='ranking_log.txt')
    parser.add_argument('--outbase', type=str, help='output base file',default='evalution_segmentation.txt')
    parser.add_argument('--outdir', type=str, help='output dir',default='experiment/')
    return parser.parse_args(argv)

if __name__ == "__main__":
    main(parse_arguments(sys.argv[1:]))