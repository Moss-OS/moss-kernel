#include <stdint.h>
#include "common/utils.h"
#include "common/pi.h"
#include "bootstrapper/mm.h"
#include "peripherals/base.h"

int pi_ver;
uint64_t device_base;
uint64_t pbase;

int set_pi_ver (void) {
	uint32_t cpu_id = get_core_info();
	if (cpu_id == 0x410FD034) {
		return 3;   // pi2 & 3
	} else if (cpu_id == 0x410FD083) {
		return 4;   // pi4
	} else {
		return 0;	// unknown
	}
}

void init_peripheral_bases(void) {
	switch (pi_ver) {
		case 4:
			device_base = PI4_DEVICE_BASE;
			break;
		case 3:
		default:
			device_base = PI3_DEVICE_BASE;
			break;
	}
	pbase = VA_START + device_base;
}