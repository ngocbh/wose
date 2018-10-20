#!/usr/bin/python
# -*- coding: utf8 -*-

from os import listdir
from os.path import isfile, join, isdir
import sys, codecs, random

sys.stdout = codecs.getwriter('utf_8')(sys.stdout)
sys.stdin = codecs.getreader('utf_8')(sys.stdin)

#create raw corpus for vietnameses from crawled paper
OUTFILE = 'raw-corpus.txt'
OUTDIR = '../../data/'
INDIR = '../../raw-data/corpus/'


def crawl(INDIR):
	for filename in listdir(INDIR):
		filepath = join(INDIR,filename)
		if filename[0] == '.': 
			continue;
		# print(filename)
		if isfile(filepath):
			file = codecs.open(filepath,encoding='utf16', mode = 'r', errors='ignore')
			doc = file.read();
			paragraphs = doc.split(u"\n")
			for paragraph in paragraphs:
				sequences = paragraph.split(u". ")
				for idx, sequence in enumerate(sequences):
					words = sequence.split(u" ")
					endseq = ""
					if ( idx == len(sequences)-1):
						endseq = "\n"
					else :
						endseq = ".\n"
					if len(words) > 2:
						rawCorpus.write(sequence + endseq)
			file.close()
		else:
			if isdir(join(INDIR,filename)): 
				crawl(join(INDIR,filename))

# rawCorpus = codecs.open(join(OUTDIR,OUTFILE),encoding='utf16',mode = 'w', errors='ignore')
# crawl(INDIR)

#build corpus for train_toy.sh
OUTFILE = 'raw-corpus-5K.txt'
OUTDIR = '../../data/'
INDIR = '../../raw-data/corpus/train_full/Khoa hoc'

global nos
nos = 0
def crawl(INDIR):
	global nos
	if nos > 5000:
		return
	for filename in listdir(INDIR):
		filepath = join(INDIR,filename)
		if filename[0] == '.': 
			continue;
		# print(filename)
		if isfile(filepath):
			file = codecs.open(filepath,encoding='utf16', mode = 'r', errors='ignore')
			doc = file.read();
			paragraphs = doc.split(u"\n")
			for paragraph in paragraphs:
				if ( nos > 5000 ):
					return
				nos = nos + 1
				sequences = paragraph.split(u". ")
				for idx, sequence in enumerate(sequences):
					words = sequence.split(u" ")
					endseq = ""
					if ( idx == len(sequences)-1):
						endseq = "\n"
					else :
						endseq = ".\n"
					if len(words) > 2:
						rawCorpus.write(sequence + endseq)
				
			file.close()
		else:
			if isdir(join(INDIR,filename)): 
				crawl(join(INDIR,filename))

# rawCorpus = codecs.open(join(OUTDIR,OUTFILE),encoding='utf16',mode = 'w', errors='ignore')
# crawl(INDIR)

#create labeled word for vietnameses from dictionary and corpus
OUTFILE = 'labeledword.txt'
OUTDIR = '../../data/'
INDIR = '../../data/'
DICTIONARY = 'WordList41K.txt'
CORPUS = 'raw-corpus.txt'


def label():
	dicFile = codecs.open(join(INDIR,DICTIONARY),encoding='utf8',mode='r',errors='ignore')
	dic = dicFile.read()
	words = dic.split(u"\n")
	randomWords = random.sample(words,10000)

	i = 0
	for word in randomWords:
		syllables = word.split(u' ');
		if ( len(syllables) < 2 ):
			continue;
		labeledWord.write(word)
		labeledWord.write("\t1\n")
		i += 1
		if i >= 200:
			break;

	i = 0
	for word in randomWords:
		syllables = word.split(u' ');
		if ( len(syllables) > 2 ):
			continue;
		labeledWord.write(word);
		if i % 2 == 0:
			labeledWord.write(" ")
		else:
			labeledWord.write("\t0\n")
		i += 1
		if i >= 200:
			break;

labeledWord = codecs.open(join(OUTDIR,OUTFILE), encoding='utf8', mode='w',errors='ignore')
label()






