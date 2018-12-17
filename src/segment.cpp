#include "utils/parameters.h"
#include "utils/argument_parser.h"
#include "utils/utils.h"
#include "frequent_pattern_mining/frequent_pattern_mining.h"
#include "classification/feature_extraction.h"
#include "classification/label_generation.h"
#include "classification/predict_quality.h"
#include "segmentation/segmentation.h"
#include "data_processing/documents.h"
#include "data_processing/dump.h"

using FrequentPatternMining::Pattern;
using FrequentPatternMining::patterns;
using Documents::SEPARATOR_BEFORE;
using Documents::SEPARATOR_AFTER;
using Documents::id2token;

vector<double> f;
vector<int> pre;

int highlights = 0, sentences = 0;

void process(const vector<TOTAL_TOKENS_TYPE>& tokens,Segmentation& segmenter, FILE* out)
{
    ++ sentences;
    segmenter.viterbi_for_testing(tokens, f, pre, SEGMENT_MULTI_WORD_QUALITY_THRESHOLD, SEGMENT_SINGLE_WORD_QUALITY_THRESHOLD);

    int i = (int)tokens.size();
    assert(f[i] > -1e80);
    vector<string> ret;

    while (i > 0) {
        int j = pre[i];
        // cout << j << endl;
        size_t u = 0;
        bool quality = true;
        for (int k = j; k < i; ++ k) {
            if (!trie[u].children.count(tokens[k])) {
                quality = false;
                break;
            }
            u = trie[u].children[tokens[k]];
        }
        if ( i - j <= 1 ) quality = false;
        if (quality) {
            ret.push_back("]");
        }
        for (int k = i - 1; k >= j; -- k) {
            ostringstream sout;
            sout << tokens[k];
            ret.push_back(sout.str());
        }
        if (quality) {
            ret.push_back("[");
        }

        i = j;
    }

    
    reverse(ret.begin(), ret.end());

    for (int i = 0; i < ret.size(); i++) {
        fprintf(out, "%s%c", ret[i].c_str(), i + 1 == ret.size() ? '\n' : ' ');
    }

    /*
     // to show preview output
    int inCompound = 0;
    for (int i = 0; i < ret.size(); ++ i) {
        if ( i + 1 == ret.size() ) {
            fprintf(out, "%c", '\n');
        } else if ( ret[i] == "[" ) {
            inCompound = 1;
        } else if ( ret[i] == "]" ) {
            inCompound = 0;
        } else {
            fprintf(out, "%s", id2token[stoi(ret[i])].c_str());
            if ( inCompound && ret[i+1] != "]" ) {
                fprintf(out, "%c", '_');
            } else {
                fprintf(out, "%c", ' ');
            }
        }
    }
    */
    
}

inline bool byQuality(const Pattern& a, const Pattern& b)
{
    return a.quality > b.quality + EPS || fabs(a.quality - b.quality) < EPS && a.currentFreq > b.currentFreq;
}

int main(int argc, char* argv[])
{
    parseArgument(argc, argv);

    Dump::loadSegmentationModel(SEGMENTATION_MODEL);
    Documents::loadTokenMapping(TOKEN_MAPPING_FILE);
    sort(patterns.begin(), patterns.end(), byQuality);

    constructTrie(false); // update the current frequent enough patterns

    Segmentation* segmenter = new Segmentation(Segmentation::alpha,Segmentation::beta);

    FILE* in = tryOpen(TEXT_TO_SEG_FILE, "r");

    FILE* out = tryOpen("tmp/tokenized_segmented_sentences.txt", "w");

    while (getLine(in)) {
        stringstream sin(line);
        vector<TOTAL_TOKENS_TYPE> tokens;
        bool lastPunc = false;

        for (string temp; sin >> temp;) {
            // get token
            bool flag = true;
            TOKEN_ID_TYPE token = 0;
            for (size_t i = 0; i < temp.size() && flag; ++ i) {
                flag &= isdigit(temp[i]) || i == 0 && temp.size() > 1 && temp[0] == '-';
            }
            stringstream sin(temp);
            sin >> token;

            if (!flag) {
                string punc = temp;
                if (Documents::separatePunc.count(punc)) {
                    process(tokens, *segmenter, out);
                    tokens.clear();
                }
            } else {
                tokens.push_back(token);
            }
        }
        if (tokens.size() > 0) {
            process(tokens, *segmenter, out);
            tokens.clear();
        }
    }
    fclose(in);
    fclose(out);

    cerr << "Phrasal segmentation finished." << endl;
    cerr << "   # of total processed sentences = " << sentences << endl;

    return 0;
}
