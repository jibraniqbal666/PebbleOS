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

import subprocess
import sys
import waflib
from waflib import Logs, Context


def _has_esptool(ctx):
    try:
        ctx.cmd_and_log(['which', 'esptool.py'], quiet=waflib.Context.BOTH)
        return True
    except:
        return False


def flash_bootloader(ctx, bootloader_bin, port=None, baud=921600):
    """Flash bootloader to ESP32-C3"""
    if not _has_esptool(ctx):
        ctx.fatal("esptool.py not found. Please install ESP-IDF or esptool.py")

    cmd = ['esptool.py', '--chip', 'esp32c3']
    
    if port:
        cmd.extend(['--port', port])
    else:
        # Try to auto-detect port
        cmd.append('--port')
        cmd.append('auto')
    
    cmd.extend(['--baud', str(baud)])
    cmd.extend(['write_flash', '0x0', bootloader_bin])

    Logs.info("Flashing bootloader to ESP32-C3...")
    Logs.info("Command: %s" % ' '.join(cmd))
    
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        Logs.info(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        Logs.error("Failed to flash bootloader:")
        Logs.error(e.stderr)
        ctx.fatal("Flash failed")


def write_cfg(ctx):
    """Write esptool configuration (placeholder for future use)"""
    pass

