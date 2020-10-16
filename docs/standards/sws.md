# SWS: Software Writing Standards

> :warning: This document is under development

Task Ref: `UT_2.0.1`

## Changelog

- __`2019-12-10 - Richard Abrams GDP201920`__
    
    - Creation

- __`2020-03-11 - Richard Abrams GDP201920`__

    - Struct/header standard
    - Removed “usage” section prototype, this is now in the Software Manual 
      document
- __`2020-10-02 - Duncan Hamill GDP202021`__

    - Significant update and expansion of SWS

## Preface

The following points apply in general to the entire software repository,
including this document.

- Terms such as “__must__”, “_should_” and respective negatives are to be 
  interpreted as outlined in [RFC2119](https://tools.ietf.org/html/rfc2119).
- Semantic version numbering is to be used for files, with version number 
  included in a comment at the top of the file. See [semver](semver.org) for 
  instruction.
- The OBC firmware is stored on GitHub under the 
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
        ? p_status_report_inout.f_division_by_zero = true, 0.0
        : angular_change_rad / sample_time_s;
    ```

    should be expanded to something like

    ```c
    float angular_rate_rads;

    if (fabs(sample_time_s) < FLT_EPSILON) {
        p_status_report_inout.f_division_by_zero = true;
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

The language used for the OBC firmware is __`C99`__, and __must__ be compiled 
with GCC using the ARM embedded tool chain provided 
[here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).

## Version Control

Git is being used as the version control tool for the software repository. Any
source other than that in the main 
[GitHub repository](https://github.com/uos3/obc-firmware) __shall not__ be 
considered a more complete/up-to-date version of the source code.

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

The code base and functionality of the software is divided into modules. One
definition of a module, and the one that is used here, is given by:

> A module
>  1. encapsulates code and data to implement a particular functionality,
>  2. has an interface that lets clients access its functionality in a 
>    uniform manner,
>  3. is easily pluggable with another module that expects its interface,
>  4. is usually packaged in a single unit so that it can be easily deployed,

In the case of this software these points can be explained to mean:

1. A module deals with a single functionality of the vehicle, such as the GNSS
   module.
2. Has public functions available for use by other modules,
3. Creates a level of abstraction over its internal functioning so that the
   user does not have to think of these problems.
4. Is compiled as a static library and linked into other modules through the
   build system.



A module should be contained within a single folder which is named after the
module itself. The module folder _should_ contain at least:

- `CMakeLists.txt` - CMake listing for the module which should create a new 
  library using the module name.
- `<ModuleName>_public.h` - declarations of functions and defines available for 
  use by other modules.
- `<ModuleName>_public.c` - implementations of public functions and static 
  definitions.

In addition, the module may contain:

- `<ModuleName>_private.h` - declarations of private functions and defines for
  use within the module itself.
- `<ModuleName>_private.c` - implementations of private functions and static 
  definitions.

By convention an external module _MUST NOT_ import or use a module's private 
items. 

Any long functions (>100 lines) should be kept in their own file following the 
`<ModuleName>_<function_name>.c` convention.

## Naming Conventions

The following naming conventions apply for the software:

 - Modules __shall__ follow the `UpperCamelCase` naming scheme. 
    - Abbreviations _should_ have their first letter capitalised and all 
      subsequent letters are lower case, such as `Gnss` or `Imu`.
    - Module names _should not_ be shortened, for instance `Buffer` is
      preferred over `Buf`.

 - Functions __shall__ follow the `<ModuleName>_<function_name>` naming scheme,
   that is the module name in `UpperCamelCase`, followed by the function name
   in `lower_snake_case`. For example `Gnss_get_data` or `Imu_init`.
     - Initialisation functions __shall__ be called `init`.
     - Functions names _should_ start with a verb, such as `get`, `calc`, or
       `do`. Verbs _should_ be shortened to less than 4 characters when
       possible.

 - Variables and function arguments __must__ follow the `lower_snake_case`
   naming scheme.
     - Pointers __must__:
        - be prefixed with `p_`. Double pointers __must__ be prefixed with 
          `pp_`. Higher orders of pointers _should not_ be used.
        - have the declaring asterisk in contact with the variable name, not
          the type: `int *p_number`, __NOT__ `int* p_number`.
     - Function arguments __must__ be suffixed depending on their use:
        - `_in` for inputs
        - `_out` for outputs
        - `_inout` for both inputs and outputs
     - If a quantity has an associated unit it __must__ be suffixed with that
       unit:
        - Units __shall__ form a single 'word' in `lower_snake_case`, for
          instance use `ms` for meters/second not `m_s`. 
        - Units with powers __shall__ omit the power: `mss` for
          meters/second^2.
        - The definition comment of the variable/argument shall specify in 
          words the units, for example:
          ```c
          /**
           * @brief The velocity
           *
           * @units meters/second
           */
          float velocity_ms = 0.0;
          ```
     - For example:
        - `p_output_data_out` for a pointer to some output data which will be
          written to by the function.
        - `elapsed_time_us` for an elapsed time in microseconds.

 - Constants and preprocessor definitions __must__ follow the
   `UPPER_SNAKE_CASE` naming scheme.
    - Constants and defines __must__ be prefixed with the module name, such as
      `GNSS_NMEA_LENGTH_BYTES`.
    - Constants and defines __shall__ follow the same rules as variables and 
      function arguments with regards to units, however they shall be in
      `UPPER_SNAKE_CASE`.
    - Defines _should_ be used over constants. This is because defines do not
      add to the symbol count and memory usage of the software.
    - Define values __must__ be wrapped in brackets to avoid evaluation issues,
      for instance `#define MY_DEF (12)` over `#define MY_DEF 12`.

## Commenting

### General Comments

Comments _should_ explain code, not describe it. For example:

```c
/* Calculate the angular rate so we can determine when the s/c has despun */
float angular_rate_rads = angular_change_rad / sample_time_s;
```

should be preferred over

```c
/* get the angular rate */
float angular_rate_rads = angular_change_rad / sample_time_s;
```
Long comments are not to be discouraged. If you need a paragraph to explain why 
you're doing something it's likely a very complicated and should be explained 
in depth so that:

1. You can check you're doing the right thing
2. Other people can verify that the code is doing what it's supposed to as a 
   part of a review
3. Future maintainers can understand the purpose of the code.

### Block Title Comments

Two levels of block title comments are used:

```c
/* -------------------------------------------------------------------------
 * SECTION TITLE
 * ------------------------------------------------------------------------- */

/* ---- SUBSECTION TITLE ---- */
```

Section titles _should_ be reserved for use outside of a function body, i.e.
for separating the includes and defines sections of a source file. If you find
yourself needing more than two levels of section titles it is likely you need
to refactor into functions or rethink the design of the function itself.

### Documentation Comments

The doxygen javadoc comment style __shall__ be used for all documentation
comments. If using the [Doxygen Documentation Generator](https://marketplace.visualstudio.com/items?itemName=cschlosser.doxdocgen)
VSCode extension these comments are automatically inserted when writing `/**`
and pressing enter. This is the recommended way of creating documentation 
comments. If in doubt follow the doxygen guidelines.

Files shall start with a doxygen file header comment, for example:

```c
/**
 * @ingroup demo
 * 
 * @file demo_launchpad_blinky.c
 * @author Duncan Hamill (dh2g16@soton.ac.uk/duncanrhamill@googlemail.com)
 * 
 * @brief TM4C Launchpad Blinky Demo
 * 
 * This demo file is used to verify the build system functionality with the
 * TM4C launchpad. It will toggle the onboard LED if SW1 is pressed, and keep
 * the LED on if SW2 is pressed. If buttons are released the LED is turned off.
 * 
 * Originally written by Yusef Karim, adapted for UoS3.
 * 
 * @version 0.1
 * @date 2020-10-02
 * @copyright Copyright (c) UoS3 2020
 */
```

In files which define a module (for instance the module `_public.h` header) a
doxygen group __shall__ be defined for that module, for instance:

```c
/* (At end of header comment:)
 * @defgroup demo_launchpad_blinky Demo Launchpad Blinky
 * @{
 */
```

The group __must__ also be closed at the end of the file, with a comment
including the group name:

```c
/** @} */ /* End of demo_launchpad_blinky */
```

All other files in that module shall include the `@ingroup` directive in their
header comment, including the opening/closing markers as described in
[doxygen/Grouping](https://www.doxygen.nl/manual/grouping.html).

### Numerical Protection Comments

When numerical protection is needed 
([see Numerical Protection](#numerical-protection)) a comment explaining the 
need for the protection and the solution should be included, for example:

```c
float angular_rate_rads;

/* ---- NUMERICAL PROTECTION ----
 *
 * To prevent a division by zero here sample time is compared to the floating 
 * point epsilon value. If the sample time is close to zero the angular rate is
 * patched to zero in order to avoid large spikes in the angular rate signal.
 *
 * In addition the division by zero flag is raised in the status report.
 */
if (fabs(sample_time_s) < FLT_EPSILON) {
    p_status_report_inout.f_division_by_zero = true;
    angular_rate_rads = 0.0;
}
else {
    angular_rate_rads = angular_change_rad / sample_time_s;
}
```


## File Format

### File Structure

C source code and header files _should_ use the following format:

```c
/**
 *  File header comment 
 */

/* -------------------------------------------------------------------------
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdio.h>

/* TivaWare includes */
#include "inc/tm4c123gh6pm.h"

/* Internal includes */
#include "driver/spi/spi_public.h"

/* -------------------------------------------------------------------------
 * DEFINES
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * GLOBALS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * ENUMS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * STRUCTS
 * ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * FUNCTIONS
 * ------------------------------------------------------------------------- */
```

### Line Ending

Files __shall__ follow the LF (line feed) end of line sequence, for 
compatibility with Linux. Windows editors should be set to use the LF line 
ending sequence.


## Error Handling

TODO
