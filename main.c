#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "queue.h"
#include "urls.h"
#include "reghelp.h"

int main(int argc, char *argv[]) {
	
	char server_reply[100001];
	char current_url[1001];
	char current_host[1001];
	char pending_url[1001];
	queue* url_queue = init_queue();
	queue* visited_queue = init_queue();
	queue* pending_queue;
	
	enqueue(url_queue, argv[1]);
    
	while(!is_empty(url_queue)) {
		dequeue(url_queue, current_url);
		enqueue(visited_queue, current_url);
		remove_path(current_url, current_host);
		
		// http_get threw an error of some kind, move on to next url
		if(http_get(current_url, server_reply) != 0) {
			continue;
		}
		
		// don't fetch non-html files
		if(!istype_html(server_reply)) {
			continue;
		}
		
		// only parsing OK codes
		if(get_reply_code(server_reply) == 200) {
			pending_queue = get_url(server_reply, current_host);
			while(!is_empty(pending_queue)) {
				dequeue(pending_queue, pending_url);
				if(contains(visited_queue, pending_url) || contains(url_queue, pending_url)) {
					continue;
				}
				enqueue(url_queue, pending_url);
			}
			
			free_queue(pending_queue);
		}
	}
	
	free_queue(visited_queue);
	free_queue(url_queue);
	
	return 0;
}
