#ifndef __TERNARY_SEARCH__
#define __TERNARY_SEARCH__

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
	double lf = pow(100,f*f) + pow(100,g*g) - 2;

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

void ternary_search(double &alpha,double &beta,vector<Pattern> truthPool)
{
	double left_alpha = 0.05, right_alpha = 1;
	double left_beta = 1, right_beta = 1000;

	double lf1, lf2, lf3, lf4;
	double min_lf = (double)INF;

	int round = 50;
	for (int _ = 0; _ < round; _++){
		double alpha1 = (left_alpha * 2 + right_alpha) / 3.0;
		double alpha2 = (left_alpha + right_alpha * 2) / 3.0;

		double beta1 = (left_beta * 2 + right_beta) / 3.0;
		double beta2 = (left_beta + right_beta * 2) / 3.0;

		lf1 = calc_loss_function(alpha1,beta1,truthPool,(_==round-1));
		lf2 = calc_loss_function(alpha2,beta1,truthPool,(_==round-1));
		lf3 = calc_loss_function(alpha1,beta2,truthPool,(_==round-1));
		lf4 = calc_loss_function(alpha2,beta2,truthPool,(_==round-1));

		min_lf = min(lf1,min(lf2,min(lf3,lf4)));

		if ( abs(min_lf - lf4) < EPS ) {
			right_alpha = alpha2;
			right_beta = beta2;
		} else if ( abs(min_lf - lf3) < EPS ) {
			left_alpha = alpha1;
			right_beta = beta2;
		} else if ( abs(min_lf - lf2) < EPS ) {
			right_alpha = alpha2;
			left_beta = beta1;
		} else if ( abs(min_lf - lf1) < EPS ) {
			left_alpha = alpha1;
			left_beta = beta1;
		}
	}

	alpha = left_alpha;
	beta = left_beta;

	fprintf(stderr, "====Ternary Search:=====\n");
	fprintf(stderr, "\tLoss function = %.4f\n",min_lf);
	fprintf(stderr, "\tAlpha = %.4f\n",alpha);
	fprintf(stderr, "\tBeta = %.4f\n",beta);
}

#endif