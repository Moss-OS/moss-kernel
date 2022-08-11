#ifndef	_UTILS_H
#define	_UTILS_H
#include <stdint.h>

#ifndef __ASSEMBLER__

extern unsigned int get_core_info ( void );
extern unsigned int get_core_id ( void );
extern void put32 ( unsigned long, unsigned int);
extern unsigned int get32 ( unsigned long );
extern void delay ( unsigned long );

#endif

enum {
	RASPI3 = 3,
	RASPI4 = 4,
};

int get_raspi_ver ( void );

#endif  /*_UTILS_H */