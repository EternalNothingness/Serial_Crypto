#define CONFIG_BUILD_OUTPUT_EXE 1
#define CONFIG_BOARD "native_posix_64"
#define CONFIG_UART_NATIVE_POSIX 1
#define CONFIG_NATIVE_POSIX_CONSOLE 1
#define CONFIG_UART_CONSOLE 1
#define CONFIG_SOC "inf_clock"
#define CONFIG_ZEPHYR_HAL_NORDIC_MODULE 1
#define CONFIG_ZEPHYR_SOF_MODULE 1
#define CONFIG_ZEPHYR_TRUSTED_FIRMWARE_M_MODULE 1
#define CONFIG_ROM_START_OFFSET 0x0
#define CONFIG_ZEPHYR_NANOPB_MODULE 1
#define CONFIG_TINYCRYPT 1
#define CONFIG_TINYCRYPT_AES 1
#define CONFIG_TINYCRYPT_AES_CBC 1
#define CONFIG_TINYCRYPT_AES_CTR 1
#define CONFIG_TINYCRYPT_AES_CCM 1
#define CONFIG_TINYCRYPT_AES_CMAC 1
#define CONFIG_BOARD_NATIVE_POSIX_64BIT 1
#define CONFIG_BOARD_NATIVE_POSIX 1
#define CONFIG_NATIVE_POSIX_SLOWDOWN_TO_REAL_TIME 1
#define CONFIG_SOC_POSIX 1
#define CONFIG_ARCH "posix"
#define CONFIG_ARCH_POSIX_RECOMMENDED_STACK_SIZE 40
#define CONFIG_ARCH_POSIX 1
#define CONFIG_ARCH_IS_SET 1
#define CONFIG_64BIT 1
#define CONFIG_SRAM_SIZE 0
#define CONFIG_SRAM_BASE_ADDRESS 0x0
#define CONFIG_ARCH_HAS_THREAD_ABORT 1
#define CONFIG_MULTITHREADING 1
#define CONFIG_NUM_COOP_PRIORITIES 16
#define CONFIG_NUM_PREEMPT_PRIORITIES 15
#define CONFIG_MAIN_THREAD_PRIORITY 0
#define CONFIG_COOP_ENABLED 1
#define CONFIG_PREEMPT_ENABLED 1
#define CONFIG_PRIORITY_CEILING 0
#define CONFIG_NUM_METAIRQ_PRIORITIES 0
#define CONFIG_MAIN_STACK_SIZE 1024
#define CONFIG_IDLE_STACK_SIZE 256
#define CONFIG_ISR_STACK_SIZE 2048
#define CONFIG_ERRNO 1
#define CONFIG_SCHED_DUMB 1
#define CONFIG_WAITQ_DUMB 1
#define CONFIG_BOOT_BANNER 1
#define CONFIG_BOOT_DELAY 0
#define CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE 1024
#define CONFIG_SYSTEM_WORKQUEUE_PRIORITY -1
#define CONFIG_ATOMIC_OPERATIONS_BUILTIN 1
#define CONFIG_TIMESLICING 1
#define CONFIG_TIMESLICE_SIZE 0
#define CONFIG_TIMESLICE_PRIORITY 0
#define CONFIG_NUM_MBOX_ASYNC_MSGS 10
#define CONFIG_NUM_PIPE_ASYNC_MSGS 10
#define CONFIG_KERNEL_MEM_POOL 1
#define CONFIG_HEAP_MEM_POOL_SIZE 0
#define CONFIG_ARCH_HAS_CUSTOM_SWAP_TO_MAIN 1
#define CONFIG_ARCH_HAS_CUSTOM_BUSY_WAIT 1
#define CONFIG_SYS_CLOCK_TICKS_PER_SEC 100
#define CONFIG_SYS_CLOCK_HW_CYCLES_PER_SEC 1000000
#define CONFIG_SYS_CLOCK_EXISTS 1
#define CONFIG_TIMEOUT_64BIT 1
#define CONFIG_KERNEL_INIT_PRIORITY_OBJECTS 30
#define CONFIG_KERNEL_INIT_PRIORITY_DEFAULT 40
#define CONFIG_KERNEL_INIT_PRIORITY_DEVICE 50
#define CONFIG_APPLICATION_INIT_PRIORITY 90
#define CONFIG_MP_NUM_CPUS 1
#define CONFIG_TICKLESS_KERNEL 1
#define CONFIG_HAS_DTS 1
#define CONFIG_UART_CONSOLE_ON_DEV_NAME "UART_0"
#define CONFIG_CONSOLE 1
#define CONFIG_CONSOLE_INPUT_MAX_LINE_LEN 128
#define CONFIG_CONSOLE_HAS_DRIVER 1
#define CONFIG_UART_CONSOLE_INIT_PRIORITY 60
#define CONFIG_NATIVE_POSIX_STDOUT_CONSOLE 1
#define CONFIG_NATIVE_POSIX_CONSOLE_INIT_PRIORITY 60
#define CONFIG_SERIAL 1
#define CONFIG_SERIAL_HAS_DRIVER 1
#define CONFIG_NATIVE_UART_0_ON_OWN_PTY 1
#define CONFIG_NATIVE_UART_AUTOATTACH_DEFAULT_CMD "xterm -e screen %s &"
#define CONFIG_NATIVE_POSIX_TIMER 1
#define CONFIG_SYSTEM_CLOCK_INIT_PRIORITY 0
#define CONFIG_TICKLESS_CAPABLE 1
#define CONFIG_CRYPTO 1
#define CONFIG_CRYPTO_INIT_PRIORITY 90
#define CONFIG_CRYPTO_TINYCRYPT_SHIM 1
#define CONFIG_CRYPTO_TINYCRYPT_SHIM_MAX_SESSION 2
#define CONFIG_CRYPTO_TINYCRYPT_SHIM_DRV_NAME "CRYPTO_TC"
#define CONFIG_EXTERNAL_LIBC 1
#define CONFIG_SYS_HEAP_ALLOC_LOOPS 3
#define CONFIG_CBPRINTF_COMPLETE 1
#define CONFIG_CBPRINTF_FULL_INTEGRAL 1
#define CONFIG_CBPRINTF_N_SPECIFIER 1
#define CONFIG_POSIX_MAX_FDS 4
#define CONFIG_MAX_TIMER_COUNT 5
#define CONFIG_PRINTK 1
#define CONFIG_EARLY_CONSOLE 1
#define CONFIG_ASSERT_VERBOSE 1
#define CONFIG_TEST_EXTRA_STACKSIZE 0
#define CONFIG_HAS_COVERAGE_SUPPORT 1
#define CONFIG_LINKER_ORPHAN_SECTION_WARN 1
#define CONFIG_KERNEL_ENTRY "__start"
#define CONFIG_LINKER_SORT_BY_ALIGNMENT 1
#define CONFIG_SRAM_OFFSET 0x0
#define CONFIG_NATIVE_APPLICATION 1
#define CONFIG_SIZE_OPTIMIZATIONS 1
#define CONFIG_COMPILER_OPT ""
#define CONFIG_RUNTIME_ERROR_CHECKS 1
#define CONFIG_KERNEL_BIN_NAME "zephyr"
#define CONFIG_OUTPUT_STAT 1
#define CONFIG_OUTPUT_DISASSEMBLY 1
#define CONFIG_COMPAT_INCLUDES 1
