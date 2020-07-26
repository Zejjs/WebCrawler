#ifndef URLS
#define URLS

void make_absolute_inplace(char* url, char* host);
queue* get_url(char* html, char* host);
int valid_url(char* url, char* host);

#endif