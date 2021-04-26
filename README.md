![](./resources/official_armmbed_example_badge.png)
# Error handling Mbed OS example

This example shows how Mbed OS error handling works on an Mbed OS enabled platform.

You can find more information about the error handling APIs in the [documentation](https://os.mbed.com/docs/mbed-os/latest/apis/error-handling.html).

You can build the project with all supported [Mbed OS build tools](https://os.mbed.com/docs/mbed-os/latest/tools/index.html). However, this example project specifically refers to the command-line interface tools, [Arm Mbed CLI 1](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli) and [Mbed CLI 2](https://github.com/ARMmbed/mbed-tools#installation).

## Mbed OS build tools

### Mbed CLI 2
Starting with version 6.5, Mbed OS uses Mbed CLI 2. It uses Ninja as a build system, and CMake to generate the build environment and manage the build process in a compiler-independent manner. If you are working with Mbed OS version prior to 6.5 then check the section [Mbed CLI 1](#mbed-cli-1).
1. [Install Mbed CLI 2](https://os.mbed.com/docs/mbed-os/latest/build-tools/install-or-upgrade.html).
1. From the command-line, import the example: `mbed-tools import mbed-os-example-error-handling`
1. Change the current directory to where the project was imported.

### Mbed CLI 1
1. [Install Mbed CLI 1](https://os.mbed.com/docs/mbed-os/latest/quick-start/offline-with-mbed-cli.html).
1. From the command-line, import the example: `mbed import mbed-os-example-error-handling`
1. Change the current directory to where the project was imported.

## Application functionality

This example demonstrates the Mbed OS error handling feature and the APIs associated with it. One specific functionality is the error history, which saves to a file the errors that previously occurred in the system. This functionality requires storage support on the device. The example program creates a [little file system](https://os.mbed.com/docs/mbed-os/latest/apis/littlefilesystem.html) instance on a simulated [heap block device](https://os.mbed.com/docs/mbed-os/latest/apis/heapblockdevice.html).

Note the example application mostly uses MBED_WARNING macros to generate errors to demonstrate the error handling APIs. MBED_ERROR macros are identical, but the behavior is different in that MBED_ERROR macros will halt the application after the error has been recorded. In this example application, `MBED_ERROR()` is called at the end.

The example shows how to use the following APIs:

- `mbed_get_first_error_info()` and `mbed_get_last_error_info()` to retrieve the first and last error context information captured.
- `mbed_get_error_hist_info()` to retrieve the error context information for a specific error from error history.
- `mbed_save_error_hist()` to save the error history information to a file.
- `mbed_set_error_hook()` to register an application defined error callback with the error handling system. The system calls this function with error context information whenever it handles `mbed_error` or `mbed_warning`.

## Building and running

1. Connect a USB cable between the USB port on the target and the host computer.
1. Run this command to build the example project and program the microcontroller flash memory:

    * Mbed CLI 2

    ```bash
    $ mbed-tools compile -m <TARGET> -t <TOOLCHAIN> --flash --sterm
    ```

    * Mbed CLI 1

    ```bash
    $ mbed compile -m <TARGET> -t <TOOLCHAIN> --flash --sterm
    ```


Your PC may take a few minutes to compile your code.

The binary is located at:
* **Mbed CLI 2** - `./cmake_build/<TARGET>/develop/<TOOLCHAIN>/mbed-os-example-error-handling.bin`

* **Mbed CLI 1** - `./BUILD/<TARGET>/<TOOLCHAIN>/mbed-os-example-error-handling.bin`

You can manually copy the binary to the target, which gets mounted on the host computer through USB, rather than using the `--flash` option.

You can also open a serial terminal separately, rather than using the `--sterm` option, with the following command:
* Mbed CLI 2 
	```bash
	$ mbed-tools sterm
	```

* Mbed CLI 1
	```bash
	$ mbed sterm
    ```


## Expected output

The serial terminal shows an output similar to:

```
--- Terminal on /dev/tty.usbmodem31102 - 9600,8,N,1 ---
This is the error handling Mbed OS example

Reporting the following warnings:
   MBED_ERROR_CODE_INVALID_ARGUMENT Status: 0x80000101 Value: 0x1234
   MBED_ERROR_CODE_INVALID_ARGUMENT Status: 0x80000101 Value: 0x4567
   MBED_ERROR_CODE_ALREADY_IN_USE   Status: 0x8001010c Value: 0xABCD
Retrieving first error info logged...  successful
Retrieving last error info logged...   successful

Reporting the following warnings:
   MBED_ERROR_TIME_OUT         Status: 0x80ff010d Value: 0x64
   MBED_ERROR_FAILED_OPERATION Status: 0x80ff010f Value: 0x65
   MBED_ERROR_UNSUPPORTED      Status: 0x80ff0109 Value: 0x66
   MBED_ERROR_ACCESS_DENIED    Status: 0x80ff0108 Value: 0x67
Calling mbed_get_error_hist_info() to retrieve the error history:
   Status: 0x80ff010d Value: 0x64
   Status: 0x80ff010f Value: 0x65
   Status: 0x80ff0109 Value: 0x66
   Status: 0x80ff0108 Value: 0x67
Retrieving error history... successful

Reporting the following warnings:
   MBED_ERROR_TIME_OUT       Status: 0x80ff010d Value: 0x1
   MBED_ERROR_ALREADY_IN_USE Status: 0x80ff010c Value: 0x2
   MBED_ERROR_UNSUPPORTED    Status: 0x80ff0109 Value: 0x3
   MBED_ERROR_ACCESS_DENIED  Status: 0x80ff0108 Value: 0x4
   MBED_ERROR_ITEM_NOT_FOUND Status: 0x80ff0107 Value: 0x5
Calling mbed_save_error_hist() to save the error history to a file
Retrieving error history by reading from file:

First Error: Status:0x80000101 ThreadId:0x20001160 Address:0x1be5 Value:0x1234

Last Error: Status:0x80ff0107 ThreadId:0x20001160 Address:0x1eab Value:0x5

3: Status:0x80ff0107 ThreadId:0x20001160 Address:0x1eab Value:0x5

2: Status:0x80ff0108 ThreadId:0x20001160 Address:0x1e93 Value:0x4

1: Status:0x80ff0109 ThreadId:0x20001160 Address:0x1e7b Value:0x3

0: Status:0x80ff010c ThreadId:0x20001160 Address:0x1e63 Value:0x2
Retrieving error history by reading from file... successful

Registering my_error_hook... successful
Reporting an error. Note that this will cause the system to halt


++ MbedOS Error Info ++
Error Status: 0x800B0110 Code: 272 Module: 11
Error Message: I2C driver error
Location: 0x1D91
Error Value: 0xDEADDEAD
Current Thread: main Id: 0x20001160 Entry: 0x768F StackSize: 0x1000 StackMem: 0x200022B8 SP: 0x2000324C
For more info, visit: https://mbed.com/s/error?error=0x800B0110&tgt=K64F
-- MbedOS Error Info --

= System will be rebooted due to a fatal error =
= Reboot count(=1) reached maximum, system will halt after rebooting =
```

## Configuring the application

You can enable the error history feature by setting `platform.error-hist-enabled` to true in the application configuration file:

```
    "target_overrides": {
        "*": {            
            "platform.error-hist-enabled": true            
        }
    }
```

## Troubleshooting 

If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it. 

## Related links

- [Mbed OS crash reporting example](https://github.com/ARMmbed/mbed-os-example-crash-reporting).
- [Mbed OS block device example](https://github.com/ARMmbed/mbed-os-example-blockdevice).
- [Mbed OS file system example](https://github.com/ARMmbed/mbed-os-example-filesystem).
- [Mbed OS configuration](https://os.mbed.com/docs/latest/reference/configuration.html).
- [Mbed OS serial communication](https://os.mbed.com/docs/latest/tutorials/serial-communication.html).
- [Mbed boards](https://os.mbed.com/platforms/).

### License and contributions

The software is provided under the Apache-2.0 license. Contributions to this project are accepted under the same license. Please see contributing.md for more information.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.

