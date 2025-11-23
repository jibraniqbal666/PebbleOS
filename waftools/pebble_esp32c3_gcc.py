# Copyright 2024 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
from waflib.Configure import conf

def options(opt):
    pass

def configure(conf):
    # RISC-V ESP32-C3 toolchain prefix
    CROSS_COMPILE_PREFIX = 'riscv32-esp-elf-'

    conf.env.AS = CROSS_COMPILE_PREFIX + 'gcc'
    conf.env.AR = CROSS_COMPILE_PREFIX + 'gcc-ar'
    conf.env.CC = CROSS_COMPILE_PREFIX + 'gcc'
    conf.env.LINK_CC = conf.env.CC
    
    # Note: Architecture flags will be set later in args, don't set them here
    # Setting them early can cause issues with flag ordering

    conf.find_program('ccache', var='CCACHE', mandatory=False)
    if conf.env.CCACHE:
        conf.env.CC = [conf.env.CCACHE[0], conf.env.CC]

    conf.load('gcc')

    conf.env.append_value('CFLAGS', ['-std=c11'])  # C11 standard
    # Note: Waf should handle C vs C++ based on file extension
    # If asm keyword issues persist, we may need to ensure files are .c not .cpp
    
    # Force include asm_compat.h before any ESP-IDF headers to fix 'asm' keyword issues
    # This ensures 'asm' is defined as '__asm__' before ESP-IDF headers are processed
    # Calculate path relative to this tool file's location
    tool_dir = os.path.dirname(os.path.abspath(__file__))
    workspace_root = os.path.dirname(tool_dir)  # Go up from waftools/ to workspace root
    hal_esp32c3_path = os.path.join(workspace_root, 'third_party', 'hal_esp32c3')
    asm_compat_path = os.path.join(hal_esp32c3_path, 'asm_compat.h')
    if os.path.exists(asm_compat_path):
        conf.env.append_value('CFLAGS', ['-include', asm_compat_path])

    c_warnings = [
        '-Wall',
        '-Wextra',
        '-Werror',
        '-Wpointer-arith',
        '-Wno-unused-parameter',
        '-Wno-missing-field-initializers',
        '-Wno-error=unused-function',
        '-Wno-error=unused-variable',
        '-Wno-error=unused-parameter',
    ]

    conf.env.append_value('CFLAGS', c_warnings)

    conf.add_platform_defines(conf.env)

    # RISC-V ESP32-C3 CPU flags
    # rv32imc = 32-bit RISC-V with Integer, Multiply, Compressed extensions
    # zicsr = Control and Status Register extension (required for csrr/csrw instructions)
    # Note: ESP32-C3 supports zicsr, but we need to explicitly enable it in the architecture string
    args = [
        '-march=rv32imczicsr',  # Added zicsr extension for CSR instructions (mcycle, etc.)
        '-mabi=ilp32',
        '-ffreestanding',
        '-ffunction-sections',
        '-fbuiltin',
        '-fshort-enums',  # Use smallest enum size for RISC-V (fixes Bluetooth enum size issues)
    ]

    if not conf.options.no_debug:
        args += ['-g3', '-gdwarf-4']

    # Set ASFLAGS first with architecture flags, then add assembler-specific flags
    # This ensures the assembler knows about zicsr extension when processing inline assembly
    conf.env.ASFLAGS = ['-xassembler-with-cpp', '-c'] + args
    conf.env.AS_TGT_F = '-o'

    conf.env.append_value('LINKFLAGS', ['-Wl,--warn-common'])

    # Add architecture flags to CFLAGS
    # For inline assembly processing, GCC should automatically pass -march to the assembler,
    # but we also explicitly pass it via -Wa to ensure the assembler recognizes zicsr extension
    # Add architecture flags first
    conf.env.append_value('CFLAGS', args)
    # Then explicitly pass architecture to assembler for inline assembly processing
    # The assembler needs to know about zicsr extension for CSR instructions
    # IMPORTANT: -Wa flag must come AFTER -march flag for proper processing
    conf.env.append_value('CFLAGS', ['-Wa,-march=rv32imczicsr'])
    # ASFLAGS already set above with architecture flags
    conf.env.append_value('LINKFLAGS', args)

    conf.env.SHLIB_MARKER = None
    conf.env.STLIB_MARKER = None

    # Set optimization level
    if conf.options.beta:
        optimize_flags = '-Os'
    elif conf.options.release:
        optimize_flags = '-Os'
    elif conf.options.fat_firmware:
        optimize_flags = '-O0'
        conf.env.IS_FAT_FIRMWARE = True
    elif conf.options.gdb:
        optimize_flags = '-Og'
    else:
        optimize_flags = '-Os'

    conf.env.append_value('CFLAGS', optimize_flags)
    conf.env.append_value('LINKFLAGS', optimize_flags)

