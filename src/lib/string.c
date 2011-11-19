#include <lib/string.h>
#include <lib/stdlib.h>

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

int strspn(const char *string, const char *in) {
	register const char *s1, *s2;

	for (s1 = string; *s1; s1++) {
		for (s2 = in; *s2 && *s2 != *s1; s2++)
			/* EMPTY */;
		if (*s2 == '\0')
			break;
	}
	return s1 - string;
}

char *strtok(register char *string, const char *separators) {
	register char *s1, *s2;
	static char *savestring = NULL;

	if (string == NULL) {
		string = savestring;
		if (string == NULL)
			return (char *) NULL;
	}

	s1 = string + strspn(string, separators);
	if (*s1 == '\0') {
		savestring = NULL;
		return (char *) NULL;
	}

	s2 = strpbrk(s1, separators);
	if (s2 != NULL)
		*s2++ = '\0';
	savestring = s2;
	return s1;
}

char *strpbrk(register const char *string, register const char *brk) {
	register const char *s1;

	while (*string) {
		for (s1 = brk; *s1 && *s1 != *string; s1++)
			/* EMPTY */;
		if (*s1)
			return (char *) string;
		string++;
	}
	return (char *) NULL;
}
