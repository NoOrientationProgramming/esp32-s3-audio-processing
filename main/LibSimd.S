
// Input
// a0 .. Return address
// a1 .. Stack pointer (also sp)
// a2 .. pBase
// a3 .. offset

// Output
// a2 .. pBase + offset

.text
.align  4
.global dummyCalc
.type   dummyCalc,@function

dummyCalc:
	entry sp, 32

	add a2, a2, a3

	retw

// Input
// a0 .. Return address
// a1 .. Stack pointer (also sp)
// a2 .. pSrc
// a3 .. pDst

// Output
// Memory change

.align  4
.global simdCpy
.type   simdCpy,@function

simdCpy:
	entry sp, 32

	ee.vld.128.ip q0, a2, 16
	ee.vst.128.ip q0, a3, 16

	ee.vld.128.ip q0, a2, 16
	ee.vst.128.ip q0, a3, 16

	retw

// Input
// a0 .. Return address
// a1 .. Stack pointer (also sp)
// a2 .. pDst

// Output
// Memory change

.align  4
.global simdClear
.type   simdClear,@function

simdClear:
	entry sp, 32

	ee.zero.q q0
	ee.vst.128.ip q0, a2, 16
	ee.vst.128.ip q0, a2, 16

	retw

// Input
// a0 .. Return address
// a1 .. Stack pointer (also sp)
// a2 .. pSrc
// a3 .. pDst

// Output
// Memory change

.align  4
.global simdLdQr
.type   simdLdQr,@function

simdLdQr:
	entry sp, 32

	ld.qr q0, a2, 16
	ee.vst.128.ip q0, a3, 16

	retw

