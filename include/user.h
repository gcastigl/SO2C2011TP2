#ifndef USER_H_
#define USER_H_

#include "video.h"

#define NAME_MAX_LENGTH	20

void user_doLogin();

int user_isLoggedIn();

const char* user_getName();

void user_logout();

#endif
