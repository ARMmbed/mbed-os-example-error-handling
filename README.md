# Application to demonstrate Mbed-OS error handling and error coding APIs

This example demonstrates how to use the Mbed OS error handling API.

**Table of contents:**

1. [Requirements](#requirements)
1. [Setup](#setup)
   - [Clone the example](#import-the-example)
   - [Compile the example](#compile-the-example)
   - [Run the example](#run-the-example)

## Requirements

This example program can be run on any Mbed-Enabled device. For demostration purposes, the below documentation uses K64F as the test device. 
One of the functionality with error handling feature is to save the error log in a file and that requires a storage support on the device. 
The example program does have sample code to invoke that functionality, but you will need to pass a special flag as part of your command if 
you want to test that functionality.

Note that the example application mostly uses MBED_WARNING macros to demostrate the usage. Usage of MBED_ERROR macros are identical but the behavior is 
different in that calling MBED_ERROR macros halt the application after the recording the error. So, in this test application MBED_ERROR() call 
is the last call we make.

## Setup

#### Clone the example

Setup your Mbed development environment as described in - [Get started with Mbed OS](https://os.mbed.com/docs/latest/tutorials/your-first-program.html).

From the command-line, clone the example:

```
git clone https://github.com/ARMmbed/mbed-os-example-error-handling
cd mbed-os-example-error-handling
mbed deploy
```

#### Compile the example

Compile the application as below using ARM compiler. If you want to use GCC or IAR toolchains, use GCC_ARM or IAR for -t option.

```
mbed compile -m K64F -t ARM
```
If you want to test the "saving error history to file" functionality you can build the program as below provided you have a SD card on the device.
```
mbed compile -t arm -m k64f -DMBED_TEST_SIM_BLOCKDEVICE=SDBlockDevice
```
You can also test this functionality using a simulated block device as 
```
mbed compile -t arm -m k64f -DMBED_TEST_SIM_BLOCKDEVICE=HeapBlockDevice
```

This may take few minutes and once its successfully completed it would generate the output as below.

```
Link: mbed-os-example-error-handling
Elf2Bin: mbed-os-example-error-handling
+------------------+-------+-------+------+
| Module           | .text | .data | .bss |
+------------------+-------+-------+------+
| [lib]\c_w.l      | 12963 |    16 |  348 |
| [lib]\cpprt_w.l  |    42 |     0 |    0 |
| [lib]\fz_wm.l    |    18 |     0 |    0 |
| [lib]\m_wm.l     |    48 |     0 |    0 |
| anon$$obj.o      |    32 |     0 | 1024 |
| main.o           |  2057 |     0 |  268 |
| mbed-os\drivers  |   130 |     0 |    0 |
| mbed-os\features | 16840 |     0 |  304 |
| mbed-os\hal      |  1660 |    30 |   64 |
| mbed-os\platform |  4565 |   104 |  604 |
| mbed-os\rtos     | 14646 |  2310 | 4592 |
| mbed-os\targets  |  9249 |   104 |  324 |
| Subtotals        | 62250 |  2564 | 7528 |
+------------------+-------+-------+------+
Total Static RAM memory (data + bss): 10092 bytes
Total Flash memory (text + data): 64814 bytes

Image: .\BUILD\k64f\arm\mbed-os-example-error-handling.bin
```

#### Run the example

1. Connect a K64F device to your pc
1. Open a serial terminal connected to the device
1. Copy the binary file to the device.
1. Reset the device by pressing the reset button to start the program.

The output should look similar to below:

```
Error Handling and Error Coding Demo...
Error Status and Context capture successful
Error hook successful


++ MbedOS Error Info ++
Error Status: 0x800B0110 Code: 272 Module: 11
Error Message: I2C driver error
Location: 0x5D03
Error Value: 0xDEADDEAD
Current Thread: Id: 0x20001E90 Entry: 0x62B1 StackSize: 0x1000 StackMem: 0x20000E90 SP: 0x20001E18
For more info, visit: https://armmbed.github.io/mbedos-error/?error=0x800B0110
-- MbedOS Error Info --
```

If you have enabled testing for file saving feature, the output should look similar to below.
```
Error Handling and Error Coding Demo...
Error Status and Context capture successful
Error history capture successful
First Error: Status:0x80000101 ThreadId:0x20002068 Address:0xb233 Value:0x1234

Last Error: Status:0x80ff0107 ThreadId:0x20002068 Address:0xb233 Value:0x5

3: Status:0x80ff0107 ThreadId:0x20002068 Address:0xb233 Value:0x5

2: Status:0x80ff0108 ThreadId:0x20002068 Address:0xb233 Value:0x4

1: Status:0x80ff0109 ThreadId:0x20002068 Address:0xb233 Value:0x3

0: Status:0x80ff010c ThreadId:0x20002068 Address:0xb233 Value:0x2

Saving error history to filesystem successful
Error hook successful


++ MbedOS Error Info ++
Error Status: 0x80000110 Code: 272 Entity: 0
Error Message: System type error
Location: 0xaa79
Error Value: 0xdeaddead
Current Thread: Id: 0x20002068 Entry: 0xc97d StackSize: 0x1000 StackMem: 0x20001068 SP: 0x20001fd0
-- MbedOS Error Info --
```

## License and contributions

The software is provided under Apache-2.0 license. Contributions to this project are accepted under the same license. Please see [contributing.md](CONTRIBUTING.md) for more info.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.
