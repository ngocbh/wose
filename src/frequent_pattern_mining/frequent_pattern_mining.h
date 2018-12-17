#ifndef __FREQUENT_PATTERN_MINING_H__
#define __FREQUENT_PATTERN_MINING_H__

#include "../utils/utils.h"
#include "../data_processing/documents.h"

using Documents::id2token;

namespace FrequentPatternMining
{
    ULL MAGIC = 0xabcdef; //HASH BASE
    const int UNKNOWN_LABEL = -1000000000;

    struct Pattern {
        vector<TOKEN_ID_TYPE> tokens;
        int label;
        double probability, quality;
        ULL hashValue;
        TOKEN_ID_TYPE currentFreq;

        void dump(FILE* out) {
            Binary::write(out, currentFreq);
            Binary::write(out, quality);
            Binary::write(out, tokens.size());
            for (auto& token : tokens) {
                Binary::write(out, token);
            }
        }

        void load(FILE* in) {
            Binary::read(in, currentFreq);
            Binary::read(in, quality);
            size_t tokenSize;
            Binary::read(in, tokenSize);
            tokens.clear();
            for (size_t i = 0; i < tokenSize; ++ i) {
                TOTAL_TOKENS_TYPE token;
                Binary::read(in, token);
                append(token);
            }
        }

        Pattern(const TOKEN_ID_TYPE &token) {
            tokens.clear();
            hashValue = 0;
            currentFreq = 0;
            label = UNKNOWN_LABEL;
            append(token);
            quality = 1;
        }

        Pattern(const Pattern &other) {
            this->tokens = other.tokens;
            this->hashValue = other.hashValue;
            this->probability = other.probability;
            this->label = other.label;
            this->quality = other.quality;
            this->currentFreq = other.currentFreq;
        }

        Pattern() {
            tokens.clear();
            hashValue = 0;
            currentFreq = 0;
            label = UNKNOWN_LABEL;
            quality = 1;
        }

        inline void shrink_to_fit() {
            tokens.shrink_to_fit();
        }

        inline Pattern substr(int l, int r) const {
            Pattern ret;
            for (int i = l; i < r; ++ i)  {
                ret.append(tokens[i]);
            }
            return ret;
        }

        inline int size() const {
            return tokens.size();
        }

        inline void append(const TOKEN_ID_TYPE &token) {
            tokens.push_back(token);
            hashValue = hashValue * MAGIC + token + 1;
        }

        inline bool operator == (const Pattern &other) const {
            return hashValue == other.hashValue && tokens == other.tokens;
        }

        inline void show() const {
            for (int i = 0; i < tokens.size(); ++ i) {
                cout << id2token[tokens[i]].c_str() << " ";
            }
            cout << endl;
        }
    };

// === global variables ===
    TOTAL_TOKENS_TYPE *unigrams; // 0 .. Documents::maxTokenID
    vector<Pattern> patterns, truthPatterns, truthLabels;
    vector<vector<TOTAL_TOKENS_TYPE>> id2ends;
    unordered_map<ULL, PATTERN_ID_TYPE> pattern2id;

// ===

    bool isPrime(ULL x) {
        for (ULL y = 2; y * y <= x; ++ y) {
            if (x % y == 0) {
                return false;
            }
        }
        return true;
    }

    inline void initialize() {
        MAGIC = Documents::maxTokenID + 1;
        while (!isPrime(MAGIC)) {
            ++ MAGIC;
        }
        cerr << "selected MAGIC = " << MAGIC << endl;
    }

    inline void addPattern(const Pattern& pattern, const TOTAL_TOKENS_TYPE& ed, bool addPosition = true) {
        assert(pattern2id.count(pattern.hashValue));
        PATTERN_ID_TYPE id = pattern2id[pattern.hashValue];
        assert(id < id2ends.size());
        if (patterns[id].currentFreq == 0) {
            patterns[id] = pattern;
        }
        if (addPosition) {
            assert(patterns[id].currentFreq < id2ends[id].size());
            id2ends[id][patterns[id].currentFreq] = ed;
        }
        ++ patterns[id].currentFreq;
    }

