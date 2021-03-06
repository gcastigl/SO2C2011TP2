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

int strindex(char* str, char c, int offset) {
	boolean searchBackwards = false;
	if (offset < 0) {	// searching backwards...
		offset = -offset;
		searchBackwards = true;
	}
	int i = offset;
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

int strreplace(char* str, char replace, char replacement) {
	int replaces = 0;
	while(*str != '\0') {
		if (*str == replace) {
			*str = replacement;
			replaces++;
		}
		str++;
	}
	return replaces;
}

char *strcat(char *ret, register const char *s2) {
    register char *s1 = ret;

    while (*s1++ != '\0')
        /* EMPTY */ ;
    s1--;
    while ((*s1++ = *s2++))
        /* EMPTY */ ;
    return ret;
}

char* strjoin(char* strings[], char* seperator, int count) {
    char* str = NULL;             /* Pointer to the joined strings  */
    unsigned int total_length = 0;      /* Total length of joined strings */
    int i = 0;                    /* Loop counter                   */

    /* Find total length of joined strings */
    for (i = 0; i < count; i++) total_length += strlen(strings[i]);
    total_length++;     /* For joined string terminator */
    total_length += strlen(seperator) * (count - 1); // for seperators

    str = (char*) malloc(total_length);  /* Allocate memory for joined strings */
    str[0] = '\0';                      /* Empty string we can append to      */

    /* Append all the strings */
    for (i = 0; i < count; i++) {
        strcat(str, strings[i]);
        if (i < (count - 1)) strcat(str, seperator);
    }

    return str;
}
