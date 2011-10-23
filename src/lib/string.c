#include <lib/string.h>

u32int strlen(char* string) {
	u32int count = 0;
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
	
	for(int i = 0; ret == 0 && i <= n && (*s1 != '\0' || *s2 != '\0'); i++)
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
	while(*from != '\0') {
		*to++ = *from++;
	}
	*to = '\0';
}

void *memset(void *s, int c, u32int n) {
    unsigned char* p=s;
    while(n--)
        *p++ = (unsigned char) c;
    return s;
}

void strncpy(char *to, const char *from, int size) {
	while(size--) {
		*to++ = *from++;
	}
	*to = '\0';
}

int strIndexOf(char* str, char c, int startIndex) {
	boolean searchBackwards = false;
	if (startIndex < 0) {	// searching backwards...
		startIndex = -startIndex;
		searchBackwards = true;
	}
	int i = startIndex;
	while(str[i] != '\0' && i >= 0) {
		if (str[i] == c) {
			return i;
		}
		if (searchBackwards) i--;
		else i++;
	}
	return -1;
}

int strContains(char* str, char c) {
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] == c) {
			return true;
		}
	}
	return false;
}
