# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
set(BOARD native_POSIX_64)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(serial_crypto)

target_sources(app PRIVATE src/main.c)
