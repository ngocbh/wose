#include <iostream>
#include "utils/parameters.h"
#include "utils/utils.h"
#include "utils/argument_parser.h"
#include "data_processing/documents.h"
#include "data_processing/dump.h"
#include "frequent_pattern_mining/frequent_pattern_mining.h"
#include "classification/feature_extraction.h"
#include "classification/label_generation.h" 
#include "utils/random.h"
#include "classification/predict_quality.h"
using namespace std;

using FrequentPatternMining::Pattern;
using FrequentPatternMining::patterns;
using FrequentPatternMining::truthPatterns;
using RandomNumbers::rng;

int main(int argc, char* argv[])
{
	parseArgument(argc,argv);

	cerr << "Loading data..." << endl;
    // load stopwords, documents, and capital information
    Documents::loadTokenMapping(TOKEN_MAPPING_FILE);
	Documents::loadStopwords(STOPWORD_FILE);
	Documents::loadAllTrainingFiles(RAW_CORPUS_FILE,SHAPE_CORPUS_FILE);
	Documents::splitIntoSentences();

	cerr << "Mining frequent patterns..." << endl;
    FrequentPatternMining::initialize();
    FrequentPatternMining::mine(MIN_SUP, MAX_LEN);

    // check the patterns
    if (INTERMEDIATE) {
        vector<pair<TOTAL_TOKENS_TYPE, PATTERN_ID_TYPE>> order;
        for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
            order.push_back(make_pair(patterns[i].currentFreq, i));
            // cout << patterns[i].currentFreq << endl;
        }
        Dump::dumpRankingList("tmp/frequent_patterns.txt", order);
    }

    // feature extraction
    cerr << "Extracting features..." << endl;
    vector<string> featureNames;
    vector<vector<double>> features = Features::extract(featureNames);

    vector<string> featureNamesUnigram;
    vector<vector<double>> featuresUnigram = Features::extractUnigram(featureNamesUnigram);

    //labeled 
    // tất cả các 
    cerr << "Constructing label pools..." << endl;
    vector<Pattern> truth = Label::generateAll(LABEL_METHOD, LABEL_FILE, ALL_WORD_FILE, QUALITY_WORD_FILE);

    truthPatterns = Label::loadTruthPatterns(QUALITY_WORD_FILE);
    cerr << "# truth patterns = " << truthPatterns.size() << endl;
    for (Pattern p : truth) {
        if (p.label == 1) {
            truthPatterns.push_back(p);
        }
    }

    if (INTERMEDIATE) {
        Dump::dumpLabels("tmp/labeled_words.txt",truth);
    }

    TOTAL_TOKENS_TYPE recognized = Features::recognize(truth);
    // estimate
    cerr << "Estimating Phrase Quality..." << endl;
    predictQuality(patterns, features, featureNames);
    predictQualityUnigram(patterns, featuresUnigram, featureNamesUnigram);

    if (INTERMEDIATE) {
        char filename[256];
        sprintf(filename, "tmp/iter_0_quality" );
        Dump::dumpResults(filename);
    }

} 