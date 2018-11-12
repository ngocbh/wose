#!/usr/bin/python
# -*- coding: utf8 -*-

from os import listdir
from os.path import isfile, join, isdir
from math import *
import sys, codecs, random
import pandas, re
import dawg

out = codecs.open('ranking.csv',encoding='utf8',mode='w',errors='ignore');

with codecs.open('ranking.txt',encoding='utf8',mode='r',errors='ignore') as f:
    data = f.read()
    lines = data.split(u'\n')
    for line in lines:
    	cols = line.split(u'\t')
    	if ( len(cols) == 2 ):
    		cols[1] = cols[1].replace(',','.')
    		out.write(cols[1] + ',' + cols[0] + '\n')
    	# out.write(str(cols))

