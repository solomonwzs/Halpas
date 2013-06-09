#ifndef _BASE_H
#define _BASE_H

#include <stdint.h>
#include <stdlib.h>

#define ENTRY_TYPE_UNKNOWN 0x00
#define ENTRY_TYPE_POINT 0x01
#define ENTRY_TYPE_UINT 0x02
#define ENTRY_TYPE_INT 0x03
#define ENTRY_TYPE_FLOAT 0x04
#define ENTRY_TYPE_NULL 0x05
#define ENTRY_TYPE_MIN 0x06
#define ENTRY_TYPE_MAX 0x07

#define setEntryFunc(_f_, _h_, _kc_, _kf_, _vf_, _kd_, _vd_) do{\
    (_f_).hashfunc=(_h_); \
    (_f_).keycmpfunc=(_kc_); \
    (_f_).keyfreefunc=(_kf_); \
    (_f_).valfreefunc=(_vf_); \
    (_f_).keydupfunc=(_kd_); \
    (_f_).valdupfunc=(_vd_); \
} while(0)

#define printEntryValue(_ev_) (\
    ((_ev_).type==ENTRY_TYPE_UINT || (_ev_).type==ENTRY_TYPE_INT)?\
        printf(".%li.", (_ev_).val.ui):(\
            (_ev_).type==ENTRY_TYPE_FLOAT?\
                printf(".%f.", (_ev_).val.f):(\
                    (_ev_).type==ENTRY_TYPE_POINT?\
                        printf(".%s.", (_ev_).val.point):\
                        printf("unknown"))))

typedef struct entryValue{
    int type;
    union{
        void *point;
        uint64_t ui;
        int64_t si;
        double f;
    } val;
} entryValue;

typedef struct entryFunc{
    unsigned long (*hashfunc)(void *privdata, const void *key);
    int (*keycmpfunc)(void *privdata, const void *key1, const void *key2);
    void (*keyfreefunc)(void *privdata, void *key);
    void (*valfreefunc)(void *privdata, void *val);
    void *(*keydupfunc)(void *privdata, void *key);
    void *(*valdupfunc)(void *privdata, void *val);
} entryFunc;

#endif
