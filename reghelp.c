#include <string.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

int matches(char* astring, char* regexpattern) {
	regex_t regex;
	regcomp(&regex, regexpattern, REG_EXTENDED);
	int match = 0;
	if(regexec(&regex, astring, 0, NULL, 0) == 0) {
		match = 1;
	}
	regfree(&regex);
	return match;
}
