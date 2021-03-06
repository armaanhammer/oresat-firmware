#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include "ch.h"
#include "hal.h"
#include "CANopen.h"

/* Preamble definitions
 * While Level 1 Time Codes are generally preferred by CCSDS, we are using
 * Level 2 Time Codes (meaning Agency Defined). In this case we are using
 * the UNIX Epoch of Jan 1, 1970.
 * Refer to CCSDS 301.0-B-4 for Time Code Formats.
 * TODO: Verify that the bit order is correct for preambles
 */
#define TIME_CUC_PREAMBLE   0x2E    /* CCSDS Unsegmented Time Code (CUC) Preamble */
#define TIME_CDS_PREAMBLE   0x49    /* CCSDS Day Segmented Time Code (CDS) Preamble */

/* CCSDS Unsegmented Time Code (CUC) data type */
typedef union {
    uint64_t raw;
    struct {
        uint32_t coarse:32;
        uint32_t fine:24;
    };
} time_scet_t;

/* CCSDS Day Segmented Time Code (CDS) data type */
typedef union {
    uint64_t raw;
    struct {
        uint16_t day:16;
        uint32_t ms:32;
        uint16_t us:16;
    };
} time_utc_t;

time_t get_time_unix(uint32_t *msec);
void set_time_unix(time_t unix_time, uint32_t msec);
void get_time_scet(time_scet_t *scet);
void set_time_scet(const time_scet_t *scet);
void get_time_utc(time_utc_t *utc);
void set_time_utc(const time_utc_t *utc);

CO_SDO_abortCode_t OD_SCET_Func(CO_ODF_arg_t *ODF_arg);
CO_SDO_abortCode_t OD_UTC_Func(CO_ODF_arg_t *ODF_arg);

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*TIME_SYNC_H*/
