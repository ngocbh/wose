#ifndef __SEGMENTATION_H__
#define __SEGMENTATION_H__

#include <cassert>

#include "../utils/utils.h"
#include "../frequent_pattern_mining/frequent_pattern_mining.h"
using FrequentPatternMining::Pattern;
// === global variables ===
using FrequentPatternMining::patterns;
using FrequentPatternMining::pattern2id;
using FrequentPatternMining::truthPatterns;
using FrequentPatternMining::id2ends;
using FrequentPatternMining::unigrams;

#define TRUTH (patterns.size())

struct TrieNode {
    unordered_map<TOTAL_TOKENS_TYPE, size_t> children;

    PATTERN_ID_TYPE id;

    TrieNode() {
        id = -1;
        children.clear();
    }
};
vector<TrieNode> trie;

// ===

void constructTrie(bool duringTraingStage = true) {
    trie.clear();
    trie.push_back(TrieNode());

    for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
        const vector<TOTAL_TOKENS_TYPE>& tokens = patterns[i].tokens;
        if (tokens.size() == 0 || tokens.size() > 1 && patterns[i].currentFreq == 0) {
            continue;
        }
        size_t u = 0;
        for (const TOTAL_TOKENS_TYPE& token : tokens) {
            if (!trie[u].children.count(token)) {
                trie[u].children[token] = trie.size();
                trie.push_back(TrieNode());
            }
            u = trie[u].children[token];
        }
        trie[u].id = i;
    }
    if (INTERMEDIATE) {
        cerr << "# of trie nodes = " << trie.size() << endl;
    }

    if (duringTraingStage) {
        for (PATTERN_ID_TYPE i = 0; i < truthPatterns.size(); ++ i) {
            const vector<TOTAL_TOKENS_TYPE>& tokens = truthPatterns[i].tokens;
            size_t u = 0;
            for (const TOTAL_TOKENS_TYPE& token : tokens) {
                if (!trie[u].children.count(token)) {
                    trie[u].children[token] = trie.size();
                    trie.push_back(TrieNode());
                }
                u = trie[u].children[token];
            }
            if (trie[u].id == -1 || !duringTraingStage) {
                trie[u].id = TRUTH;
            }
        }
        if (INTERMEDIATE) {
            cerr << "# of trie nodes = " << trie.size() << endl;
        }
    }
}

class Segmentation
{
private:
    static const double INF;
    static vector<vector<TOTAL_TOKENS_TYPE>> total;
private:
    // generated
    int maxLen;
    double *prob;

    void normalize(vector<double> &pLen) {
        vector<double> sum(maxLen + 1, 0);
        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            sum[patterns[i].size()] += prob[i];
        }

        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            prob[i] /= sum[patterns[i].size()];
        }

        int sumLen = 0;
        for (PATTERN_ID_TYPE i = 0; i <= maxLen; i++) {
            sumLen += pLen[i];
        }

        for (PATTERN_ID_TYPE i = 0; i <= maxLen; i++) {
            pLen[i] /= sumLen;
        }
    }

    vector<double> initialize() {
        // compute maximum tokens
        maxLen = 0;
        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            maxLen = max(maxLen, patterns[i].size());
        }

        vector<double> pLen(maxLen + 1, 1);

        prob = new double[patterns.size() + 1];
        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            prob[i] = 0;
        }
        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            prob[i] = patterns[i].currentFreq;
            pLen[patterns[i].size()] += patterns[i].currentFreq;
        }
        normalize(pLen);
        prob[patterns.size()] = 1;

        return pLen;
    }

