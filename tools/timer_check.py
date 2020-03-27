#!/usr/bin python3.7

"""
Python script to perform the RTC timekeeping check. Python slow, but C time
library fast.

Instructions for use:
1) build and flash rtc_timekeeping_check
2) run the script. This script will start the test.
3) Check the output of this script for results.
"""

import time

WAIT_PERIOD = 10
NUM_TESTS = 2
start_message = "start"
debug_filepath = "/dev/ttyACM1"


def check_tobc():
    try:
        with open(debug_filepath, 'w') as uart:
            if uart.writable():
                return True
    except Exception as e:
        print(f"could not open file pointer: {e}")
    return False


def send(msg):
    with open(debug_filepath, 'w') as uart:
        uart.write(msg)
        uart.write("\r\n")


def read(blocking=True):
    msg = None
    if blocking:
        with open(debug_filepath, 'r') as uart:
            msg = uart.readline()
            msg = msg[:-1]
    else:
        with open(debug_filepath, 'rb', buffering=0) as uart:
            if uart.readable():
                msg = uart.read()
            if msg is not None:
                msg = msg.decode('utf-8')
        
    # print("message: ", msg)
    return msg


def flush():
    with open(debug_filepath, 'rb', buffering=0) as uart:
        # uart.flush()
        if uart.readable():
            uart.read()



def blocking_read(expected_length=1, return_all=True):
    retrieved_str = ""
    while (len(retrieved_str) < expected_length):
        if return_all:
            retrieved_str += read()
        else:
            retrieved_str = read()
    return retrieved_str


def get_acknowledgement(expected):
    ack = read()
    if len(ack.split("'")) > 1:
        if ack.split("'")[1] == expected:
            print("got expected acknowledgement:", ack)
            return True
    print("got unexpected acknowledgement:", ack)
    return False


def run_test():
    # print("flushing buffer...")
    # flush()
    print("running test...")
    send(start_message)
    start_time_s = time.time()
    # print("start signal sent, awaiting acknowledgement")
    # get_acknowledgement(start_message)
    print("start sent, awaiting response")
    # 30s wait should occur here...
    # end_time_s = time.time()
    # msg = None
    # while (msg is None):
    #     # wait for the end times
    #     # msg = read(False)
    #     # print(".", end="")
    #     if ((end_time_s - start_time_s) > (WAIT_PERIOD * 1.1)):
    #         print("waiting took too long")
    #         break
    #     # if msg is not None:
    #     #     print("got message")
    #     #     break
    # else:
    #     pass
    rtc_diff_time = read()
    end_time_s = time.time()

    # 38 should be length of output string, -1 as we've just read 1
    # rtc_start_time = read()
    # rtc_end_time = read()

    # print("rtc start:", rtc_start_time)
    # print("lcl start:", start_time_s*1000)

    # print("rtc end:  ", rtc_end_time)
    # print("lcl end:  ", end_time_s * 1000)
    if ":" in rtc_diff_time:
        rtc_diff_time = rtc_diff_time.split(":")[-1]
        if "  " in rtc_diff_time:
            rtc_diff_time = rtc_diff_time.split(" ")[-1]

    print("rtc diff: ", rtc_diff_time)
    print("lcl diff: ", (end_time_s-start_time_s))

    print("done")


if __name__ == "__main__":
    print("=== TOBC RTC timer check ===")
    print("checking TOBC availability: ", end="")
    if check_tobc():
        print("tobc functioning")
        # clear message header
        for i in range(NUM_TESTS):
            time.sleep(2)
            run_test()
    else:
        print("tobc not detected. exiting")
