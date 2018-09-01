/* Syntax analyzer of Modula-2 translator.
 * Copyright 1993, 1994, 1995, 1996, 1997 Vladimir Makarov
 * This file is part of m2c.
 *
 * m2c is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * m2c is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with m2c. If not, see <http://www.gnu.org/licenses/>.
 */

/*   COROUTINE PROCEDURES                                             */
/*                                                                    */
/* _m2_currcor, _m2_maincor contains address of current/main coroutine*/
/*                                                                    */
/* _m2_newcor contains address of new coroutine                       */
/*    (set up by _m2_newpr1 for newprocess)                           */
/*                                                                    */
/* _m2_nprcadr, _m2_nprclong contains address,long of coroutine region*/
/*    (set up by newprocess for _m2_newpr1)                           */
/*                                                                    */
.globl _m2_newcor
.globl _m2_currcor
.globl _m2_maincor
.globl _m2_nprcadr
.globl _m2_nprclong
/*                                                             */
/* descriptor of main coroutine                                */
/*  (analogous but not identious to other coroutines)          */
/*      ----------------------                                 */
/*   0 |      0               | <--- coroutine address         */
/*     |----------------------|                                */
/*  -4 |      0               | is not used                    */
/*     |----------------------|                                */
/* -10 |      0               | is not used                    */
/*     |----------------------|                                */
/* -14 |    reserved          |                                */
/*     |----------------------|                                */
/* -20 | 0 (flag of main cor) |                                */
/*     |----------------------|                                */
/* -24 | priority             | is not implemented             */
/*     |----------------------|                                */
/* -30 | saved  pc            |                                */
/*     |----------------------|                                */
/* -34 | saved  sp            |                                */
/*     |----------------------|                                */
/* -40 | 0                    | is not used                    */
/*     |----------------------|                                */
/* -44 | saved r11            |                                */
/*     |----------------------|                                */
/* -50 | saved r10            |                                */
/*     |----------------------|                                */
/* -54 | saved r9             |                                */
/*     |----------------------|                                */
/* -60 | saved r8             |                                */
/*     |----------------------|                                */
/* -64 | saved r7             |                                */
/*     |----------------------|                                */
/* -70 | saved r6             |                                */
/*     |----------------------|                                */
/* -74 | saved r5             |                                */
/*     |----------------------|                                */
/*-100 | saved r4             |                                */
/*     |----------------------|                                */
/*-104 | saved r3             |                                */
/*     |----------------------|                                */
/*-110 | saved r2             |                                */
/*     |----------------------|                                */
/*-114 | 0                    | is not used                    */
/*     |----------------------|                                */
/*-120 | saved fp of the cor  |                                */
/*     |----------------------|                                */
/*-124 | saved ap of the cor  |                                */
/*     |----------------------|                                */
/*-130 | 0                    | is not used                    */
/*     |----------------------|                                */
/*-134 | 0                    | is not used                    */
/*      ----------------------                                 */
/*                                                             */
.data
.align 2
 .lcomm maindesc,0160
.data
.align 2
_m2_maincor:
 .long maindesc+0140
_m2_currcor:
 .long maindesc+0140
.text
.align 2
.globl _exit
/*                                                    */
/* restore currfr of coroutine                        */
/*  (see epilog of procedure where currfr is changed) */
/*                                                    */
corfin: 
 .word 0
 pushl $0
 calls $1,_exit
