#include <signal.h>
#include <defs.h>
#include <util/circularBuffer.h>
#include <tty.h>
#include <process/process.h>

void checkReset();
void checkTTY();

static int newTTY = -1;

void signal_keyPressed() {
	TTY* tty = tty_getCurrentTTY();
	char c = getKeyFromBuffer();
	PROCESS** all = scheduler_getAllProcesses();
	// If there is a process child of current TTY, give input to that process
	// else give input to current TTY
	for (int i = 0; i < MAX_PROCESSES; i++) {
		PROCESS* p = all[i];
		if (p != NULL && p->status == BLOCKED && p->waitingInfo == W_INPUT) {
			log(L_DEBUG, "Sending  input to process: %s", p->name);
			return;
		}
	}
    circularBuffer_add(&tty->input_buffer , c);
	PROCESS* tty_process = scheduler_getProcess(tty ->pid);
    if (tty_process->status == BLOCKED && tty_process->waitingFlags == W_INPUT) {
        // Only notify tty if it is waiting for an input!
        scheduler_setStatus(tty_getCurrentTTY()->pid, READY);
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
            log(L_DEBUG, "waking up %s / waiting: %d", processes[i]->name, processes[i]->waitingFlags);
            scheduler_setStatus(processes[i]->pid, READY);
        }
    }
    return 0;
}
