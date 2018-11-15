#!/usr/bin/python
# -*- encoding: utf8 -*-

import re
import argparse
import sys, codecs, os
import operator

PUNCTUATIONS = u"//!!¡!(())--{{}}[[]]'::;;<<>>??¿?؟?..,,،‎,؍‎,«<»>—-‘’‚,“”„•-\"=+*"
DELEMITERS= u' | \.|\. |\.\.|\,\,|, | ,|\[|\]|\(|\)|\!|\/|\{|\}|\?|\:|\;|\-|\<|\>|\"|\'|\=|\*|\+'

def isDigital(token):
	for char in token:
		if ( char >= u'0' and char <= u'9' ):
			return 1;
	return 0;

def getFileBaseName(input,outdir,outbase):
	if ( outdir[len(outdir)-1] != '/' ):
		outdir += u'/'
	if outbase != u'':
		return outdir,outbase
	lastDot = input.rfind(u'.')
	if lastDot == -1:
		lastDot = len(args.input)
	lastDash = input.rfind(u'/')
	return input[:lastDash+1],input[lastDash+1:lastDot]

def map(args):
	words = {}

	with codecs.open(args.pm,encoding='utf8',mode='r',errors='ignore') as inp:
		data = inp.read()
		lines = data.split(u'\n')
		for line in lines:
			word = line.lower()
			if ( word not in words ):
				words[word] = 1

	with codecs.open(args.input,encoding='utf8',mode='r',errors='ignore') as inp:
		data = inp.read()
		lines = data.split(u'\n')
		for line in lines:
			size = len(line)
			while ( len(line) >= 1 and line[size-1] == u' '):
				size -= 1
			word = line[:size].lower()
			if ( word not in words ):
				words[word] = 1

	sorted_words = sorted(words.items(), key=operator.itemgetter(0))

	with codecs.open(args.outbase,encoding='utf8',mode='w',errors='ignore') as out:
		for word,value in sorted_words:
			out.write(word + '\n')
	
def translate(args):
	return

def main(args):
	if args.mode == 'map':
		map(args)
	elif args.mode == 'translate':
		translate(args)
	else:
		print("cannot understand mode, type: -h")

def parse_arguments(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--mode', type=str, help='mode: map, translate',default='init')
    parser.add_argument('--input', type=str, help='input file',default='data/raw-corpus-5K.txt')
    parser.add_argument('--pm', type=str, help='punctuation mapping file',default='data/punctuation_mapping.txt')
    parser.add_argument('--tf', type=str, help='tokenized file',default='tmp/tokenized_corpus.txt')
    parser.add_argument('--tm', type=str, help='token mapping',default='tmp/token_mapping.txt')
    parser.add_argument('--outdir', type=str, help='output directory',default='tmp/')
    parser.add_argument('--outbase', type=str, help='output file base name',default='')
    parser.add_argument('--extract', type=str,help='extract shape feature yes/no',default='no')
    return parser.parse_args(argv)

if __name__ == "__main__":
	main(parse_arguments(sys.argv[1:]))