/*                                                                       */
/* coroutine descriptor:                                                 */
/*                                                                       */
/*      -----------------------------------------------                  */
/*   0 | address of pointer to var (see m2_newpr1)     |<--  cor. address*/
/*     |-----------------------------------------------|                 */
/*  -4 | saved value of the pointer of coroutine       |                 */
/*     |_______________________________________________|                 */
/* -10 |   saved the value (i.e. out of the corout.)   |                 */
/*     |_______________________________________________|                 */
/* -14 |             reserved                          |                 */
/*     |-----------------------------------------------|                 */
/* -20 |             reserved                          |                 */
/*     |-----------------------------------------------|                 */
/* -24 |          priority of the cor                  |is'nt implemented*/
/*     |-----------------------------------------------|                 */
/* -30 |         saved  pc of the cor.                 |                 */
/*     |-----------------------------------------------|                 */
/* -34 |          saved  sp of the cor.                |                 */
/*     |-----------------------------------------------|                 */
/* -40 |          0 (number of arguments)              | <-ap            */
/*     |-----------------------------------------------|                 */
/* -44 |                 saved r11 of the cor.         |                 */
/*     |-----------------------------------------------|                 */
/* -50 |                 saved r10 of the cor.         |                 */
/*     |-----------------------------------------------|                 */
/* -54 |                 saved r9 of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/* -60 |                 saved r8 of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/* -64 |                 saved r7 of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/* -70 |                 saved r6 of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/* -74 |                 saved r5 of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/*-100 |                 saved r4 of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/*-104 |                 saved r3 of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/*-110 |                 saved r2 of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/*-114 |   $corfin (for return from the cor.)          |                 */
/*     |-----------------------------------------------|                 */
/*-120 |                 saved fp of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/*-124 |                 saved ap of the cor.          |                 */
/*     |-----------------------------------------------|                 */
/*-130 |    mask & PSW                                 |                 */
/*     |-----------------------------------------------|                 */
/*-134 |    0                                          | <-fp,sp         */
/*     |-----------------------------------------------|                 */
/*     |                                               |                 */
/*     |  |    stack                                   |                 */
/*     |  V    of the cor.                             |                 */
/*     |                                               | <-- address of  */
/*      -----------------------------------------------  coroutine region*/
/*                                                                       */
/*    m2_newpr1(address of vars,address of the pointer):                 */
/*                                                                       */
/*      initiation of coroutine region                                   */
/*                                                                       */
/*                                                                       */
.globl _m2_newpr1
_m2_newpr1:
 .word 0
 movl *$_m2_nprcadr,r0
 addl2 *$_m2_nprclong,r0
 subl2 $1,r0
 bicl2 $3,r0
 movl r0,*$_m2_newcor /*m2_newcor=(m2_nprcadr+m2_nprclong-1)/4*4 - coroutine address */
 movl 010(ap),(r0)
 movl *$_m2_newcor,r1
 subl2 $0120,r1
 addl2 4(ap),r1
 subl2 014(fp),r1 /* 014(fp) fp in coroutine procedure */
 movl r1,-4(r0) /*m2_newcor-0120+4(ap)-014(fp):adr of vars in coroutine stack*/
 movl $0,-040(r0) /* number of arguments */
 movl r11,-044(r0)
 movl r10,-050(r0)
 movl r9,-054(r0)
 movl r8,-060(r0)
 movl r7,-064(r0)
 movl r6,-070(r0)
 movl r5,-074(r0)
 movl r4,-0100(r0)
 movl r3,-0104(r0)
 movl r2,-0110(r0)
 movl 020(fp),-030(r0) /* cor will be resumed after m2_newpr1 */
 movl *$_m2_newcor,r1
 subl2 $0120,r1
 addl2 ap,r1
 addl2 $014,r1 /* because 2 arguments & its number in frame for m2_newpr1 */
 subl2 014(fp),r1
 movl r1,-034(r0) /*m2_newcor-0120+ap+14*/
 movl $corfin,-0114(r0) /* return from corr */
 movl 014(fp),-0120(r0) /*saved fp*/
 movl 010(fp),-0124(r0) /*saved ap*/
 movl $05777000000,-0130(r0) /*PSW & mask*/
 movl $0,-0134(r0)
 movl 014(fp),fp
 ret
/*                                    */
/* m2_pasivcor()                      */
/*   passivate current coroutine      */
/*                                    */
.globl _m2_pasivcor
.data
.align 2
m13to2:
 .word 037774
mask:
 .word 0
.text
_m2_pasivcor:
 .word 0
 movl *$_m2_currcor,r0
 tstl (r0)
 beql 1f /* cor procedure hasn't pointer to vars */
 movl *(r0),-4(r0) /* save pointer to vars (of cor) */
 movl -010(r0),*(r0) /* restore pointer to vars */
1:
 movl 014(fp),r1
 movl 020(r1),-030(r0) /* cor is resumed after m2_transfer */
 movl *010(fp),r1
 addl2 $1,r1
 mull2 $4,r1
 addl2 010(fp),r1
 movl r1,-034(r0) /* save sp */
 pushr *$m13to2
 movl sp,r1
 movl 014(fp),fp /*fp in m2_transfer */
 movw 6(fp),*$mask
 bicw2 $0170000,*$mask
 movl fp,sp
 addl2 $024,sp
 popr *$mask
 movl r11,-044(r0)
 movl r10,-050(r0)
 movl r9,-054(r0)
 movl r8,-060(r0)
 movl r7,-064(r0)
 movl r6,-070(r0)
 movl r5,-074(r0)
 movl r4,-0100(r0)
 movl r3,-0104(r0)
 movl r2,-0110(r0)
 movl r1,sp
 popr *$m13to2
 movl 014(fp),r1
 movl 010(r1),-0124(r0) /* save ap */
 movl 014(r1),-0120(r0) /* save fp */
 ret
/*                                    */
/*  m2_activcor()                     */
/*    activate current coroutine      */
/*                                    */
.globl _m2_activcor
_m2_activcor:
 .word 0
 movl *$_m2_currcor,r0
 tstl (r0)
 beql 1f /* cor proc hasn't pointer to vars */
 movl *(r0),-010(r0) /* save pointer to vars */
 movl -4(r0),*(r0) /* restore pointer to vars for cor */
1:
 movl -034(r0),sp
 movl -044(r0),r11
 movl -050(r0),r10
 movl -054(r0),r9
 movl -060(r0),r8
 movl -064(r0),r7
 movl -070(r0),r6
 movl -074(r0),r5
 movl -0100(r0),r4
 movl -0104(r0),r3
 movl -0110(r0),r2
 movl -0120(r0),fp
 movl -0124(r0),ap
 jmp *-030(r0)
