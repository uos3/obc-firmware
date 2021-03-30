/**
 * @file Rtc_public.h
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * @brief Real Time Clock module of the software.
 * 
 * Task ref: [UT_2.8.4]
 * 
 * The Rtc driver is responsible for managing the RTC (real time clock) 
 * component of the TM4C, which is managed by the TivaWare Hibernate module. 
 * 
 * TivaWare Hibernate includes more than just the RTC, namely the ability to
 * put the processor into Hibernate mode. This is essentially an ultra-low
 * power mode for the TM4C, in which power is removed from the processor with
 * only the Hibernation module itself remaining powered. This is actually not
 * desired behaviour of the system as this would include removing power for the
 * RAM, requiring a reboot and re-initialisation of the software. Sleep mode is
 * therefore obtained using the `Kernel_sleep` function.
 * 
 * The RTC from the Hibernate module is used to keep wall time of the
 * spacecraft, that is time since last OBC reset. It is not capable of
 * providing an absolute time signal such as unix time. This hardware
 * limitation requires us to get absolute time fixes from the GNSS instead,
 * however this process is managed by ground. The Rtc module is responsible for
 * providing millisecond accurate wall time values, which can be correlated by
 * ground with GNSS fixes in order to produce absolute timings.
 * 
 * @version 0.1
 * @date 2021-03-24
 * 
 * @copyright Copyright (c) 2021
 */

#ifndef H_RTC_PUBLIC_H
#define H_RTC_PUBLIC_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* External includes */
#include <stdint.h>
#include <stdbool.h>

/* Internal includes */
#include "system/kernel/Kernel_public.h"
#include "drivers/rtc/Rtc_errors.h"

/* -------------------------------------------------------------------------   
 * DEFINES
 * ------------------------------------------------------------------------- */

/**
 * @brief Number of bytes used to represent a timestamp when serialized.
 * 
 * 6 bytes are chosen as detailed in the `Rtc_Timestamp` doc comment.
 */
#define RTC_TIMESTAMP_NUM_BYTES (6)

/* -------------------------------------------------------------------------   
 * TYPES
 * ------------------------------------------------------------------------- */

/**
 * @brief Represents a point in time relative to the last reset of the RTC, to
 * a precision of 1/32768ths of a second.
 * 
 * The TM4C's RTC has 15-bit subsecond precision, therefore defining the
 * precision of the Rtc_Timestamp value. In total 48 bits are allocated for
 * timekeeping. A 64 bit unsigned int is therefore required to store this.
 * 
 * Using only 32 bits would limit the maximum duration of an Rtc_Time value to
 * ~1.5 days. Using 64 bits puts this up to ~17 million years. As a compramise
 * with the aim of minimising the number of bytes required in telemetry for
 * timestamps 48 bits is chosen, which gives a maximum timespan of ~272 years,
 * far in excess of the expected lifetime of the mission, while still being an
 * even integer number of bytes, something which is desired for communications.
 * 
 * Use `Rtc_timestamp_to_bytes` to get the representation of the timestamp as a
 * string of bytes.
 */
typedef uint64_t Rtc_Timestamp;

/**
 * @brief Represents a duration between two Rtc_Timestamps, to a precision of 
 * 1/32768ths of a second.
 * 
 * See `Rtc_Timestamp` for more information on the size of this value.
 */
typedef uint64_t Rtc_Timespan;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Initialise the Rtc module.
 * 
 * This will reset the RTC to 0.
 * 
 * @return ErrorCode ERROR_NONE on success, other values indicate an error.
 */
ErrorCode Rtc_init(void);

/**
 * @brief Get the current timestamp from the RTC.
 * 
 * @return Rtc_Timestamp The current timestamp.
 */
Rtc_Timestamp Rtc_get_timestamp(void);

/**
 * @brief Set the current RTC seconds value.
 * 
 * Setting the subseconds counter is not supported.
 * 
 * _WARNING_: Setting the RTC time may have unexpected consequences, only use
 * in special circumstances.
 * 
 * @param seconds_in The value to set the seconds counter to.
 */
void Rtc_set_seconds(uint32_t seconds_in);

/**
 * @brief Convert the given Rtc_Timestamp struct into a number of milliseconds 
 * since RTC init.
 * 
 * Note: Converting back to a timestamp using `Rtc_timestamp_from_ms` is not
 * guarenteed to produce the same value due to rounding/flooring errors.
 * 
 * @param p_time_in Pointer to the `Rtc_Timestamp` struct to convert.
 * @return uint64_t Number of whole milliseconds since RTC init.
 */
uint64_t Rtc_timestamp_to_ms(
    Rtc_Timestamp *p_timestamp_in
);

/**
 * @brief Convert the given number of milliseconds since RTC init to an
 * `Rtc_Timestamp` struct.
 * 
 * Note: Converting back to a ms value using `Rtc_timestamp_to_ms` is not
 * guarenteed to produce the same value due to rounding/flooring errors.
 * 
 * @param milliseconds_in The number of milliseconds since RTC init.
 * @return Rtc_Timestamp The calculated Rtc_Timestamp.
 */
Rtc_Timestamp Rtc_timestamp_from_ms(
    uint64_t milliseconds_in
);

/**
 * @brief Convert the given timestamp into a sequence of bytes.
 * 
 * @param p_timestamp_in The timestamp to convert
 * @param p_bytes_out Pointer to the bytes array to populate. The array must
 * contain at least RTC_TIMESTAMP_NUM_BYTES bytes.
 */
void Rtc_timestamp_to_bytes(
    Rtc_Timestamp *p_timestamp_in,
    uint8_t *p_bytes_out
);

/**
 * @brief Convert a sequence of bytes into a timestamp.
 * 
 * @param p_bytes_in Pointer to the bytes to convert. The array must contain
 * RTC_TIMESTAMP_NUM_BYTES.
 * @return Rtc_Timestamp The converted timestamp.
 */
Rtc_Timestamp Rtc_timestamp_from_bytes(
    uint8_t *p_bytes_in
);

/**
 * @brief Convert the given timespan to milliseconds
 * 
 * Note: Converting back to a timespan using `Rtc_timespan_from_ms` is not
 * guarenteed to produce the same value due to rounding/flooring errors.
 * 
 * @param p_timespan_in Pointer to the timespan to convert
 * @return uint64_t The duration of the timespan in whole milliseconds
 */
uint64_t Rtc_timespan_to_ms(
    Rtc_Timespan *p_timespan_in
);

/**
 * @brief Convert a duration in milliseconds to a timespan.
 * 
 * Note: Converting back to a ms value using `Rtc_timespan_to_ms` is not
 * guarenteed to produce the same value due to rounding/flooring errors.
 * 
 * @param milliseconds_in Number of milliseconds in the duration.
 * @return Rtc_Timespan The converted timespan
 */
Rtc_Timespan Rtc_timespan_from_ms(
    uint64_t milliseconds_in
);

/**
 * @brief Determines if the given duration from the start time has ellapsed.
 * 
 * @param p_start_in Pointer to the start timestamp
 * @param p_duration_in Pointer to the timespan representing the duration
 * @param p_is_ellapsed_out 
 * @return ErrorCode 
 */
ErrorCode Rtc_is_timespan_ellapsed(
    Rtc_Timestamp *p_start_in, 
    Rtc_Timespan *p_duration_in, 
    bool *p_is_ellapsed_out
);

#endif /* H_RTC_PUBLIC_H */