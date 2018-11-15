#!/usr/bin/python
# -*- encoding: utf8 -*-

import re
import argparse
import sys, codecs, os

PUNCTUATIONS = u"//!!¡!(())--{{}}[[]]'::;;<<>>??¿?؟?..,,،‎,؍‎,«<»>—-‘’‚,“”„•-\"=+*"
DELEMITERS= u' | \.|\. |\.\.|\,\,|, | ,|\[|\]|\(|\)|\!|\/|\{|\}|\?|\:|\;|\-|\<|\>|\"|\'|\=|\*|\+'

def nomalize(token):
	separator_after = 0
	separator_before = 0
	while len(token) > 0:
		if PUNCTUATIONS.find(token[0]) != -1 :
			token = token[1:]
			separator_before = 1
		else:
			break
	while len(token) > 0:
		if PUNCTUATIONS.find(token[len(token)-1]) != -1 :
			token = token[:len(token)-1]
			separator_after = 1
		else:
			break
	return token,separator_before,separator_after;

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

def setBit(mask,index,value):
	if value == 1: 
		return mask | ( 1 << index )
	else : 
		return mask & ( ~(1 << index) )

def getBit(mask,index):
	return (mask >> i) & 1

def extractShapeFeatures(token, ptoken, separator_before, separator_after):
	mask = 0
	if (len(token) == 0):
		return 0

	#BIT 0-th bit: First Char Capital?
	if token[0].isupper(): 
		mask = setBit(mask,0,1)
	else:
		mask = setBit(mask,0,0)
	#BIT 1-st bit: All Chars Capital?
	for char in token:
		if not char.isupper():
			mask = setBit(mask,1,0)
			break
		else:
			mask = setBit(mask,1,1)
	#BIT 2-nd bit: any delemiter inside of this token?
	if (ptoken[len(ptoken)-1] == u'-'):
		mask = setBit(mask,2,1)
	else:
		mask = setBit(mask,2,0)
	#BIT 3-rd bit: any punctuation before this token?
	if (separator_before):
		mask = setBit(mask,3,1)
	else: 
		mask = setBit(mask,3,0)
	#BIT 4-rd bit: any punctuation after this token?
	if (separator_after):
		mask = setBit(mask,4,1)
	else: 
		mask = setBit(mask,4,0)
	#BIT 5-rd bit: any " before this token?
	if (ptoken[0] == u'"'):
		mask = setBit(mask,5,1)
	else: 
		mask = setBit(mask,5,0)
	#BIT 6-rd bit: any " after this token?
	if (ptoken[len(ptoken)-1] == u'"'):
		mask = setBit(mask,6,1)
	else: 
		mask = setBit(mask,6,0)
	#BIT 7-rd bit: any ( before this token?
	if (ptoken[0] == u'('):
		mask = setBit(mask,7,1)
	else: 
		mask = setBit(mask,7,0)
	#BIT 8-rd bit: any ) after this token?
	if (ptoken[len(ptoken)-1] == u')'):
		mask = setBit(mask,8,1)
	else: 
		mask = setBit(mask,8,0)
	#BIT 9-rd bit: is digital token
	if (isDigital(token) == 1):
		mask = setBit(mask,9,1)
	else: 
		mask = setBit(mask,9,0)
	

	return mask

def map(args):
	words = {}
	words_additional = {}
	nWord = 0
	punc_map = {}

	if ( not os.path.isfile(args.tm) ):
		with codecs.open(args.tm,encoding='utf8',mode='w',errors='ignore') as tmout:
			tmout.write('0\t0\n1\t1\n')


	#Load token mapping file
	with codecs.open(args.tm,encoding='utf8',mode='r',errors='ignore') as tminp:
		data = tminp.read()
		lines = data.split('\n')
		for line in lines: 
			word_map = line.split('\t')
			if ( len(word_map) < 2 ):
				continue
			if ( word_map[0] != '' and word_map[1] != '' ):
				words[word_map[0]] = int(word_map[1])
				nWord = max(nWord,int(word_map[1]))

	#Load punctuation mapping file
	with codecs.open(args.pm,encoding='utf8',mode='r',errors='ignore') as pminp:
		data = pminp.read()
		lines = data.split('\n')
		for line in lines:
			puncs = line.split('\t')
			punc_map[puncs[0]] = puncs[1]

	path,fileBaseName = getFileBaseName(args.input,args.outdir,args.outbase)
	
	wpFile = path + 'smooth_' + fileBaseName + '.txt'
	tFile = path + 'tokenized_' + fileBaseName + '.txt'
	spFile = path + 'shape_' + fileBaseName + '.txt'

	if (args.extract == 'yes' ):
		spout = codecs.open(spFile,encoding='utf8',mode='w',errors='ignore')

	with codecs.open(wpFile,encoding='utf8',mode='w',errors='ignore') as wpout:
		with codecs.open(tFile,encoding='utf8',mode='w',errors='ignore') as tcout:
			with codecs.open(args.input,encoding='utf8',mode='r',errors='ignore') as inp:
				data = inp.read();
				for punc_s,punc_t in punc_map.items():
					data = data.replace(punc_s,punc_t)

				sentences = data.split('\n')
				for sentence in sentences:
					ptokens = re.split(' ',sentence)
					for index,ptoken in enumerate(ptokens):
						ptoken,separator_before,separator_after = nomalize(ptoken)
						tokens = re.split(DELEMITERS,ptoken)
						for token in tokens: 
							token,_,_ = nomalize(token)
							if ( len(token) == 0 ):
								continue
							if ( args.extract == 'yes' ):
								spout.write(str(extractShapeFeatures(token,ptoken,(separator_before == 1),((separator_after==1) or (index == len(ptokens))) )) + ' ')
							lower_token = token.lower()
							if ( lower_token == '' ):
								continue
							wpout.write(token + ' ')
							if lower_token not in words:
								nWord += 1
								words[lower_token] = nWord
								words_additional[lower_token] = nWord
							tcout.write(str(words[lower_token]) + u' ')
					wpout.write('\n')
					tcout.write('\n')
					if ( args.extract == 'yes' ):
						spout.write('\n')

	with codecs.open(args.tm,encoding='utf8',mode='a',errors='ignore') as tmout:
		for word,token in words_additional.items():
			tmout.write(word + '\t' + str(token) + '\n')

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