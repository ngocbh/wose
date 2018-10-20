import re
import sys, codecs
from math import *

def main(argv):
    rawTextInput = 'rawText.txt'
    stopwordsOutput = 'stopwordsFromText.txt'
    argc = len(argv)
    for i in xrange(argc):
        if argv[i] == "-raw" and i + 1 < argc:
            rawTextInput = argv[i + 1]
        elif argv[i] == "-o" and i + 1 < argc:
            stopwordsOutput = argv[i + 1]
    
    inDocs = {}
    occurrence = {}
    docsN = 0
    tokensN = 0
    for line in codecs.open(rawTextInput,encoding='utf8',mode='r',errors='ignore'):
        docsN += 1
        
        inside = 0
        chars = []
        tokens = {}
        for ch in line:
            if ch == '(':
                inside += 1
            elif ch == ')':
                inside -= 1
            elif inside == 0:
                if ch.isalpha():
                    chars.append(ch.lower())
                elif ch == '\'':
                    chars.append(ch)
                else:
                    if len(chars) > 0:
                        token = ''.join(chars)
                        tokensN += 1
                        if token in occurrence:
                            occurrence[token] += 1
                        else:
                            occurrence[token] = 1
                        tokens[token] = True
                    chars = []
        if len(chars) > 0:
            token = ''.join(chars)
            tokensN += 1
            if token in occurrence:
                occurrence[token] += 1
            else:
                occurrence[token] = 1
            tokens[token] = True
            chars = []
        for token in tokens:
            if token in inDocs:
                inDocs[token] += 1
            else:
                inDocs[token] = 1

    #print 'tokens = ', tokensN
    #print 'docs = ', docsN

    rank = []
    for token, occur in occurrence.items():
        tf = occur / float(tokensN)
        idf = max(log(docsN / float(inDocs[token])), 1e-10)
        rank.append((token, tf * idf))
    sorted_x = sorted(rank, key=lambda x: -x[1])
    
    out = codecs.open(stopwordsOutput,encoding='utf8',mode='w',errors='ignore')
    for token, key in sorted_x:
        out.write(token + u',' + str(key) + u'\n');
    out.close()

if __name__ == "__main__":
    main(sys.argv[1 : ])
