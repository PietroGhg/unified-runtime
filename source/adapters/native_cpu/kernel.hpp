//===--------------- kernel.hpp - Native CPU Adapter ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#pragma once

#include "common.hpp"
#include "nativecpu_state.hpp"
#include "program.hpp"
#include <cstring>
#include <ur_api.h>
#include <utility>

using nativecpu_kernel_t = void(void *, native_cpu::state *);
using nativecpu_ptr_t = nativecpu_kernel_t *;
using nativecpu_task_t = std::function<nativecpu_kernel_t>;

struct local_arg_info_t {
  uint32_t argIndex;
  size_t argSize;
  local_arg_info_t(uint32_t argIndex, size_t argSize)
      : argIndex(argIndex), argSize(argSize) {}
};

struct ur_kernel_handle_t_ : RefCounted {


  ur_kernel_handle_t_(ur_program_handle_t hProgram, const char *name,
                      nativecpu_task_t subhandler)
      : hProgram(hProgram), _name{name}, _subhandler{std::move(subhandler)},
        HasReqdWGSize(false) {}

  ur_kernel_handle_t_(const ur_kernel_handle_t_ &other)
      : Args(other.Args), hProgram(other.hProgram), _name(other._name),
        _subhandler(other._subhandler), 
        _localArgInfo(other._localArgInfo), _localMemPool(other._localMemPool),
        _localMemPoolSize(other._localMemPoolSize),  
        HasReqdWGSize(other.HasReqdWGSize), ReqdWGSize(other.ReqdWGSize) {
    incrementReferenceCount();
  }

  ~ur_kernel_handle_t_() {
    if (decrementReferenceCount() == 0) {
      free(_localMemPool);
    }
  }

  ur_kernel_handle_t_(ur_program_handle_t hProgram, const char *name,
                      nativecpu_task_t subhandler,
                      const native_cpu::ReqdWGSize_t &ReqdWGSize)
      : hProgram(hProgram), _name{name}, _subhandler{std::move(subhandler)},
        HasReqdWGSize(true), ReqdWGSize(ReqdWGSize) {}


  struct arguments {
    static constexpr size_t MaxParamBytes = 4000u;
    using args_t = std::array<char, MaxParamBytes>;
    using args_size_t = std::vector<size_t>;
    using args_index_t = std::vector<void *>;
    args_t Storage;
    args_size_t ParamSizes;
    args_index_t Indices;

    /// Add an argument to the kernel.
    /// If the argument existed before, it is replaced.
    /// Otherwise, it is added.
    /// Gaps are filled with empty arguments.
    /// Implicit offset argument is kept at the back of the indices collection.
    void addArg(size_t Index, size_t Size, const void *Arg) {
      if (Index + 1 > Indices.size()) {
        Indices.resize(Index + 1);
        // Ensure enough space for the new argument
        ParamSizes.resize(Index + 1);
      }
      ParamSizes[Index] = Size;
      // calculate the insertion point on the array
      size_t InsertPos = std::accumulate(std::begin(ParamSizes),
                                         std::begin(ParamSizes) + Index, 0);
      // Update the stored value for the argument
      std::memcpy(&Storage[InsertPos], Arg, Size);
      Indices[Index] = &Storage[InsertPos];
    }

    void addPtrArg(size_t Index, void *Arg) {
      constexpr size_t Size = sizeof(uint8_t*);
      if (Index + 1 > Indices.size()) {
        Indices.resize(Index + 1);
        // Ensure enough space for the new argument
        ParamSizes.resize(Index + 1);
      }
      ParamSizes[Index] = Size;
      Indices[Index] = Arg;
    }

    const args_index_t &getIndices() const noexcept { return Indices; }

  } Args;

  ur_program_handle_t hProgram;
  std::string _name;
  nativecpu_task_t _subhandler;
  std::vector<local_arg_info_t> _localArgInfo;

  bool hasReqdWGSize() const { return HasReqdWGSize; }

  const native_cpu::ReqdWGSize_t &getReqdWGSize() const { return ReqdWGSize; }

  void updateMemPool(size_t numParallelThreads) {
    // compute requested size.
    size_t reqSize = 0;
    for (auto &entry : _localArgInfo) {
      reqSize += entry.argSize * numParallelThreads;
    }
    if (reqSize == 0 || reqSize == _localMemPoolSize) {
      return;
    }
    // realloc handles nullptr case
    _localMemPool = (char *)realloc(_localMemPool, reqSize);
    _localMemPoolSize = reqSize;
  }

  // To be called before executing a work group
  void handleLocalArgs(size_t numParallelThread, size_t threadId) {
    // For each local argument we have size*numthreads
    size_t offset = 0;
    for (auto &entry : _localArgInfo) {
      Args.Indices[entry.argIndex] =
          _localMemPool + offset + (entry.argSize * threadId);
      // update offset in the memory pool
      offset += entry.argSize * numParallelThread;
    }
  }

  std::vector<void *> getArgs() const {
    return Args.getIndices();
  }

  void addArg(void *Ptr, size_t Index, size_t Size) {
    Args.addArg(Index, Size, Ptr);
  }
  
  void addPtrArg(void *Ptr, size_t Index) {
    Args.addPtrArg(Index, Ptr);
  }

private:
  char *_localMemPool = nullptr;
  size_t _localMemPoolSize = 0;
  bool HasReqdWGSize;
  native_cpu::ReqdWGSize_t ReqdWGSize;
};
