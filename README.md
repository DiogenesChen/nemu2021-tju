# ICS2021 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System) in College of Intelligence and Computing, Tianjin Univerisity.

This project is introduced from Nanjin University in 2016. Thank you for  Prof. Chunfeng Yuan of NJU and Dr. Zihao Yu of ICT.

The following subprojects/components are included. Some of them are not fully implemented.
* NEMU
* testcase
* uClibc
* kernel
* typing game
* NEMU-PAL

## NEMU

NEMU(NJU Emulator) is a simple but complete full-system x86 emulator designed for teaching. It is the main part of this programming assignment. Small x86 programs can run under NEMU. The main features of NEMU include
* a small monitor with a simple debugger
 * single step
 * register/memory examination
 * expression evaluation with the support of symbols
 * watch point
 * backtrace
* CPU core with support of most common used x86 instructions in protected mode
 * real mode is not supported
 * x87 floating point instructions are not supported
* DRAM with row buffer and burst
* two-level unified cache
* IA-32 segmentation and paging with TLB
 * protection is not supported
* IA-32 interrupt and exception
 * protection is not supported
* 6 devices
 * timer, keyboard, VGA, serial, IDE, i8259 PIC
 * most of them are simplified and unprogrammable
* 2 types of I/O
 * port-mapped I/O and memory-mapped I/O

## testcase

Some small C programs to test the implementation of NEMU.

## uClibc

uClibc(https://www.uclibc.org/) is a C library for embedding systems. It requires much fewer run-time support than glibc and is very friendly to NEMU.

## kernel

This is the simplified version of Nanos(http://cslab.nju.edu.cn/opsystem). It is a uni-tasking kernel with the following features.
* 2 device drivers
 * Ramdisk
 * IDE
* ELF32 loader
* memory management with paging
* a simple file system
 * with fix number and size of files
 * without directory
* 6 system calls
 * open, read, write, lseek, close, brk

## typing game

This is a fork of the demo of NJU 2013 oslab0(the origin repository has been deleted, but we have a fork of it -- https://github.com/nju-ics/os-lab0). It is ported to NEMU.

## NEMU-PAL

This is a fork of Wei Mingzhi's SDLPAL(https://github.com/CecilHarvey/sdlpal). It is obtained by refactoring the original SDLPAL, as well as porting to NEMU.


------

# ICS2021编程作业

本项目是天津大学智能与计算机学院ICS（计算机系统导论）班的编程作业。

本项目是2016年从南京大学引进的。感谢南京大学的袁春峰教授和ICT的余子豪博士。

包括以下子项目/组成部分。其中一些还没有完全实施。
* NEMU
* testcase
* uClibc
* 内核
* 打字游戏
* NEMU-PAL

## NEMU

NEMU(NJU Emulator)是一个简单但完整的全系统x86模拟器，为教学而设计。它是这个编程作业的主要部分。小型的x86程序可以在NEMU下运行。NEMU的主要特点包括
*一个带有简单调试器的小型显示器
 * 单一步骤
 * 寄存器/内存检查
 * 在符号的支持下进行表达式评估
 *观察点
 * 逆向跟踪
* 在保护模式下支持最常用的X86指令的CPU核心
 * 不支持实数模式
 * 不支持x87浮点指令
* 带有行缓冲器和突发的DRAM
* 两级统一的高速缓存
* 带有TLB的IA-32分段和分页
 不支持*保护
* IA-32中断和异常
 * 不支持保护
* 6个设备
 * 计时器、键盘、VGA、串行、IDE、i8259 PIC
 * 其中大部分是简化的，不可编程的
* 2种类型的I/O
 * 端口映射的I/O和内存映射的I/O

## 测试案例

一些小的C程序来测试NEMU的实现。

## uClibc

uClibc(https://www.uclibc.org/)是一个用于嵌入系统的C库。它需要的运行时支持比glibc少得多，对NEMU非常友好。

## kernel

这是Nanos(http://cslab.nju.edu.cn/opsystem)的简化版本。它是一个单任务的内核，具有以下特点。
* 2个设备驱动
 * Ramdisk
 * IDE
* ELF32加载器
* 带分页的内存管理
* 一个简单的文件系统
 * 具有固定数量和大小的文件
 *没有目录
* 6个系统调用
 * 打开、读、写、搜索、关闭、刷新

## 打字游戏

这是NJU 2013 oslab0的demo的分叉（起源库已被删除，但我们有它的分叉--https://github.com/nju-ics/os-lab0）。它被移植到了NEMU上。

## NEMU-PAL

这是Wei Mingzhi的SDLPAL(https://github.com/CecilHarvey/sdlpal)的一个分叉。它是通过重构原始的SDLPAL，以及移植到NEMU而得到的。

通过www.DeepL.com/Translator（免费版）翻译

----

这是天津大学2020级暑期学期(8.23—9.28)的计算机系统基础课的实践课程
