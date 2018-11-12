#ifndef __DOCUMENTS_H__
#define __DOCUMENTS_H__

#include "../utils/utils.h"

namespace Documents
{
    const int FIRST_CAPITAL = 0;
    const int ALL_CAPITAL = 1;
    const int DASH_AFTER = 2;
    const int SEPARATOR_BEFORE = 3;
    const int SEPARATOR_AFTER = 4;
    const int QUOTE_BEFORE = 5;
    const int QUOTE_AFTER = 6;
    const int PARENTHESIS_BEFORE = 7;
    const int PARENTHESIS_AFTER = 8;

    struct WordTokenInfo {
        int mask;
        // 8 types:
        //    #BIT 0-th bit: First Char Capital?
        //    #BIT 1-st bit: All Chars Capital?
        //    #BIT 2-nd bit: any delemiter after or before of this token?
        //    #BIT 3-rd bit: any punctuation before this token?
        //    #BIT 4-rd bit: any punctuation after this token?
        //    #BIT 5-rd bit: any " before this token?
        //    #BIT 6-rd bit: any " after this token?
        //    #BIT 7-rd bit: any ( before this token?
        //    #BIT 8-rd bit: any ) after this token?
        //    #BIT 9-rd bit: is digital token?
        WordTokenInfo() {
            mask = 0;
        }

        inline void turnOn(int bit) {
            mask |= 1 << bit;
        }

        inline bool get(int bit) const {
            return mask >> bit & 1;
        }
    };
// === global variables ===
    TOTAL_TOKENS_TYPE totalWordTokens = 0;

    TOKEN_ID_TYPE maxTokenID = 0;

    float* idf; // 0 .. maxTokenID
    TOKEN_ID_TYPE* wordTokens; // 0 .. totalWordTokens - 1

    // 0 .. ((totalWordTokens * 7 + 31) / 32) - 1
    WordTokenInfo* wordTokenInfo;
    bool* isDigital; // 0..maxTokenID

    vector<pair<TOTAL_TOKENS_TYPE, TOTAL_TOKENS_TYPE>> sentences;

    set<TOKEN_ID_TYPE> stopwords;

    set<string> separatePunc = {",", ".", "\"", ";", "!", ":", "(", ")", "\""};
// ===
    inline bool hasDashAfter(TOTAL_TOKENS_TYPE i) {
        return 0 <= i && i < totalWordTokens && wordTokenInfo[i].get(DASH_AFTER);
    }

    inline bool hasQuoteBefore(TOTAL_TOKENS_TYPE i) {
        return 0 <= i && i < totalWordTokens && wordTokenInfo[i].get(QUOTE_BEFORE);
    }

    inline bool hasQuoteAfter(TOTAL_TOKENS_TYPE i) {
        return 0 <= i && i < totalWordTokens && wordTokenInfo[i].get(QUOTE_AFTER);
    }

    inline bool hasParentThesisBefore(TOTAL_TOKENS_TYPE i) {
        return 0 <= i && i < totalWordTokens && wordTokenInfo[i].get(PARENTHESIS_BEFORE);
    }

    inline bool hasParentThesisAfter(TOTAL_TOKENS_TYPE i) {
        return 0 <= i && i < totalWordTokens && wordTokenInfo[i].get(PARENTHESIS_AFTER);
    }

    inline bool isFirstCapital(TOTAL_TOKENS_TYPE i) {
        return 0 <= i && i < totalWordTokens && wordTokenInfo[i].get(FIRST_CAPITAL);
    }

    inline bool isAllCapital(TOTAL_TOKENS_TYPE i) {
        return 0 <= i && i < totalWordTokens && wordTokenInfo[i].get(ALL_CAPITAL);
    }

    inline bool isEndOfSentence(TOTAL_TOKENS_TYPE i) {
        return i < 0 || i + 1 >= totalWordTokens || wordTokenInfo[i].get(SEPARATOR_AFTER) || (i+1 < totalWordTokens && wordTokenInfo[i+1].get(SEPARATOR_BEFORE) );
    }

    inline void loadStopwords(const string &filename) {
        FILE* in = tryOpen(filename, "r");
        
        while (getLine(in)) {
            vector<string> tokens = splitBy(line, ' ');
            if (tokens.size() == 1) {
                TOKEN_ID_TYPE id;
                fromString(tokens[0], id);
                if (id >= 0) {
                    stopwords.insert(id);
                }
            }
        }
        if (INTERMEDIATE) {
            cerr << "# of loaded stop words = " << stopwords.size() << endl;
        }
        fclose(in);
    }