    inline void mine(int MIN_SUP, int LENGTH_THRESHOLD = 6) {

        id2ends.clear();
        patterns.clear();
        pattern2id.clear();

        //frequency of unigrams
        unigrams = new TOTAL_TOKENS_TYPE[Documents::maxTokenID + 1];

        for (TOTAL_TOKENS_TYPE i = 0; i <= Documents::maxTokenID; ++ i) {
            unigrams[i] = 0;
        }
        // count frequency of unigrams
        for (TOTAL_TOKENS_TYPE i = 0; i < Documents::totalWordTokens; ++ i) {
            const TOTAL_TOKENS_TYPE& token = Documents::wordTokens[i];
            ++ unigrams[token];
        }

        // all unigrams should be added as patterns
        // allocate memory
        // add unigram to patterns and mapping it by pattern2id
        for (TOTAL_TOKENS_TYPE i = 0; i <= Documents::maxTokenID; ++ i) {
            pattern2id[i + 1] = patterns.size();
            patterns.push_back(Pattern());
        }

        //nếu unigram mà xuất hiện >= min_sup thì resize id2ends. lưu các vị trị xuất hiện của unigrams đó
        id2ends.resize(patterns.size());
        for (TOTAL_TOKENS_TYPE i = 0; i <= Documents::maxTokenID; ++ i) {
            id2ends[i].resize(unigrams[i] >= MIN_SUP ? unigrams[i] : 0);
        }

        long long totalOcc = 0;

        // duyệt corpus. nếu unigram token >= MIN_SUP. thì thêm unigram đó như pattern. để chuẩn bị lập multi word phrase
        for (TOTAL_TOKENS_TYPE i = 0; i < Documents::totalWordTokens; ++ i) {
            const TOTAL_TOKENS_TYPE& token = Documents::wordTokens[i];
            addPattern(Pattern(token), i, unigrams[token] >= MIN_SUP);
            totalOcc += unigrams[token] >= MIN_SUP;
        }
        if (INTERMEDIATE) {
            cerr << "unigrams inserted" << endl;
        }

        PATTERN_ID_TYPE last = 0;
        for (int len = 1; len <= LENGTH_THRESHOLD && last < patterns.size(); ++ len) {
            if (INTERMEDIATE) {
                cerr << "# of frequent patterns of length-" << len << " = "  << patterns.size() - last + 1 << endl;
            }
            PATTERN_ID_TYPE backup = patterns.size();

            unordered_map<ULL, TOTAL_TOKENS_TYPE> hashFreq;
            //Duyệt tất cả các patterns bắt đầu từ last -> patterns.size() là các pattern có độ dài = len. 
            // các pattern có độ dài ngắn hơn bỏ qua vì duyệt rồi
            // phần này chỉ tính toán để phân bổ lại bộ nhớ. sau đó sẽ tính cụ thể vị trí sau
            for (PATTERN_ID_TYPE id = last; id < backup; ++ id) {
                assert(patterns[id].size() == 0 || patterns[id].size() == len);
                id2ends[id].shrink_to_fit();
                if (len < LENGTH_THRESHOLD) {
                    // id2ends[id] là danh sách các vị trí trong corpus có xuất hiện pattern.
                    for (const TOTAL_TOKENS_TYPE& ed : id2ends[id]) {
                        TOTAL_TOKENS_TYPE st = ed - len + 1;
                        assert(Documents::wordTokens[st] == patterns[id].tokens[0]);
                        //thêm từ tiếp theo vào pattern nếu từ tiếp theo có ngưỡng xuất hiện >= MIN_SUP
                        if (!Documents::isEndOfSentence(ed)) {
                            if (unigrams[Documents::wordTokens[ed + 1]] >= MIN_SUP) {
                                ULL newHashValue = patterns[id].hashValue * MAGIC + Documents::wordTokens[ed + 1] + 1;
                                //lưu danh sách các pattern mới tạo ra đc và tần số của pattern mới
                                ++ hashFreq[newHashValue];
                            }
                        }
                    }
                }
            }

            // merge and allocate memory
            vector<pair<ULL, TOTAL_TOKENS_TYPE>> newPatterns;
            for (const auto& iter : hashFreq) {
                const TOTAL_TOKENS_TYPE& freq = iter.second;
                if (freq >= MIN_SUP) {
                    const ULL& hashValue = iter.first;
                    pattern2id[hashValue] = patterns.size();
                    patterns.push_back(Pattern());
                    newPatterns.push_back(make_pair(hashValue, freq));
                }
            }
            hashFreq.clear();

            id2ends.resize(patterns.size());

            for (size_t i = 0; i < newPatterns.size(); ++ i) {
                const ULL& hashValue = newPatterns[i].first;
                const TOTAL_TOKENS_TYPE& freq = newPatterns[i].second;
                id2ends[pattern2id[hashValue]].resize(freq);
            }
            newPatterns.clear();

            //Tính toán lại và add pattern
            for (PATTERN_ID_TYPE id = last; id < backup; ++ id) {
                if (len < LENGTH_THRESHOLD) {
                    vector<TOTAL_TOKENS_TYPE> positions = id2ends[id];
                    for (const TOTAL_TOKENS_TYPE& ed : positions) {
                        TOTAL_TOKENS_TYPE st = ed - len + 1;
                        assert(Documents::wordTokens[st] == patterns[id].tokens[0]);

                        if (!Documents::isEndOfSentence(ed)) {
                            if (unigrams[Documents::wordTokens[ed + 1]] >= MIN_SUP) {
                                ULL newHashValue = patterns[id].hashValue * MAGIC + Documents::wordTokens[ed + 1] + 1;
                                if (pattern2id.count(newHashValue)) {
                                    Pattern newPattern(patterns[id]);
                                    newPattern.append(Documents::wordTokens[ed + 1]);
                                    assert(newPattern.size() == len + 1);
                                    newPattern.currentFreq = 0;

                                    addPattern(newPattern, ed + 1);
                                    ++ totalOcc;
                                }
                            }
                        }
                    }
                    /*if (len == 1) {
                        id2ends[id].clear();
                        id2ends[id].shrink_to_fit();
                    }*/
                }
            }
            last = backup;

        }
        id2ends.shrink_to_fit();

        cerr << "# of frequent phrases = " << patterns.size() << endl;
        if (INTERMEDIATE) {
            cerr << "total occurrence = " << totalOcc << endl;
        }

        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            assert(patterns[i].currentFreq == id2ends[i].size() || id2ends[i].size() == 0);
            assert(patterns[i].size() == 0 || patterns[i].size() == 1 || id2ends[i].size() >= MIN_SUP);
        }
    }
};

#endif
