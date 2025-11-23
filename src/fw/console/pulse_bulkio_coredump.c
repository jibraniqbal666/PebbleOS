/*
 * Copyright 2024 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pulse_bulkio_domain_handler.h"

#include "console/pulse_protocol_impl.h"
#include "drivers/flash.h"
#include "kernel/core_dump.h"
#include "kernel/core_dump_private.h"
#include "system/status_codes.h"
#include "util/attributes.h"

#include <stdint.h>

typedef struct PACKED CoredumpStatResp {
  uint8_t flags;
  uint8_t unread;
  uint32_t size;
} CoredumpStatResp;

static uint32_t prv_get_coredump_index(void *packet_data) {
  return *(uint32_t*)packet_data;
}

static int coredump_domain_read(uint8_t *buf, uint32_t address, uint32_t length,
                                     void *context) {
  uint32_t index = (uintptr_t)context;
  uint32_t core_base_addr = core_dump_get_slot_address(index) + sizeof(CoreDumpFlashRegionHeader);
  flash_read_bytes(buf, core_base_addr + address, length);
  return length;
}

static int coredump_domain_write(uint8_t *buf, uint32_t address, uint32_t length,
                                      void *context) {
  return E_INVALID_OPERATION;
}

static int coredump_domain_stat(uint8_t *resp, size_t resp_max_len, void *context) {
  uint32_t index = (uintptr_t)context;
  uint32_t addr = core_dump_get_slot_address(index);

  CoredumpStatResp *stat_resp = (CoredumpStatResp*)resp;
  *stat_resp = (CoredumpStatResp) {
    .flags = 0,
    .unread = core_dump_is_unread_available(addr),
    // Size of 0 indicates no core dump available
    .size = 0
  };

  if (stat_resp->unread == 1) {
    // Copy packed member to local variable to avoid taking address of packed member
    uint32_t size = 0;
    status_t ret = core_dump_size(addr, &size);
    stat_resp->size = size;

    if (FAILED(ret)) {
      return ret;
    }
  }

  return sizeof(CoredumpStatResp);
}

static status_t coredump_domain_erase(uint8_t *packet_data, size_t length, uint8_t cookie) {
  uint32_t index = prv_get_coredump_index(packet_data);
  uint32_t addr = core_dump_get_slot_address(index);
  core_dump_mark_read(addr);
  return S_SUCCESS;
}

static status_t coredump_domain_open(uint8_t *packet_data, size_t length, void **resp) {
  *resp = (void*)(uintptr_t)prv_get_coredump_index(packet_data);
  return S_SUCCESS;
}

static status_t coredump_domain_close(void *context) {
  return S_SUCCESS;
}

PulseBulkIODomainHandler pulse_bulkio_domain_coredump = {
  .id = PulseBulkIODomainType_Coredump,
  .open_proc = coredump_domain_open,
  .close_proc = coredump_domain_close,
  .read_proc = coredump_domain_read,
  .write_proc = coredump_domain_write,
  .stat_proc = coredump_domain_stat,
  .erase_proc = coredump_domain_erase
};
