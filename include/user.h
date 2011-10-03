#ifndef USER_H
#define USER_H

#include <tty.h>
#include <shell.h>
#include <driver/video.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define NAME_MAX_LENGTH	20

void user_doLogin();

int user_isLoggedIn();

const char* user_getName();

void user_logout();

#endif
