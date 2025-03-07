#include "sbi.h"
#include "printf.h"

#include "vm_defs.h"

#define SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE 0x08424b45

#define SBI_CALL(___ext, ___which, ___arg0, ___arg1, ___arg2)    \
  ({                                                             \
    register uintptr_t a0 __asm__("a0") = (uintptr_t)(___arg0);  \
    register uintptr_t a1 __asm__("a1") = (uintptr_t)(___arg1);  \
    register uintptr_t a2 __asm__("a2") = (uintptr_t)(___arg2);  \
    register uintptr_t a6 __asm__("a6") = (uintptr_t)(___which); \
    register uintptr_t a7 __asm__("a7") = (uintptr_t)(___ext);   \
    __asm__ volatile("ecall"                                     \
                     : "+r"(a0)                                  \
                     : "r"(a1), "r"(a2), "r"(a6), "r"(a7)        \
                     : "memory");                                \
    a0;                                                          \
  })

/* Lazy implementations until SBI is finalized */
#define SBI_CALL_0(___ext, ___which) SBI_CALL(___ext, ___which, 0, 0, 0)
#define SBI_CALL_1(___ext, ___which, ___arg0) SBI_CALL(___ext, ___which, ___arg0, 0, 0)
#define SBI_CALL_2(___ext, ___which, ___arg0, ___arg1) \
  SBI_CALL(___ext, ___which, ___arg0, ___arg1, 0)
#define SBI_CALL_3(___ext, ___which, ___arg0, ___arg1, ___arg2) \
  SBI_CALL(___ext, ___which, ___arg0, ___arg1, ___arg2)

#define SBI_ERR_SM_ENCLAVE_SUCCESS      0
#define SBI_ERR_SM_NOT_IMPLEMENTED 100100

// Eyrie uses the old timer extension par default, switch to the new one that Tyche supports
#define SBI_EXT_TIME  0x54494D45
#define SBI_EXT_TYCHE 0x5479636865

#define TYCHE_CALL_MANAGER 25

void
sbi_putchar(char character) {
  SBI_CALL_1(SBI_CONSOLE_PUTCHAR, 0, character);
}

void
sbi_set_timer(uint64_t stime_value) {
#if __riscv_xlen == 32
  SBI_CALL_2(SBI_EXT_TIME, 0, stime_value, stime_value >> 32);
#else
  SBI_CALL_1(SBI_EXT_TIME, 0, stime_value);
#endif
}

uintptr_t
sbi_stop_enclave(uint64_t request) {
  printf("[CC] sbi stop enclave with request: %lu\n", request);
  return SBI_CALL_3(SBI_EXT_TYCHE, 0, TYCHE_CALL_MANAGER, request, 0);
  /* return SBI_CALL_1(SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE, SBI_SM_STOP_ENCLAVE, request); */
}

void
sbi_exit_enclave(uint64_t retval) {
  printf("[CC] sbi exit enclave with retval %lu\n", retval);
  SBI_CALL_3(SBI_EXT_TYCHE, 0, TYCHE_CALL_MANAGER, retval, 1);
  /* SBI_CALL_1(SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE, SBI_SM_EXIT_ENCLAVE, retval); */
}

uintptr_t
sbi_random() {
  printf("[CC] sbi call random\n");
  return 0;
  // return SBI_CALL_0(SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE, SBI_SM_RANDOM);
}

uintptr_t
sbi_query_multimem() {
  printf("[CC] sbi query multimem\n");
  return 0; //SBI_ERR_SM_NOT_IMPLEMENTED;
  /* return SBI_CALL_2(SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE, */
  /*     SBI_SM_CALL_PLUGIN, SM_MULTIMEM_PLUGIN_ID, SM_MULTIMEM_CALL_GET_SIZE); */
}

uintptr_t
sbi_query_multimem_addr() {
  printf("[CC] sbi query multimem addr\n");
  return 0; //SBI_ERR_SM_NOT_IMPLEMENTED;
  /* return SBI_CALL_2(SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE, */
  /*     SBI_SM_CALL_PLUGIN, SM_MULTIMEM_PLUGIN_ID, SM_MULTIMEM_CALL_GET_ADDR); */
}

uintptr_t
sbi_attest_enclave(void* report, void* buf, uintptr_t len) {
  printf("[CC] sbi attest enclave\n");
  return SBI_ERR_SM_ENCLAVE_SUCCESS;
  /* return SBI_CALL_3(SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE, SBI_SM_ATTEST_ENCLAVE, report, buf, len); */
}

uintptr_t
sbi_get_sealing_key(uintptr_t key_struct, uintptr_t key_ident, uintptr_t len) {
  printf("[CC] sbi get sealing key\n");
  return SBI_ERR_SM_ENCLAVE_SUCCESS;
  /* return SBI_CALL_3(SBI_EXT_EXPERIMENTAL_KEYSTONE_ENCLAVE, SBI_SM_GET_SEALING_KEY, key_struct, key_ident, len); */
}
