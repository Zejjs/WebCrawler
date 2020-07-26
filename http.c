#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <arpa/inet.h>
#include "reghelp.h"
#define PORT 80

int get_ip_from_url(char*url, char* destination);
void host_path_split(char* url, char* host, char* path);
int get_content_length(char* reply);
int get_header_length(char* reply);
void remove_path(char* url_in, char* destination);
int get_reply_code(char* reply);
int istype_html(char* http_response);

int http_get (char* url, char* destination) {

    char host[1001] = "";
    char path[1001] = "";
    char hostip[256] = "";
    char server_reply[100001] = "";
	
	// split the provided url into host and path parts and stores them in respective arrays
    host_path_split(url, host, path);
	
	if (get_ip_from_url(host, hostip) < 0) {
		fprintf(stderr, "Failed to obtain ip for host");
        return -1;
    }

    printf("http://%s%s\n", host, path);
	
	
	// create the socket
    int socket_desc;
    struct sockaddr_in server;
    memset(&server, 0 ,sizeof(server));
    char message[4096] = "";

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1){
        fprintf(stderr, "Error creating socket\n");
        return(-1);
    }

    // connect using socket
    server.sin_addr.s_addr = inet_addr(hostip);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if(connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0){
        fprintf(stderr, "Error connecting\n");
		close(socket_desc);
        return -1;
    }

    // send the message
    sprintf(message, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: aponiatowski\r\n\r\n", path, host);

    // check for errors sending data
    if(send(socket_desc, message, strlen(message), 0) < 0){
        fprintf(stderr, "Error sending data\n");
		close(socket_desc);
        return -1;
    }
    // initial receive
    int bytes_received = recv(socket_desc, server_reply, sizeof(server_reply), 0);
    if (bytes_received <= 0) {
        fprintf(stderr, "Error receiving data\n");
		close(socket_desc);
        return -1;
    }
    
    int new_bytes = 0;
	
    // check if we got the entire header, if not recv more
    while(!matches(server_reply, "\r\n\r\n")) {
        new_bytes = recv(socket_desc, server_reply+bytes_received, sizeof(server_reply)-bytes_received, 0);
        if (new_bytes <= 0) {
            fprintf(stderr, "Received incomplete header\n");
    		close(socket_desc);
            return -1;
        }
        bytes_received += new_bytes;
    }
    
    // check whether reply is html
    if (!istype_html(server_reply)) {
        close(socket_desc);
        return -1;
    }
    
	// extract the expected length of the reply
    int content_length = get_content_length(server_reply);
    int header_length = get_header_length(server_reply);
	
	// if we received less than expected, ask for more
    while (bytes_received < content_length + header_length) {
        new_bytes = recv(socket_desc, server_reply+bytes_received, sizeof(server_reply)-bytes_received, 0);
        if (new_bytes <= 0) {
            fprintf(stderr, "Error receiving data\n");
    		close(socket_desc);
            return -1;
        }
        bytes_received += new_bytes;
    }
	
	// copy the response to destination and return 0 for success
	strcpy(destination, server_reply);
	
    return 0;
}

// standard gedaddrinfo implementation based on https://github.com/angrave/SystemProgramming/wiki/Networking,-Part-2:-Using-getaddrinfo
int get_ip_from_url(char*url, char* destination) {
    struct addrinfo hints, *iptr;
    memset(&hints, 0 ,sizeof(hints));

    hints.ai_family = AF_INET;

    int result = getaddrinfo(url, NULL, &hints, &iptr);
    if (result) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        return -1;
    }

    char host[256];
    getnameinfo(iptr->ai_addr, iptr->ai_addrlen, host, sizeof (host), NULL, 0, NI_NUMERICHOST);
    strcpy(destination, host);
    freeaddrinfo(iptr);
    return 0;
}
// extracts the value at the content length header of http reply
int get_content_length(char* reply) {
    regex_t regex;
    regmatch_t match;
    regcomp(&regex, "Content-Length: [0-9]*", REG_EXTENDED);

    if(regexec(&regex, reply, 1, &match, 0) != 0) {
        regfree(&regex);
        return -1;
    }

    int start = match.rm_so + 16;
    int end = match.rm_eo;
    char* conlen = malloc(end-start+1);
    for(int i = 0; i < end-start; i++) {
        conlen[i] = reply[start+i];
    }
    conlen[end-start] = '\0';

	regfree(&regex);
	int content_length = atoi(conlen);
	free(conlen);
    return content_length;
}

int get_header_length(char* reply) {
    int headerlen = 0;
    while(1) {
        if(reply[headerlen] == '\r' && reply[headerlen+1] == '\n'
        && reply[headerlen+2] == '\r' && reply[headerlen+3] == '\n'){
            return headerlen+4;
        }
        headerlen++;
    }
}

int get_reply_code(char* reply) {
    char code[4];

    code[0] = reply[9];
    code[1] = reply[10];
    code[2] = reply[11];
    code[3] = '\0';

    return atoi(code);
}

// splits the provided url into two parts, and stores them at provided pointers
void host_path_split(char* url, char* host, char* path) {
    char* buffer = malloc(strlen(url)+1);
    int i = 0;
    int urli = 0;
    if(matches(url, "http://")) {
        urli = 7;
    }

    while(url[urli] != '\0' && url[urli] != '/') {
        buffer[i] = url[urli];
        i++;
        urli++;
    }

    if(url[urli] == '\0') {
        buffer[i] = '\0';
        strcpy(host, buffer);
        strcpy(path, "/");
    }

    else if (url[urli] == '/') {
        buffer[i] = '\0';
        strcpy(host, buffer);
        i = 0;
        while (url[urli] != '\0') {
            buffer[i] = url[urli];
            i++;
            urli++;
        }
        buffer[i] = '\0';
        strcpy(path, buffer);
    }
    free(buffer);
}

int istype_html(char* http_response) {
    return matches(http_response, "Content-Type: text/html");
}

// removes the file path from a url
void remove_path(char* url_in, char* destination){
    
    char* host = malloc(strlen(url_in)+1);
    strcpy(host, url_in);
    
    for(int i = 7; i < strlen(host); i++) {
        if (host[i] == '/') {
            host[i] = '\0';
            break;
        }
    }
    strcpy(destination, url_in);
    free(host);
}
