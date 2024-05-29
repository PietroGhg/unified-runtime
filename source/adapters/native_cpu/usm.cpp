//===------------- usm.cpp - NATIVE CPU Adapter ---------------------------===//
//
// Copyright (C) 2023 Intel Corporation
//
// Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM
// Exceptions. See LICENSE.TXT
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ur/ur.hpp"
#include "ur_api.h"

#include "common.hpp"
#include "context.hpp"

UR_APIEXPORT ur_result_t UR_APICALL
urUSMHostAlloc(ur_context_handle_t hContext, const ur_usm_desc_t *pUSMDesc,
               ur_usm_pool_handle_t pool, size_t size, void **ppMem) {
  std::ignore = pUSMDesc;
  std::ignore = pool;

  UR_ASSERT(ppMem, UR_RESULT_ERROR_INVALID_NULL_POINTER);
  // TODO: Check Max size when UR_DEVICE_INFO_MAX_MEM_ALLOC_SIZE is implemented
  UR_ASSERT(size > 0, UR_RESULT_ERROR_INVALID_USM_SIZE);

  void *ptr = malloc(size);
  hContext->add_alloc_info_entry(ptr, UR_USM_TYPE_HOST, size, nullptr);
  *ppMem = ptr;

  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL
urUSMDeviceAlloc(ur_context_handle_t hContext, ur_device_handle_t hDevice,
                 const ur_usm_desc_t *pUSMDesc, ur_usm_pool_handle_t pool,
                 size_t size, void **ppMem) {
  std::ignore = hDevice;
  std::ignore = pUSMDesc;
  std::ignore = pool;

  UR_ASSERT(ppMem, UR_RESULT_ERROR_INVALID_NULL_POINTER);
  // TODO: Check Max size when UR_DEVICE_INFO_MAX_MEM_ALLOC_SIZE is implemented
  UR_ASSERT(size > 0, UR_RESULT_ERROR_INVALID_USM_SIZE);

  void *ptr = malloc(size);
  *ppMem = ptr;
  hContext->add_alloc_info_entry(ptr, UR_USM_TYPE_DEVICE, size, nullptr);

  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL
urUSMSharedAlloc(ur_context_handle_t hContext, ur_device_handle_t hDevice,
                 const ur_usm_desc_t *pUSMDesc, ur_usm_pool_handle_t pool,
                 size_t size, void **ppMem) {
  std::ignore = hDevice;
  std::ignore = pUSMDesc;
  std::ignore = pool;

  UR_ASSERT(ppMem, UR_RESULT_ERROR_INVALID_NULL_POINTER);
  // TODO: Check Max size when UR_DEVICE_INFO_MAX_MEM_ALLOC_SIZE is implemented
  UR_ASSERT(size > 0, UR_RESULT_ERROR_INVALID_USM_SIZE);

  void *ptr = malloc(size);
  *ppMem = ptr;
  hContext->add_alloc_info_entry(ptr, UR_USM_TYPE_SHARED, size, nullptr);

  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL urUSMFree(ur_context_handle_t hContext,
                                              void *pMem) {

  UR_ASSERT(pMem, UR_RESULT_ERROR_INVALID_NULL_POINTER);

  hContext->remove_alloc_info_entry(pMem);
  free(pMem);

  return UR_RESULT_SUCCESS;
}

UR_APIEXPORT ur_result_t UR_APICALL
urUSMGetMemAllocInfo(ur_context_handle_t hContext, const void *pMem,
                     ur_usm_alloc_info_t propName, size_t propSize,
                     void *pPropValue, size_t *pPropSizeRet) {

  UR_ASSERT(pMem != nullptr, UR_RESULT_ERROR_INVALID_NULL_POINTER);
  UrReturnHelper ReturnValue(propSize, pPropValue, pPropSizeRet);
  if (propName == UR_USM_ALLOC_INFO_BASE_PTR) {
    //TODO: logic to compute base ptr given ptr
    DIE_NO_IMPLEMENTATION;
  }

  native_cpu::usm_alloc_info alloc_info = hContext->get_alloc_info_entry(pMem);
  switch (propName) {
  case UR_USM_ALLOC_INFO_TYPE:
    return ReturnValue(alloc_info.type);
  case UR_USM_ALLOC_INFO_SIZE:
    return ReturnValue(alloc_info.size);
  case UR_USM_ALLOC_INFO_DEVICE:
    return ReturnValue(alloc_info.device);
  case UR_USM_ALLOC_INFO_POOL:
    return UR_RESULT_ERROR_INVALID_ENUMERATION;
  default:
    DIE_NO_IMPLEMENTATION;
  }
  return UR_RESULT_ERROR_INVALID_VALUE;
}

UR_APIEXPORT ur_result_t UR_APICALL
urUSMPoolCreate(ur_context_handle_t hContext, ur_usm_pool_desc_t *pPoolDesc,
                ur_usm_pool_handle_t *ppPool) {
  std::ignore = hContext;
  std::ignore = pPoolDesc;
  std::ignore = ppPool;

  DIE_NO_IMPLEMENTATION;
}

UR_APIEXPORT ur_result_t UR_APICALL
urUSMPoolRetain(ur_usm_pool_handle_t pPool) {
  std::ignore = pPool;

  DIE_NO_IMPLEMENTATION;
}

UR_APIEXPORT ur_result_t UR_APICALL
urUSMPoolRelease(ur_usm_pool_handle_t pPool) {
  std::ignore = pPool;

  DIE_NO_IMPLEMENTATION;
}

UR_APIEXPORT ur_result_t UR_APICALL
urUSMPoolGetInfo(ur_usm_pool_handle_t hPool, ur_usm_pool_info_t propName,
                 size_t propSize, void *pPropValue, size_t *pPropSizeRet) {
  std::ignore = hPool;
  std::ignore = propName;
  std::ignore = propSize;
  std::ignore = pPropValue;
  std::ignore = pPropSizeRet;

  DIE_NO_IMPLEMENTATION;
}

UR_APIEXPORT ur_result_t UR_APICALL urUSMImportExp(ur_context_handle_t Context,
                                                   void *HostPtr, size_t Size) {
  std::ignore = Context;
  std::ignore = HostPtr;
  std::ignore = Size;
  DIE_NO_IMPLEMENTATION;
}

UR_APIEXPORT ur_result_t UR_APICALL urUSMReleaseExp(ur_context_handle_t Context,
                                                    void *HostPtr) {
  std::ignore = Context;
  std::ignore = HostPtr;
  DIE_NO_IMPLEMENTATION;
}
