
    .text
    .align  4

    .global dummyCalc
    .type   dummyCalc,@function

// Input
// a0 .. Return address
// a1 .. Stack pointer (also sp)
// a2 .. pBase
// a3 .. offset

// Output
// a2 .. pBase + offset

dummyCalc:
	entry sp, 32

	add a2, a2, a3

	retw

    .text
    .align  4

    .global simdCalc
    .type   simdCalc,@function

simdCalc:
	entry sp, 32

	ee.zero.q q0

	retw

