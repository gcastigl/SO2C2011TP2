#include <lib/string.h>

size_t strlen(char* string) {
	size_t count = 0;
	while (string[count] != 0) {
		count++;
	}
	return count;
}

int strcmp(const char * s1, const char * s2) {
	int ret = 0;
	while(ret == 0 && (*s1 != '\0' || *s2 != '\0')) {
		ret = *s1 - *s2;
		s1++;s2++;
	}
	return ret;
}

int strncmp(char * s1, char * s2, unsigned int n) {
	int ret = 0;
	int i;
	
	for(i=0;ret == 0 && i<=n && (*s1 != '\0' || *s2 != '\0'); i++)
		ret = *s1 - *s2;
		
	return ret;
}


/*Retorna true si s1 es subString de s2*/
int substr(const char * s1, const char *s2) {
	int i = 0, isSubstr = true;
	while(isSubstr && s1[i] != '\0') {
		if(s1[i] != s2[i]) {
			isSubstr = false;
		}
		i++;
	}
	return isSubstr;
}

void strcpy(char* to, char* from) {
	int len = strlen(from) + 1;
	memcpy(to, from, len);
}

void *memset(void *s, int c, size_t n) {
    unsigned char* p=s;
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}
