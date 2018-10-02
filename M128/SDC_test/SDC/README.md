# ASA SDC00_Lib

A Driver library for ASA SDC00 Module

### Prerequisites

* GCC-AVR toolchain

```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install gcc-avr binutils-avr avr-libc
sudo apt-get install gdb-avr
```

## Running the tests

### Prerequisites
* ASA M128 x 1
* ASA SDC00 x 1
* MicroSD Card(FAT32) x 1

### Steps
#### 1. Compile

```
make
```
#### 2. Uploading test program
1. 接好`ASA SDC00`
2. 上傳`main.hex`至`ASA M128`


Burning your `main.hex` file via ASA Uploader and run the program

## Deployment

尚未完成`Makefile`，未來將加入```make install```

## Authors

* **Li-wu Chen** - *Initial work*
* **Cheng-wei Ye** - *Developing with new specification* [TaiwanET](https://github.com/a0919958057/)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## Acknowledgments

* AVR mcu
* FatFs library
