#include "utils.h"

int get_raspi_ver (void) {
	unsigned int cpu_id = get_core_info();
	if (cpu_id == 0x410FD034) {
		return RASPI3;   // raspi2 & 3
	} else if (cpu_id == 0x410FD083) {
		return RASPI4;   // raspi4
	} else {
		return 0;// Unknown
	}
}