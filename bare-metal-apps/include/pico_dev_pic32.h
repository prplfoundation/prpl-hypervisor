/*********************************************************************
   PicoTCP. Copyright (c) 2012-2015 Altran Intelligent Systems. Some rights reserved.
   See LICENSE and COPYING for usage.

 *********************************************************************/
#ifndef INCLUDE_PICO_PIC32
#define INCLUDE_PICO_PIC32
#include "pico_config.h"
#include "pico_device.h"

struct pico_device *pico_eth_create(const char *name, const uint8_t *mac);

#endif

