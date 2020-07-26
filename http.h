#ifndef HTTPLIB
#define HTTPLIB

int get_ip_from_url(char*url, char* destination);
int http_get (char* url, char* destination);
void host_path_split(char* url, char* host, char* path);
int get_content_length(char* reply);
int get_header_length(char* reply);
void remove_path(char* url_in, char* destination);
int get_reply_code(char* reply);
int istype_html(char* http_response);

#endif
