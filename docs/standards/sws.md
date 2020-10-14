# SWS: Software Writing Standards

> :warning: This document is under development

Task Ref: `UT_2.0.1`

## Changelog

- __`2019-12-10 - Richard Abrams GDP201920`__
    
    Creation

- __`2020-03-11 - Richard Abrams GDP201920`__

    - Struct/header standard
    - Removed “usage” section prototype, this is now in the Software Manual 
      document
- __`2020-10-02 - Duncan Hamill GDP202021`__

    - Significant update and expansion of SWS

## Preface

The following points apply in general to the entire software repository,
including this document.

- Terms such as “__must__”, “_should_” and respective negatives are to be interpreted
  as outlined in [RFC2119](https://tools.ietf.org/html/rfc2119).
- Semantic version numbering is to be used for files, with version number 
  included in a comment at the top of the file. See [semver](semver.org) for 
  instruction.
- __TODO__ The OBC firmware is stored on GitHub under the 
  [UoS3 Project](https://github.com/uos3/obc-firmware).

## Principles

This document provides a series of standards for writing code in the 
`uos3-obc-fw` repository. The standard follows 3 main principles:

1. __Safety is critical to the mission__
    
    Mission safety cannot be comprised in any way, so operations that may
    result in errors __must__ be guarded against and justified. Therefore a
    standardised result system is used, which is detailed in [the error
    handling section](#error-handling).

2. __Readable code is better than concise code__

    "Clever" code and one-liners may seem like a good idea but they just end up
    causing confusion and wasting time when someone else comes around to
    review/fix your code. For this reason things like

    ```c
    float angular_rate_rads = fabs(sample_time_s) < FLT_EPSILON
        ? p_status_report.f_division_by_zero = true, 0.0
        : angular_change_rad / sample_time_s;
    ```

    should be expanded to something like

    ```c
    float angular_rate_rads;

    if (fabs(sample_time_s) < FLT_EPSILON) {
        p_status_report.f_division_by_zero = true;
        angular_rate_rads = 0.0;
    }
    else {
        angular_rate_rads = angular_change_rad / sample_time_s;
    }
    ``` 

3. __Modularity is preferred__

    Modular code (i.e. code which is clearly separated out into separate
    modules based on use/purpose) provides greater organisation and
    compartmentalisation than monolithic code. The directory structure
    encourages separation of modules, and the build system (CMake) allows
    static linking to different libraries based on different executables needs.

    This is covered in the sections on the [build system](#build-system),
    [directory structure](#directory-structure), and [module
    structure](#module-structure).

These principles should be followed at all times. If you encounter a situation
not covered by this standard you should:

1. Think of these principles in how you solve the situation
2. Consult with others to check its a reasonable solution
3. Add it to the standard

With that said, lets dive in.

## Language

The language used for the OBC firmware is __`C99`__, and __must__ be compiled with
GCC using the ARM embedded tool chain provided 
[here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).

## Version Control

Git is being used as the version control tool for the software repository. Any
source other than that in the main 
[__TODO__ GitHub repository](https://github.com/uos3/obc-firmware) __shall 
not__ be considered a more complete/up-to-date version of the source code.

You __must__ ensure that any changes are committed and pushed into the remote
repository. Shared devices (such as the Raspberry Pi) __must not__ be used to
store development branches.

You _should not_ make code changes on any shared devices. Instead you _should_
make changes on a private branch, and then transfer the code to the shared 
device. This avoids any untracked changes being run on the shared device.

## Build System

The build system used for the OBC firmware is __`CMake`__. A minimum version of
3.16.3 is required, as this is the version that Ubuntu 20.04 ships with.

In the past a combination of Makefiles, bash scripts, and python scripts were
used to build and flash the software. This has been replaced by CMake and a few
small bash scripts.

## Module Structure

TODO

## Error Handling

TODO