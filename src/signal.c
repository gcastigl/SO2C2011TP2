#include <signal.h>
#include <util/circularBuffer.h>
#include <tty.h>
#include <process/process.h>

void signal_keyPressed() {
	TTY* tty = tty_getCurrentTTY();
	char c = getKeyFromBuffer();
	log(L_DEBUG, "key pressed!! %c", c);
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
	log(L_DEBUG, "Sending  input to current TTY: %d", tty->id);
	circularBuffer_add(&tty->input_buffer , c);
	scheduler_setStatus(tty_getCurrent(), READY);
}
