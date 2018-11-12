#!/usr/bin/python
# -*- coding: utf8 -*-

from os import listdir
from os.path import isfile, join, isdir
from math import *
import sys, codecs, random
import re
import dawg
import argparse

def main(args):
    inDictPos = [0 for x in range(11)]
    outDictPos = [0 for x in range(11)]
    inDictNeg = [0 for x in range(11)]
    outDictNeg = [0 for x in range(11)]
    inDict = [0 for x in range(11)]
    outDict = [0 for x in range(11)]
    posPre = [0 for x in range(11)]
    negPre = [0 for x in range(11)]
    words = []

    with codecs.open(args.dict,encoding='utf8',mode='r',errors='ignore') as f:
        data = f.read()
        words = data.split('\n');

    out = codecs.open(args.output,encoding='utf8',mode='w+',errors='ignore');
    # build dafsa (automaton) for dictionary
    dafsa = dawg.DAWG(words)

    with codecs.open(args.rank,encoding='utf8',mode='r',errors='ignore') as inp:
        data = inp.read()
        rows = data.split('\n')
        for row in rows:
            pair = row.split('\t')
            if ( len(pair) < 2 ):
                continue
            rank = float(pair[0])
            word = pair[1]
            for threshold in range(11):
                negPre[threshold] += (rank < threshold/10.0) 
                posPre[threshold] += (rank >= threshold/10.0)

            word = str(word)
            if word in dafsa:
                for threshold in range(11):
                    inDictNeg[threshold] += (rank < threshold/10.0)
                    inDictPos[threshold] += (rank >= threshold/10.0)
                    inDict[threshold] += 1
            else:
                for threshold in range(11):
                    outDictNeg[threshold] += (rank < threshold/10.0)
                    outDictPos[threshold] += (rank >= threshold/10.0)
                    outDict[threshold] += 1

   
    out.write("======================New Evalution================\n")
    for threshold in range(11):
        out.write("==========>threshold = %.1f =========\n" % float(threshold/10.0))
        out.write("inDictPos = %d \n" % inDictPos[threshold])
        # out.write("inDictNeg = %d \n" % inDictNeg[threshold])
        out.write("inDict = %d \n" % inDict[threshold])
        # out.write("outDictPos = %d \n" % outDictPos[threshold])
        # out.write("outDictNeg = %d \n" % outDictNeg[threshold])
        # out.write("outDict = %d \n" % outDict[threshold])
        out.write("positive prediction = %d \n" % posPre[threshold])
        # out.write("negative prediction = %d \n" % negPre[threshold])
        if posPre[threshold] == 0:
            out.write("precision = 1.0\n")
        else:
            out.write("precision = %.5f \n" % (float)(inDictPos[threshold]/posPre[threshold]))
        if inDict[threshold] == 0:
            out.write("recall = 1.0\n")
        else:
            out.write("recall = %.5f \n" % (float)(inDictPos[threshold]/inDict[threshold]))
        out.write("\n")
        
def parse_arguments(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--dict', type=str, help='dictionary',default='dictionary.txt')
    parser.add_argument('--rank', type=str, help='ranking file',default='ranking.txt')
    parser.add_argument('--output', type=str, help='output file',default='ranking_log.txt')
    return parser.parse_args(argv)

if __name__ == "__main__":
    main(parse_arguments(sys.argv[1:]))