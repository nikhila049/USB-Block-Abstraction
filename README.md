# USB_Block_Abstraction

This project implements a Linux kernel module that provides user-space programs direct access to a virtual USB storage device using the Linux block I/O (BIO) abstraction. The module exposes a simple IOCTL interface to perform low-level block reads and writes, similar to utilities like `dd` or `mkfs`.

## Project Summary

The kernel module interacts with a 1GB virtual USB block device, allowing:

- Direct read/write access at 512-byte block granularity
- Adjustable read/write offsets
- Support for variable-sized operations, internally split into multiple BIO operations as needed

This project provides a practical implementation of block device handling within the Linux kernel, showcasing how kernel modules manage storage at a low level.

## Features

- Read and write operations using the Linux BIO interface
- Four IOCTL commands implemented:
  - `READ(size)` – reads from current offset
  - `WRITE(size)` – writes to current offset
  - `READOFFSET(size, offset)` – reads from a specified offset
  - `WRITEOFFSET(size, offset)` – writes to a specified offset
- Handles user-space and kernel-space memory safely using appropriate kernel interfaces
- All I/O operations are performed in 512-byte aligned chunks, in accordance with BIO limitations

## Technologies Used

- C (Linux kernel module development)
- Linux Block I/O subsystem (BIO)
- Bash (test automation scripts)
- VirtualBox / VMWare / UTM (for USB device simulation)
- IOCTL system calls for user-kernel communication

## IOCTL Usage

The kernel module provides the following IOCTL commands to interact with a virtual USB storage device (e.g., `/dev/sdb`):

### 1. `READ(buffer, size)`

- Reads `size` bytes from the current device offset into `buffer`
- Automatically updates the internal offset after the operation

### 2. `WRITE(buffer, size)`

- Writes `size` bytes from `buffer` to the current device offset
- Updates the internal offset after writing

### 3. `READOFFSET(buffer, size, offset)`

- Reads `size` bytes from the specified `offset` into `buffer`
- Sets internal offset to `offset` after the operation

### 4. `WRITEOFFSET(buffer, size, offset)`

- Writes `size` bytes from `buffer` to the specified `offset`
- Sets internal offset to `offset` after the operation

## How It Works

1. The module opens the virtual USB device using `bdev_file_open_by_path`.
2. A `block_device` pointer is obtained and used to prepare BIO structures.
3. Memory pages are attached using `bio_add_page`, and the BIO is submitted using `submit_bio_wait`.
4. The module copies data between user space and kernel space using standard kernel APIs like `copy_to_user` and `copy_from_user`.
5. For larger transfers, operations are broken into 512-byte aligned chunks and processed iteratively.
