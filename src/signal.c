#include <signal.h>
#include <defs.h>
#include <util/circularBuffer.h>
#include <tty.h>
#include <process/process.h>
#include <session.h>
#include <util/logger.h>

void checkReset();
void checkTTY();

static int newTTY = -1;

void signal_keyPressed(char c) {
	TTY* tty = tty_getCurrentTTY();
    PROCESS* tty_process = scheduler_getProcess(tty ->pid);
	PROCESS** allProc = scheduler_getAllProcesses();
    if (tty_process->status == BLOCKED) {
        if (tty_process->waitingFlags == W_CHILD) {
            // Current TTY is waiting for a child, see if any child is waiting for input
            for (int i = 0; i < MAX_PROCESSES; i++) {
                PROCESS* p = allProc[i];
                if (p != NULL && p->tty == tty->id &&
                        p->status == BLOCKED && p->waitingFlags == W_INPUT) {
                    if (IS_CTRL() && c == 'c') {
                        kill(p->pid);
                        return;
                    } else {
                        circularBuffer_add(&tty->input_buffer , c);
                        scheduler_setStatus(p->pid, READY);
                        log(L_DEBUG, "Sending  input to process: %s", p->name);
                        return;
                    }
                }
            }
        }
        circularBuffer_add(&tty->input_buffer , c);
        if (tty_process->waitingFlags == W_INPUT) {
            // if Current TTY is waiting for input, then wake it up
            scheduler_setStatus(tty_getCurrentTTY()->pid, READY);
        }
    }
}

void signal_specialKeyPressed() {
    checkReset();
    checkTTY();
}

void checkReset() {
	if (IS_CTRL() && IS_ALT() && IS_DEL()) {
		_reset();
	}
}

void checkTTY() {
    if (!session_isLoggedIn()) {
        return;
    }
	if (IS_CTRL() && newTTY == -1) {
		for (int i = 0; i < MAX_TTYs; ++i) {
			if (F_PRESSED(i)) {
				newTTY = i;
				break;
			}
		}
	}
	if (!IS_CTRL() && newTTY != -1) {
		if (newTTY != tty_getCurrent()) { // Do not switch to the same tty!
			tty_setCurrent(newTTY);
		}
		newTTY = -1;
	}
}

int signal(int signum) {
    PROCESS **processes = scheduler_getAllProcesses();
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i] != NULL && processes[i]->status == BLOCKED && processes[i]->waitingFlags == signum) {
            // log(L_DEBUG, "waking up %s / waiting: %d", processes[i]->name, processes[i]->waitingFlags);
            scheduler_setStatus(processes[i]->pid, READY);
        }
    }
    return 0;
}

