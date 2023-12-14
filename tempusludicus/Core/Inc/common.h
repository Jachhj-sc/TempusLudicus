#ifndef COMMON_H
#define COMMON_H

#include <MKL25Z4.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "delay.h"
#include "sysTick.h"

#define F_CPU DEFAULT_SYSTEM_CLOCK

#define MASK(x) (1UL << (x))

#endif /* COMMON_H */
