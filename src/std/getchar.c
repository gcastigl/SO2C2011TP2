#include "stdio.h"

#define STREAM_SIZE 500

typedef int (*flusher)(char * streampointer);

char stream[STREAM_SIZE];
char * streamout=stream;


int intro_flush(char * streampointer){
	if(*streampointer=='\n' || 1>=STREAM_SIZE-(streampointer-stream)-1){
		return 1;
	}
	
	return 0;
}

char getchar(){
	char c=*streamout;
	if(c=='\0'){
		streamout=stream;
		char * streamin=stream;
		int i,j;
		for(i=0;i<STREAM_SIZE;i++){
			stream[i]='\0';	
		}
		while(!intro_flush(streamin)){
			if(*streamin!='\0')
				streamin++;
			__read(0,streamin,1);
			if(*streamin!='\b' && *streamin!='\t'){
				printf(streamin);
			}
			else if(*streamin=='\b'){
				if(streamin > stream){
					printf("\b");
					*streamin='\0';
					streamin--;
				}
				*streamin='\0';
                        } else if(*streamin=='\t'){
                            *streamin='\0';
                        }
		}
		c=*streamout;
	}
	streamout++;
	return c; 
	
}


