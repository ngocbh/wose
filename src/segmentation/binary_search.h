#ifndef __BINARY_SEARCH__
#define __BINARY_SEARCH__

#include "../utils/utils.h"
#include "../utils/parameters.h"
#include "segmentation.h"

int BASE = 47;

double calc_loss_function(double alpha,double beta,vector<Pattern> truthPool,bool isLastRound)
{
	double cntPositive = 0, cntNegative = 0;
	double wrongPositive = 0, wrongNegative = 0;

	Segmentation segmentation(alpha,beta);
	segmentation.rectifyFrequency(Documents::sentences);

	for (int i = 0; i < truthPool.size(); i++) {
		if (truthPool[i].label == 1) {
			++cntPositive;
			vector<double> f;//f is dp array
            vector<int> pre;//pre is trace array
            segmentation.viterbi(truthPool[i].tokens, f, pre);
            wrongPositive += pre[truthPool[i].tokens.size()] != 0; // if it is decomposed, -> wrong
		} else if (truthPool[i].label == 0) {
			++cntNegative;
			vector<double> f;//f is dp array
            vector<int> pre;//pre is trace array
            segmentation.viterbi(truthPool[i].tokens, f, pre);
            wrongNegative += pre[truthPool[i].tokens.size()] == 0; // if it is not decomposed, -> wrong
		}
	}

	double f, g;
	f = wrongPositive / cntPositive;
	g = wrongNegative / cntNegative;

	// return loss function
	double lf = 10*pow(100,f*f) + pow(100,g*g) - 2;

	if (isLastRound && INTERMEDIATE) {
		cerr << "\tAlpha : " << alpha << " | Beta : " << beta << endl;
		cerr << "\tlf : " << lf << endl;
		cerr << "\tf : " << f << endl;
		cerr << "\tg : " << g << endl;
		cerr << "\twrongPositive : " << wrongPositive << "/" << cntPositive << endl;
		cerr << "\twrongNegative : " << wrongNegative << "/" << cntNegative << endl; 
		cerr << endl;
	}
	return lf;
}

void binary_search(double &alpha,double &beta,vector<Pattern> truthPool)
{
	// Binary Search for Length Penalty
	double left_alpha = EPS, right_alpha = 200;
	double wrongPositive = 1, wrongNegative = 1;
    for (int _ = 0; _ < 10; ++ _) {
        alpha = (left_alpha + right_alpha) / 2;
        // calculate probability to to segment each pattern to 1 part
        Segmentation segmentation(alpha,beta);
        segmentation.rectifyFrequency(Documents::sentences);
        double wrongP = 0, totalP = 0, wrongN = 0, totalN = 0;
        for (int i = 0; i < truthPool.size(); ++ i) {
            if (truthPool[i].label == 1) {
                ++ totalP;
                vector<double> f;//f is dp array
                vector<int> pre;//pre is trace array
                segmentation.viterbi(truthPool[i].tokens, f, pre);
                wrongP += pre[truthPool[i].tokens.size()] != 0; // if it is decomposed, -> wrong
            } else if (truthPool[i].label == 0) {
            	++ totalN;
                vector<double> f;//f is dp array
                vector<int> pre;//pre is trace array
                segmentation.viterbi(truthPool[i].tokens, f, pre);
                wrongN += pre[truthPool[i].tokens.size()] == 0; // if it is decomposed, -> wrong
            }
        } 
        if (wrongP / totalP <= DISCARD) {
            left_alpha = alpha;
        } else {
            right_alpha = alpha;
        }
        wrongPositive = wrongP / (double)totalP;
        wrongNegative = wrongN / (double)totalN;
    }

    double resFR = wrongPositive;
    if (INTERMEDIATE) {
    	cerr << "---after binary alpha----" << endl;
    	cerr << "\talpha = " << alpha << " | beta = " << beta << endl;
    	cerr << "\twrongPositive = " << wrongPositive << endl;
    	cerr << "\twrongNegative = " << wrongNegative << endl;
    }

    double left_beta = 1, right_beta = 200;
    for (int _ = 0; _ < 10; _ ++) {
    	beta = (left_beta + right_beta) / 2;
    	// calculate probability to to segment each pattern to 1 part
        Segmentation segmentation(alpha,beta);
        segmentation.rectifyFrequency(Documents::sentences);
        double wrongP = 0, totalP = 0, wrongN = 0, totalN = 0;
        for (int i = 0; i < truthPool.size(); ++ i) {
            if (truthPool[i].label == 1) {
                ++ totalP;
                vector<double> f;//f is dp array
                vector<int> pre;//pre is trace array
                segmentation.viterbi(truthPool[i].tokens, f, pre);
                wrongP += pre[truthPool[i].tokens.size()] != 0; // if it is decomposed, -> wrong
            } else if (truthPool[i].label == 0) {
            	++ totalN;
                vector<double> f;//f is dp array
                vector<int> pre;//pre is trace array
                segmentation.viterbi(truthPool[i].tokens, f, pre);
                wrongN += pre[truthPool[i].tokens.size()] == 0; // if it is decomposed, -> wrong
            }
        } 
        if ( abs(resFR - (wrongP / totalP)) <= 5*DISCARD) {
            left_beta = beta;
        } else {
            right_beta = beta;
        }
        wrongPositive = wrongP / (double)totalP;
        wrongNegative = wrongN / (double)totalN;
    }

    if (INTERMEDIATE) {
    	cerr << "---after binary beta----" << endl;
    	cerr << "\talpha = " << alpha << " | beta = " << beta << endl;
    	cerr << "\twrongPositive = " << wrongPositive << endl;
    	cerr << "\twrongNegative = " << wrongNegative << endl;
    }
}

#endif