#ifndef KEYBOARDLISTENER_H
#define KEYBOARDLISTENER_H

#define MAX_SCAN_CODE 300

#define CTRL_KEY_PRESED_SCAN_CODE 29
#define CTRL_KEY_RELESED_SCAN_CODE 157

#define ALT_KEY_PRESED_SCAN_CODE 56
#define ALT_KEY_RELESED_SCAN_CODE 184

typedef int (*key_listener)();

int activate(int scan_code);
void add_key_listener(int mode, int scan_code, key_listener listener);
void init_key_listeners();

#endif //KEYBOARDLISTENER_H
