#include "process/user_sys.h"
#include "process/user.h"

void loop(char* str) {
	char buf[2] = {""};
	while (1){
		for (int i = 0; i < 5; i++){
			buf[0] = str[i];
			call_sys_write(buf);
			user_delay(1000000);
		}
	}
}

void user_process() {
	call_sys_write("User process started\n\r");
	
	// Test page fault handling - access unmapped memory
	call_sys_write("Testing page fault...\n\r");
	volatile char *test_ptr = (char *)0x400000; // Unmapped user VA
	*test_ptr = 'X'; // Should trigger page fault and allocate page
	if (*test_ptr == 'X') {
		call_sys_write("Page fault test PASSED - page allocated!\n\r");
	}
	
	int pid = call_sys_fork();
	if (pid < 0) {
		call_sys_write("Error during fork\n\r");
		call_sys_exit();
		return;
	}
	if (pid == 0) {
		loop("abcde");
	} else {
		loop("12345");
	}
}