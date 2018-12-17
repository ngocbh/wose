#ifndef __LABEL_GENERATION_H__
#define __LABEL_GENERATION_H__

#include "../utils/utils.h"
#include "../data_processing/documents.h"
#include "../frequent_pattern_mining/frequent_pattern_mining.h"
#include "../utils/random.h"

using Documents::totalWordTokens;
using Documents::wordTokens;
using Documents::stopwords;

using FrequentPatternMining::Pattern;
using FrequentPatternMining::patterns;
using FrequentPatternMining::truthLabels;
using FrequentPatternMining::pattern2id;
using FrequentPatternMining::id2ends;
using FrequentPatternMining::unigrams;

using RandomNumbers::rng;

namespace Label
{

inline vector<Pattern> loadLabels(string filename)
{
    vector<Pattern> ret;
    FILE* in = tryOpen(filename, "r");
    while (getLine(in)) {
        stringstream sin(line);
        bool valid = true;
        Pattern p;
        sin >> p.label;
        for (TOKEN_ID_TYPE s; sin >> s;) {
            p.append(s);
        }
        if (p.size() > 0 && pattern2id.count(p.hashValue)) {
            ret.push_back(p);
        }
    }
    cerr << "# of loaded labels = " << ret.size() << endl;
    return ret;
}

inline vector<Pattern> loadTruthPatterns(string filename)
{
    vector<Pattern> ret;
    FILE* in = tryOpen(filename, "r");
    while (getLine(in)) {
        stringstream sin(line);
        bool valid = true;
        Pattern p;
        for (string s; sin >> s;) {
            bool possibleInt = false;
            for (int i = 0; i < s.size(); ++ i) {
                possibleInt |= isdigit(s[i]);
            }
            if (possibleInt) {
                TOKEN_ID_TYPE x;
                fromString(s, x);
                if (x < 0) {
                    valid = false;
                    break;
                }
                p.append(x);
            }
        }
        if (valid) {
            ret.push_back(p);
        }
    }
    fclose(in);
    return ret;
}

inline unordered_set<ULL> loadPatterns(string filename, int MAX_POSITIVE)
{
    FILE* in = tryOpen(filename, "r");
    vector<ULL> positivesUnigrams, positiveMultiwords;
    while (getLine(in)) {
        stringstream sin(line);
        bool valid = true; //valid <-> all token id is positive  (> 0)
        Pattern p;
        //read pattern to p
        for (string s; sin >> s;) {
            bool possibleInt = false;
            for (int i = 0; i < s.size(); ++ i) {
                possibleInt |= isdigit(s[i]);
            }
            if (possibleInt) {
                TOKEN_ID_TYPE x;
                fromString(s, x);
                if (x < 0) {
                    valid = false;
                    break;
                }
                p.append(x);
            }
        }

        // if recognize this phrase in pattern -> label it is true
        if (valid && pattern2id.count(p.hashValue)) {
            if (p.size() > 1) {
                positiveMultiwords.push_back(p.hashValue);
            } else if (p.size() == 1) {
                positivesUnigrams.push_back(p.hashValue);
            }
        }
    }
    fclose(in);

    if (MAX_POSITIVE != -1) {
        sort(positiveMultiwords.begin(), positiveMultiwords.end());
        //earse duplicate phrase and get positive 
        positiveMultiwords.erase(unique(positiveMultiwords.begin(), positiveMultiwords.end()), positiveMultiwords.end());
        if (MAX_POSITIVE < positiveMultiwords.size()) {
            srand(time(0) ^ 13548689);
            random_shuffle(positiveMultiwords.begin(), positiveMultiwords.end());
            positiveMultiwords.resize(MAX_POSITIVE);
        }
        sort(positivesUnigrams.begin(), positivesUnigrams.end());
        positivesUnigrams.erase(unique(positivesUnigrams.begin(), positivesUnigrams.end()), positivesUnigrams.end());
        if (MAX_POSITIVE < positivesUnigrams.size()) {
            srand(time(0) ^ 13548689);
            random_shuffle(positivesUnigrams.begin(), positivesUnigrams.end());
            positivesUnigrams.resize(MAX_POSITIVE);
        }
    }
    unordered_set<ULL> ret;
    for (ULL value : positiveMultiwords) {
        ret.insert(value);
    }
    for (ULL value : positivesUnigrams) {
        ret.insert(value);
    }
    return ret;
}

inline vector<Pattern> generateAll(string LABEL_METHOD, string LABEL_FILE, string ALL_FILE, string QUALITY_FILE)
{
    vector<Pattern> ret;

    int cntPositives = 0, cntNegatives = 0;

    if (LABEL_METHOD.find("E") != -1) { // experts
        vector<Pattern> truth;
        cerr << "Loading existing labels..." << endl;
        truth = Label::loadLabels(LABEL_FILE);
        bool needPos = LABEL_METHOD.find("EP") != -1;
        bool needNeg = LABEL_METHOD.find("EN") != -1;
        for (PATTERN_ID_TYPE i = 0; i < truth.size(); ++ i) {
            if (truth[i].label == 1) {
                if (needPos) {
                    truthLabels.push_back(truth[i]);
                    ret.push_back(truth[i]);
                    ++cntPositives;
                }
            } else if (truth[i].label == 0) {
                if (needNeg) {
                    truthLabels.push_back(truth[i]);
                    ret.push_back(truth[i]);
                    ++cntNegatives;
                }
            }
        }

        fprintf(stderr, "\tThe number of positive example in label file = %d\n", cntPositives);
        fprintf(stderr, "\tThe number of negative example in label file = %d\n", cntNegatives);
    }

    if (LABEL_METHOD.find("D") != -1) { // distant training
        bool needPos = LABEL_METHOD.find("DP") != -1;
        bool needNeg = LABEL_METHOD.find("DN") != -1;

        unordered_set<ULL> include = loadPatterns(QUALITY_FILE, MAX_POSITIVE);
        unordered_set<ULL> exclude = loadPatterns(ALL_FILE, MAX_POSITIVE);
        cout << include.size() << " " << exclude.size() << endl;
        if (MAX_POSITIVE != -1) {
            exclude.clear();
        }

        for (ULL value : include) { // make sure exclude is a super set of include
            exclude.insert(value);
        }
        for (int i = 0; i < ret.size(); ++ i) { // make sure every human label is excluded
            exclude.insert(ret[i].hashValue);
        }

        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            if (patterns[i].size() < 1) {
                continue;
            }
            if (patterns[i].size() == 1 && stopwords.count(patterns[i].tokens[0])) {
                ret.push_back(patterns[i]);
                ret.back().label = 0;
            } else if (include.count(patterns[i].hashValue)) {
                if (needPos) {
                    ret.push_back(patterns[i]);
                    ret.back().label = 1;
                }
            } else if (!exclude.count(patterns[i].hashValue)) {
                if (needNeg) {
                    ret.push_back(patterns[i]);
                    ret.back().label = 0;
                }
            }
        }
    }