public:
    double getProb(int id) const {
        return exp(prob[id]);
    }

    ~Segmentation() {
        delete [] prob;
    }

    Segmentation() {
        vector<double> pLen(maxLen + 1, 1);
        pLen = initialize();

        double maxProb = *max_element(prob, prob + patterns.size());
        prob[patterns.size()] = log(maxProb + EPS);
        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            prob[i] = log(prob[i] + EPS) + log(pLen[patterns[i].size() - 1]) + log(patterns[i].quality + EPS);
        }
    }

    inline double viterbi(const vector<TOKEN_ID_TYPE> &tokens, vector<double> &f, vector<int> &pre) {
        f.clear();
        f.resize(tokens.size() + 1, -INF);
        pre.clear();
        pre.resize(tokens.size() + 1, -1);
        f[0] = 0;
        pre[0] = 0;
        for (size_t i = 0 ; i < tokens.size(); ++ i) {
            if (f[i] < -1e80) {
                continue;
            }
            bool impossible = true;
            for (size_t j = i, u = 0; j < tokens.size(); ++ j) {
                if (!trie[u].children.count(tokens[j])) {
                    break;
                }
                u = trie[u].children[tokens[j]];
                if (trie[u].id != -1) {
                    impossible = false;
                    PATTERN_ID_TYPE id = trie[u].id;
                    double p = prob[id];
                    if (f[i] + p > f[j + 1]) {
                        f[j + 1] = f[i] + p;
                        pre[j + 1] = i;
                    }
                }
            }
            if (impossible) {
                if (f[i] > f[i + 1]) {
                    f[i + 1] = f[i];
                    pre[i + 1] = i;
                }
            }
        }
        return f[tokens.size()];
    }

    inline double viterbi_proba(const vector<TOKEN_ID_TYPE> &tokens, vector<double> &f, vector<int> &pre) {
        f.clear();
        f.resize(tokens.size() + 1, 0);
        pre.clear();
        pre.resize(tokens.size() + 1, -1);
        f[0] = 1;
        pre[0] = 0;
        for (size_t i = 0 ; i < tokens.size(); ++ i) {
            Pattern pattern;
            for (size_t j = i, u = 0; j < tokens.size(); ++ j) {
                if (!trie[u].children.count(tokens[j])) {
                    if (j == i) {
                        if (f[i] > f[j + 1]) {
                            f[j + 1] = f[i];
                            pre[j + 1] = i;
                        }
                    }
                    break;
                }
                u = trie[u].children[tokens[j]];
                if (trie[u].id != -1 && j - i + 1 != tokens.size()) {
                    PATTERN_ID_TYPE id = trie[u].id;
                    double p = exp(prob[id]);
                    if (f[i] * p > f[j + 1]) {
                        f[j + 1] = f[i] * p;
                        pre[j + 1] = i;
                    }
                }
            }
        }
        return f[tokens.size()];
    }

    inline void rectifyFrequency(vector<pair<TOTAL_TOKENS_TYPE, TOTAL_TOKENS_TYPE>> &sentences) {
        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            patterns[i].currentFreq = 0;
            id2ends[i].clear();
        }

        double energy = 0;
        for (INDEX_TYPE senID = 0; senID < sentences.size(); ++ senID) {
            vector<TOKEN_ID_TYPE> tokens;
            for (TOTAL_TOKENS_TYPE i = sentences[senID].first; i <= sentences[senID].second; ++ i) {
                tokens.push_back(Documents::wordTokens[i]);
            }
            vector<double> f;
            vector<int> pre;

            double bestExplain = viterbi(tokens, f, pre);

            int i = (int)tokens.size();
            assert(f[i] > -1e80);
            energy += f[i];
    		while (i > 0) {
    			int j = pre[i];
                size_t u = 0;
                for (int k = j; k < i; ++ k) {
                    assert(trie[u].children.count(tokens[k]));
                    u = trie[u].children[tokens[k]];
                }
                if (trie[u].id != -1) {
                    PATTERN_ID_TYPE id = trie[u].id;
                    if (id < patterns.size()) {
                        ++ patterns[id].currentFreq;
                        if (i - j > 1 || i - j == 1 && unigrams[patterns[id].tokens[0]] >= MIN_SUP) {
                            id2ends[id].push_back(sentences[senID].first + i - 1);
                        }
                    }
                }
    			i = j;
    		}
        }
        //cerr << "Energy = " << energy << endl;
    }


    inline double viterbi_for_testing(const vector<TOKEN_ID_TYPE> &tokens, vector<double> &f, vector<int> &pre, double multi_thres, double uni_thres) {
        f.clear();
        f.resize(tokens.size() + 1, -INF);
        pre.clear();
        pre.resize(tokens.size() + 1, -1);
        f[0] = 0;
        pre[0] = 0;
        for (size_t i = 0 ; i < tokens.size(); ++ i) {
            if (f[i] < -1e80) {
                continue;
            }
            bool impossible = true;
            for (size_t j = i, u = 0; j < tokens.size(); ++ j) {
                if (!trie[u].children.count(tokens[j])) {
                    break;
                }
                u = trie[u].children[tokens[j]];
                if (trie[u].id != -1) {
                    if (qualify(trie[u].id, j - i + 1, multi_thres, uni_thres)) {
                        impossible = false;
                        PATTERN_ID_TYPE id = trie[u].id;
                        double p = prob[id];
                        
                        if (f[i] + p > f[j + 1]) {
                            f[j + 1] = f[i] + p;
                            pre[j + 1] = i;
                        }
                        // cout << "show----" << endl;
                        // cout << id << " " << endl;
                        // patterns[id].show();
                        // cout << "endshow----" << endl;
                        // cout << i << " " << j << " " << p << " " << patterns[id].currentFreq << " " << patterns[id].quality << endl;
                        // for (int o = 0; o < tokens.size(); o++) 
                        //     cout << o << "=" << f[o] << " ";
                        // cout << endl; 
                    }
                }
            }
            if (impossible) {
                if (f[i] > f[i + 1]) {
                    f[i + 1] = f[i];
                    pre[i + 1] = i;
                }
            }
        }
        return f[tokens.size()];
    }

    inline bool qualify(int id, int length, double multi_thres, double uni_thres) {
        return id == patterns.size() && ( // These labeled word are in the dictionary, their quality scores are treated as 1.
                    length > 1 && 1 >= multi_thres ||
                    length == 1 && 1 >= uni_thres) || 
               id < patterns.size() && id >= 0 && ( 
                    patterns[id].size() > 1 && patterns[id].quality >= multi_thres ||
                    patterns[id].size() == 1 && patterns[id].quality >= uni_thres);
    }
};

const double Segmentation::INF = 1e100;
vector<vector<TOTAL_TOKENS_TYPE>> Segmentation::total;
#endif
