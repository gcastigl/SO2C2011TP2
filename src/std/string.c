
int strcmp(char* str1, char * str2) {
    int i;
    for (i = 0; str1[i] != '\0' && str1[i] != '\0'; i++) {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
    }
    if (str1[i] == '\0' && str2[i] == '\0') {
        return str1[i] - str2[i];
    }
    return 1;
}

void strcpy(char * str_des, char * str_ori) {
    int i;
    for (i = 0; str_ori[i] != '\0'; i++) {
        str_des[i] = str_ori[i];
    }
    str_des[i] = '\0';
}

void strncpy(char * str_des, char * str_ori, unsigned int count) {
    int i;
    for (i = 0; str_ori[i] != '\0' && i<=count; i++) {
        str_des[i] = str_ori[i];
    }
    str_des[i] = '\0';
}

int strlen(char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++);
    return i;
}

