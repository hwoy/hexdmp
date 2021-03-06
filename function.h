#include <stdio.h>

unsigned int
ui2s(unsigned int num,
    char* buff,
    unsigned int bsize,
    unsigned int base,
    unsigned int len);
int isUint(const char*);
unsigned int
pow2ui(unsigned int, unsigned int);
unsigned int
s2ui(const char*, unsigned int);

unsigned long
uL2s(unsigned long num,
    char* buff,
    unsigned int bsize,
    unsigned int base,
    unsigned int len);

unsigned long
pow2uL(unsigned int, unsigned int);
unsigned long
s2uL(const char*, unsigned int);

unsigned int
sLen(const char*);

unsigned
isUintHex(const char*);

fpos_t
pow2fpos(unsigned int base, unsigned int pow);

fpos_t
fpos2s(fpos_t num,
    char* buff,
    unsigned int bsize,
    unsigned int base,
    unsigned int len);

fpos_t
s2fpos(const char*, unsigned int);

size_t
pow2sT(unsigned int base, unsigned int pow);

size_t
sT2s(size_t num,
    char* buff,
    unsigned int bsize,
    unsigned int base,
    unsigned int len);

size_t
s2sT(const char*, unsigned int);
