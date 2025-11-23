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

    conf.find_program('ccache', var='CCACHE', mandatory=False)
    if conf.env.CCACHE:
        conf.env.CC = [conf.env.CCACHE[0], conf.env.CC]

    conf.load('gcc')

    conf.env.append_value('CFLAGS', ['-std=c11'])

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

    conf.env.ASFLAGS = ['-xassembler-with-cpp', '-c']
    conf.env.AS_TGT_F = '-o'

    conf.env.append_value('LINKFLAGS', ['-Wl,--warn-common'])

    # RISC-V ESP32-C3 CPU flags
    args = [
        '-march=rv32imc',
        '-mabi=ilp32',
        '-ffreestanding',
        '-ffunction-sections',
        '-fbuiltin',
    ]

    if not conf.options.no_debug:
        args += ['-g3', '-gdwarf-4']

    conf.env.append_value('CFLAGS', args)
    conf.env.append_value('ASFLAGS', args)
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

