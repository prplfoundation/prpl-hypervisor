/*
Copyright (c) 2016, prpl Foundation

Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
fee is hereby granted, provided that the above copyright notice and this permission notice appear 
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

*/

#ifndef _HAL_H_
#define _HAL_H_

#include <mips_cp0.h>

#ifdef ETHERNET_SUPPORT
#include <ethernet.h>
#endif 

int32_t isRootMode();
int32_t hasVZ();
int32_t ConfigureGPRShadow();
int32_t LowLevelProcInit();
uint32_t getCauseCode();
uint32_t getHypercallCode();
uint32_t getCauseBD();
uint32_t getGCauseCode();
uint32_t getInterruptPending();
void confTimer(uint32_t quantum);
void setInterruptMask(uint32_t im);
void clearInterruptMask(uint32_t im);
void enableIE();
uint32_t hasGuestID();
uint32_t isDirectRoot();
void setEPC(uint32_t epc);
uint32_t getEPC();
void setGuestRID(uint32_t guestrid);
void setGuestID(uint32_t guestid);
uint32_t getGuestID(void);
uint32_t isRootASID();
void setGuestMode();
void setStatusReg(uint32_t bits);
uint32_t has1KPageSupport();
void Disable1KPageSupport();
int32_t isEnteringGuestMode();
uint32_t getBadVAddress();
uint32_t MoveFromPreviousGuestGPR(uint32_t reg);
void MoveToPreviousGuestGPR(uint32_t reg, uint32_t value);
void setLowestGShadow(uint32_t lowestshadow);
uint32_t getLowestGShadow(void);
void setPreviousShadowSet(uint32_t shadow_set);
uint32_t getPreviousShadowSet();
uint32_t getCounter(void);
void setGTOffset(int32_t gtoffset);
void setGuestCTL2(uint32_t guestclt2);
uint32_t getRandom();

#endif /* _HAL_H_ */
