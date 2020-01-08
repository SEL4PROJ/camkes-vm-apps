/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */
#pragma once

#include <sel4utils/irq_server.h>
#include <sel4vm/guest_vm.h>
#include <sel4vmmplatsupport/drivers/cross_vm_connection.h>
#include <camkes/dataport_caps.h>

#include <plat/vmlinux.h>

#define MACH_TYPE_SPECIAL    ~0
#define MACH_TYPE            MACH_TYPE_SPECIAL

irq_callback_fn_t get_custom_irq_handler(ps_irq_t irq) WEAK;

/* Struct type that's passed into the IRQ callback functions for
 * this component */
struct irq_token {
    vm_t *vm;
    ps_irq_t irq;
    int virq;
    ps_irq_acknowledge_fn_t acknowledge_fn;
    void *ack_data;
};
typedef struct irq_token *irq_token_t;

typedef struct vmm_module {
    const char *name;
    void *cookie;
    // Function called for setup.
    void (*init_module)(vm_t *vm, void *cookie);
} ALIGN(32) vmm_module_t;

/**
 * Get the crossvm. This being an irq not passed through to the guest
 * such that it can be used for emulation purposes
 * @return  -1 for error, otherwise a positive value (irq number)
 */
int get_crossvm_irq_num(void);

struct camkes_crossvm_connection {
    dataport_caps_handle_t *handle;
    emit_fn emit_fn;
    seL4_Word consume_badge;
};

/**
 * Initialise and register a series of camkes crossvm connections with a given vm
 * @param[in] vm                    A handle to the VM
 * @param[in] connection_base_addr  The base guest physical address to interface the crossvm connection devices through
 * @param[in] connections           An array of camkes crossvm connections
 * @param[in] num_connection        The number of elements in the 'connections' array (parameter)
 * @return -1 for error, otherwise 0 for success
 */
int cross_vm_connections_init(vm_t *vm, uintptr_t connection_base_addr, struct camkes_crossvm_connection *connections,
                              int num_connections);

/* Declare a module.
 * For now, we put the modules in a separate elf section. */
#define DEFINE_MODULE(_name, _cookie, _init_module) \
    __attribute__((used)) __attribute__((section("_vmm_module"))) vmm_module_t VMM_MODULE_ ##_name = { \
    .name = #_name, \
    .cookie = _cookie, \
    .init_module = _init_module, \
};
