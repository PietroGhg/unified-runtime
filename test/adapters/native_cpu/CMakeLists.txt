# Copyright (C) 2023 Intel Corporation
# Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM Exceptions.
# See LICENSE.TXT
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_adapter_test(native_cpu
    FIXTURE DEVICES
    SOURCES
      test.cpp
    ENVIRONMENT
        "UR_ADAPTERS_FORCE_LOAD=\"$<TARGET_FILE:ur_adapter_native_cpu>\""
)

target_include_directories(test-adapter-native_cpu PRIVATE
    ${PROJECT_SOURCE_DIR}/source
    ${PROJECT_SOURCE_DIR}/source/adapters/native_cpu
)

