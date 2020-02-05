![](./resources/official_armmbed_example_badge.png)
# Error Handling Mbed OS example

This guide reviews the steps required to use error handling on an Mbed OS enabled platform.

You can find the complete information about error handling [here](https://os.mbed.com/docs/mbed-os/v5.15/apis/error-handling.html).

You can build this project with all supported [Mbed OS build tools](https://os.mbed.com/docs/mbed-os/latest/tools/index.html). However, this example project specifically refers to the command-line interface tool [Arm Mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).

## Application functionality

The `main()` function calls the following functions:
- **log_warning_error()**:  `MBED_MAKE_ERROR()` macro merges the log modules(application and platform) with error code before getting logged by `MBED_WARNING1()`. `mbed_get_first_error_info()` and `mbed_get_last_error_info()` used to retrieve the error context and `MBED_GET_ERROR_MODULE()` and `MBED_GET_ERROR_CODE()`  to decode the module and error code.
- **log_warning_error_hist()**: Logs the multiple warning errors using `MBED_WARNING1()` and retrieve error context one by one using `mbed_get_error_hist_info()` based on the `index` where the warning errors were logged.
- **save_error_history()**: Logs the multiple warning errors and using `mbed_save_error_hist()` to retrieves the error history, write into the file system.
- **error_hook()**: Registers the error hook call back using `mbed_set_error_hook()` which gets called when the warning errors logged.
- **error_halt()**: Logs with `MBED_ERROR1()` to halt the system.


### Note:

[HeapBlockDevice](https://os.mbed.com/docs/mbed-os/v5.14/apis/heapblockdevice.html) is used in this example to create 4KB simulated memory with [LittleFileSystem](https://os.mbed.com/docs/mbed-os/v5.15/apis/littlefilesystem.html) which is used by error handling `mbed_save_error_hist()` to write the logs into the file system.

## Preqrequisites

1. [Install Mbed CLI](https://os.mbed.com/docs/mbed-os/latest/tools/installation-and-setup.html).
1. Determine which toolchain supports your target.

   Depending on the target, you can build the example project with the GCC_ARM, ARM or IAR toolchain. To learn which toolchain supports your target, run this command:

   ```bash
   $ mbed compile -S
   ```

1. Clone this repository on your system.
1. Change the current directory to where the project was cloned.

## Building and running

1. Connect a USB cable between the USB port on the target and the host computer.
1. Run this command to build the example project and program the microcontroller flash memory:

    ```bash
    $ mbed compile -m <TARGET> -t <TOOLCHAIN> --flash --sterm
    ```

(Note: You can use the Mbed CLI command-line option "--sterm" to open a serial terminal after flashing.)

Your PC may take a few minutes to compile your code.

The binary is located at `./BUILD/<TARGET>/<TOOLCHAIN>/mbed-os-example-cpu-stats.bin`.

Alternatively, you can manually copy the binary to the target, which gets mounted on the host computer through USB.

## Expected output

The serial terminal shows an output similar to the following:
```
--- Terminal on /dev/ttyACM0 - 9600,8,N,1 ---
This is the error handling Mbed OS example
Error history capture successful

First Error: Status:0x80000101 ThreadId:0x20001178 Address:0x1bcd Value:0x1234

Last Error: Status:0x80ff0107 ThreadId:0x20001178 Address:0x1d7d Value:0x5

3: Status:0x80ff0107 ThreadId:0x20001178 Address:0x1d7d Value:0x5

2: Status:0x80ff0108 ThreadId:0x20001178 Address:0x1d6f Value:0x4

1: Status:0x80ff0109 ThreadId:0x20001178 Address:0x1d61 Value:0x3

0: Status:0x80ff010c ThreadId:0x20001178 Address:0x1d53 Value:0x2
Error hook successful


++ MbedOS Error Info ++
Error Status: 0x800B0110 Code: 272 Module: 11
Error Message: I2C driver error
Location: 0x1C95
Error Value: 0xDEADDEAD
Current Thread: main Id: 0x20001178 Entry: 0x737F StackSize: 0x1000 StackMem: 0x200022B8 SP: 0x2000324C 
For more info, visit: https://mbed.com/s/error?error=0x800B0110&tgt=K64F
-- MbedOS Error Info --
```
## Error history platform configuration

The error history feature can be enabled by `platform.error-hist-enabled": true` configuration in `mbed_app.json` as below:
```
    "target_overrides": {
        "*": {            
            "platform.error-hist-enabled": true            
        }
```

## Troubleshooting 
If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it. 

## Related links

* [Mbed OS configuration](https://os.mbed.com/docs/latest/reference/configuration.html).
* [Mbed OS serial communication](https://os.mbed.com/docs/latest/tutorials/serial-communication.html).
* [Mbed boards](https://os.mbed.com/platforms/).

### License and contributions

The software is provided under the Apache-2.0 license. Contributions to this project are accepted under the same license. Please see contributing.md for more information.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.

