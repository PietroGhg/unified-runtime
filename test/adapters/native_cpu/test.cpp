#include "kernel.hpp"
#include "program.hpp"
#include "context.hpp"
#include "platform.hpp"
#include "queue.hpp"
#include "ur_api.h"
#include <uur/fixtures.h>

using my_native_cpu_test = uur::urDeviceTest;
UUR_INSTANTIATE_DEVICE_TEST_SUITE_P(my_native_cpu_test);

// normally the device compiler adds the state parameter to the kernel
void my_kernel(int* a, native_cpu::state * s) {
  a[s->MGlobal_id[0]] = 42;
}

// the subhandler is usually emitted by the device compiler
void my_kernel_handler(const native_cpu::NativeCPUArgDesc *args, native_cpu::state *s) {
  auto ptr1 = (int*)args[0].MPtr;
  my_kernel(ptr1, s);
}

TEST_P(my_native_cpu_test, test) {

  // the nativecpu_entry array is usually emitted by clang-offload-wrapper
  nativecpu_entry entry;
  entry.kernelname = "my_kernel";
  entry.kernel_ptr = (const unsigned char*)(&my_kernel_handler);
  nativecpu_entry term{"foo", nullptr};
  nativecpu_entry entries[2]{entry, term};
  ur_platform_handle_t_ platform;
  ur_context_handle_t_ context(&platform.TheDevice);


  ur_program_handle_t program;
  urProgramCreateWithBinary(&context, &platform.TheDevice, 0, (uint8_t*)&entries, nullptr, &program);

  ur_kernel_handle_t kernel;
  urKernelCreate(program, "my_kernel", &kernel);


  constexpr size_t N = 3;
  int a[N];
  urKernelSetArgValue(kernel, 0, 1, nullptr, &a);

  ur_queue_handle_t_ queue(&platform.TheDevice);
  size_t offset[3]{0,0,0};
  size_t local[3]{1,1,1};
  size_t global[3]{N,0,0};

  urEnqueueKernelLaunch(&queue, kernel, 1, &offset[0], &global[0], &local[0], 0, nullptr, nullptr);
  for(unsigned i = 0; i < N; i++) {
    EXPECT_EQ(42, a[i]);
  }
}