    cntPositives = 0, cntNegatives = 0;
    for (PATTERN_ID_TYPE i = 0; i < ret.size(); ++ i) {
        if (ret[i].label == 1) {
            ++ cntPositives;
        } else if (ret[i].label == 0) {
            ++ cntNegatives;
        } else {
            assert(false); // It should not happen!
        }
    }

    fprintf(stderr, "\tThe size of the positive pool = %d\n", cntPositives);
    fprintf(stderr, "\tThe size of the negative pool = %d\n", cntNegatives);

    return ret;
}

inline vector<Pattern> generatePool(int NEG_POOL_SIZE,int POS_POOL_SIZE, vector<Pattern> truthLabels, vector<Pattern> truth)
{
    fprintf(stderr,"==== Generate Pool : truthLabels====\n");
    vector<Pattern> ret;

    for (int i = 0; i < truthLabels.size(); i++)
        ret.push_back(truthLabels[i]);

    // bootstrapping - thêm tất cả kết quả vào 2 vector index[0] và index[1].
    // index[0] : negative pool
    // index[1] : positive pool
    vector<int> index[2];
    for (int i = 0; i < (int)truth.size(); ++ i) 
        if ( truth[i].size() > 1 ) {
            index[(int)truth[i].label].push_back(i);
        }

    //choose negative example
    if ( NEG_POOL_SIZE == - 1) NEG_POOL_SIZE = INF;
    int selected = min(NEG_POOL_SIZE, (int)index[0].size());
    for (int i = 0; i < selected; ++ i) {
        int id = index[0][rng.next(index[0].size())];
        ret.push_back(truth[id]);
    }

    //choose postive example
    if ( POS_POOL_SIZE == -1 ) POS_POOL_SIZE = INF;
    selected = min(POS_POOL_SIZE, (int)index[1].size());
    for (int i = 0; i < selected; ++i) {
        int id = index[1][rng.next(index[1].size())];
        ret.push_back(truth[id]);
    }

    int cntPositives = 0, cntNegatives = 0;
    for (PATTERN_ID_TYPE i = 0; i < ret.size(); ++ i) {
        if (ret[i].label == 1) {
            ++ cntPositives;
        } else if (ret[i].label == 0) {
            ++ cntNegatives;
        } else {
            assert(false); // It should not happen!
        }
    }

    fprintf(stderr, "\tThe size of the positive pool = %d\n", cntPositives);
    fprintf(stderr, "\tThe size of the negative pool = %d\n", cntNegatives);

    return ret;
}

void removeWrongLabels()
{
    for (Pattern& pattern : patterns) {
        if (pattern.currentFreq == 0 && pattern.label == 1) {
            pattern.label = FrequentPatternMining::UNKNOWN_LABEL;
        }
    }
}

}

#endif