    unordered_map<int,string> id2token;

    inline void loadTokenMapping(const string& filename)
    {
        FILE* in = tryOpen(filename,"r");
        int max_ = 0;

        while (getLine(in)) {
            vector<string> pair = splitBy(line,'\t');
            if ( pair.size() < 2 ) continue;
            int id = atoi(pair[1].c_str());
            id2token[id] = pair[0];
        }
    }

    inline void loadAllTrainingFiles(const string& docFile, const string& capitalFile) {
        if (true) {
            // get total number of tokens and the maximum number of tokens
            FILE* in = tryOpen(docFile, "r");
            TOTAL_TOKENS_TYPE totalTokens = 0;
            for (;fscanf(in, "%s", line) == 1; ++ totalTokens) {
                bool flag = true;
                TOKEN_ID_TYPE id = 0;
                for (TOTAL_TOKENS_TYPE i = 0; line[i] && flag; ++ i) {
                    flag &= isdigit(line[i]);
                    id = id * 10 + line[i] - '0';
                }
                if (flag) {
                    maxTokenID = max(maxTokenID, id);
                    ++ totalWordTokens;
                }
            }
            cerr << "# of total tokens = " << totalTokens << endl;
            if (INTERMEDIATE) {
                cerr << "# of total word tokens = " << totalWordTokens << endl;
            }
            cerr << "max word token id = " << maxTokenID << endl;
            fclose(in);
        }

        idf = new float[maxTokenID + 1];
        isDigital = new bool[maxTokenID + 1];
        for (TOKEN_ID_TYPE i = 0; i <= maxTokenID; ++ i) {
            isDigital[i] = false;
        }
        wordTokens = new TOKEN_ID_TYPE[totalWordTokens];
        wordTokenInfo = new WordTokenInfo[totalWordTokens];

        char currentTag[100];

        FILE* in = tryOpen(docFile, "r");
        FILE* capitalIn = tryOpen(capitalFile, "r");

        INDEX_TYPE docs = 0;
        TOTAL_TOKENS_TYPE ptr = 0;

        while (getLine(in)) {
            ++ docs;
            TOTAL_TOKENS_TYPE docStart = ptr;

            stringstream sin(line);

            myAssert(getLine(capitalIn), "Captial info file doesn't have enough lines");
            TOTAL_TOKENS_TYPE capitalPtr = 0;
            TOTAL_TOKENS_TYPE capitalLineSize = strlen(line);

            string lastPunc = "";
            for (string temp; sin >> temp;) {

                // get token
                bool flag = true;
                TOKEN_ID_TYPE token = 0;
                for (size_t i = 0; i < temp.size() && flag; ++ i) {
                    flag &= isdigit(temp[i]);
                    token = token * 10 + temp[i] - '0';
                }

                // get capital info
                int capitalInfo = 0;
                while ( line[capitalPtr] != ' ' && capitalPtr < capitalLineSize ) {
                    capitalInfo = capitalInfo * 10 + line[capitalPtr] - '0';
                    ++capitalPtr;
                }
                ++capitalPtr;

                wordTokens[ptr] = token;
                wordTokenInfo[ptr].mask = capitalInfo;

                if (capitalInfo >> 9 & 1) {
                    isDigital[token] = true;
                }
                ++ ptr;
            }

            // The end of line is also a separator.
            wordTokenInfo[ptr - 1].turnOn(SEPARATOR_AFTER);

            set<TOKEN_ID_TYPE> docSet(wordTokens + docStart, wordTokens + ptr);
            FOR (token, docSet) {
                ++ idf[*token];
            }
        }
        fclose(in);

        for (TOKEN_ID_TYPE i = 0; i <= maxTokenID; ++ i) {
            idf[i] = log(docs / idf[i] + EPS);
        }

        cerr << "# of documents = " << docs << endl;
    }

    inline void splitIntoSentences() {
        sentences.clear();
        TOTAL_TOKENS_TYPE st = 0;
        for (TOTAL_TOKENS_TYPE i = 0; i < totalWordTokens; ++ i) {
            if (isEndOfSentence(i)) {
                sentences.push_back(make_pair(st, i));
                st = i + 1;
                // cout << wordTokens[i] << endl;
            }
            // else cout << wordTokens[i] << " " ;
        }
        sentences.shrink_to_fit();
        if (INTERMEDIATE) {
            cerr << "The number of sentences = " << sentences.size() << endl;
        }
    }

};

#endif
