#include "common/utils.h"
#include "common/pi.h"

int set_pi_ver (void) {
	unsigned int cpu_id = get_core_info();
	if (cpu_id == 0x410FD034) {
		return 3;   // pi2 & 3
	} else if (cpu_id == 0x410FD083) {
		return 4;   // pi4
	} else {
		return 0;	// unknown
	}
}