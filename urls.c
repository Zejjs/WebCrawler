#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "queue.h"
#include "reghelp.h"
#define MAX_MATCHES 1

void make_absolute_inplace(char* url, char* host);
void url_clean_inplace(char*);
int valid_url(char* url, char* host);

queue* get_url(char* html, char* host) {
	regex_t regex;
	regmatch_t match;
	queue* urlq = init_queue();
	
	regcomp(&regex, "<[aA] [^>]*(href|HREF)=\"[^\" ]*", REG_EXTENDED);
    regoff_t last_match = 0;
	int start = 0;
	int end = 0;
	int len = 0;
	
	int matchflag = 0;
	
	char url[100000];
	
	while(regexec(&regex, html+last_match, 1, &match, 0) == 0) {
		start = match.rm_so+last_match;
        end = match.rm_eo+last_match;
		len = end-start;
        last_match += match.rm_so+1;
		
		for(int i = 0; i < len; i++) {
            url[i] = html[start+i];
        }
        url[len] = '\0';
		url_clean_inplace(url);
		make_absolute_inplace(url, host);
		
		if(strlen(url) > 1000) {
		    continue;
		}
		
		if(valid_url(url, host)) {
			enqueue(urlq, url);
		}
		
	}
	
	regfree(&regex);
	
	return urlq;
}

void url_clean_inplace(char* dirty_url) {
	int start = 0;
	int end = 0;
	int len = 0;
	regex_t regex;
	regmatch_t match;
	regcomp(&regex, "(href|HREF)=\".*", REG_EXTENDED);
	regexec(&regex, dirty_url, 1, &match, 0);
	start = match.rm_so+6;
	end = match.rm_eo;
	len = end-start;

	char* clean_url = malloc( len+1);
	for(int i = 0; i < len; i++) {
		clean_url[i] = dirty_url[start+i];
	}
	clean_url[len] = '\0';
	
	strcpy(dirty_url, clean_url);
	free(clean_url);
	regfree(&regex);
}

void make_absolute_inplace(char* url, char* host) {
	if(matches(url, "http://")) {
		return;
	}
	
	if(url[0] == '/' && url[1] == '/') {
		char* abs_url = malloc(strlen(url) + 6);
		strcpy(abs_url, "http:");
		strcat(abs_url, url);
		strcpy(url, abs_url);
		free(abs_url);
		return;
	}
	if(url[0] == '/') {
		char* abs_url = malloc(strlen(url) + strlen(host) + 1);
		strcpy(abs_url, host);
		strcat(abs_url, url);
		strcpy(url, abs_url);
		free(abs_url);
		return;
	}
	else {
		char* abs_url = malloc(strlen(url) + strlen(host) + 2);
		strcpy(abs_url, host);
		strcat(abs_url, "/");
		strcat(abs_url, url);
		strcpy(url, abs_url);
		free(abs_url);
		return;
	}
	
}

int same_nonfirst_components(char* one, char* two) {
    int i = 0;
    while(one[i] != '.') {
        i++;
    }
	int j = 0;
	while(two[j] != '.') {
        j++;
    }
	
    while(one[i] != '/' && one[i] != '\0') {
        if(one[i] != two[j]) {
            return 0;
        }
        i++;
		j++;
    }
    if(two[j] == '/' || two[j] == '\0') {
        return 1;
    }
    return 0;
}

int is_http(char* url) {
	if(matches(url, "http://")) {
		return 1;
	}
	return 0;
}

int valid_url(char* url, char* host) {
	if(matches(url, "(\\./)|[(%%?#]")){
		return 0;
	}
	if(same_nonfirst_components(url, host) == 0) {
		return 0;
	}
	if(!is_http(url)) {
		return 0;
	}
	
	return 1;
}