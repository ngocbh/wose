#ifndef __PREPROCESSING_H__
#define __PREPROCESSING_H__

void preprocessing(string input,string output)
{
	FILE* in = tryOpen(input,"r");
	FILE* out = tryOpen(output,"w");

	while (getLine(in)) {
		string sline(line);
		UnicodeString uLine = sline.c_str();
		vector<UnicodeString> tokens = uSplitBy(uLine,u' ');
		for (int i = 0; i < tokens.size(); i++) {
			UnicodeString tmp = "";
			for (int j = 0; j < tokens[i].length(); j++) 
				if (PUNCTUATIONS.indexOf(tokens[i][j]) != -1) 
					tokens[i] = tokens[i].replace(j,1,(char16_t)'$');
			for (int j = tokens[i].length()-1; j >= 0; j--) 
				if (PUNCTUATIONS.indexOf(tokens[i][j]) != -1) 
					tokens[i] = tokens[i].replace(j,1,(char16_t)'$');
 			for (int j = 0; j < tokens[i].length(); j++) 
 				if ( tokens[i][j] != (char16_t)'$') 
 					tmp += tokens[i][j];

 			fprintf(out,"%s",tmp);
		}
		fprintf(out,"\n");
	}

}

#endif