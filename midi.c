/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2025 Your Name. All Rights Reserved.
 */

#include <compiler.h>
#include <kpmodule.h>
#include <linux/printk.h>
#include <log.h>

///< The name of the module, each KPM must has a unique name.
KPM_NAME("kpm-midi");

///< The version of the module.
KPM_VERSION("1.0.0");

///< The license type.
KPM_LICENSE("GPL v2");

///< The author.
KPM_AUTHOR("Your Name");

///< The description.
KPM_DESCRIPTION("KernelPatch MIDI Module");

/**
 * @brief midi module initialization
 * @details This function is called when the module is loaded
 * 
 * @param args arguments passed to the module
 * @param event event that triggered the module loading
 * @param reserved reserved parameter
 * @return long 0 on success, non-zero on failure
 */
static long midi_init(const char *args, const char *event, void *__user reserved)
{
    pr_info("kpm midi init, event: %s, args: %s\n", event, args);
    logkd("此模块kpm-midi加载成功\n");
    return 0;
}

/**
 * @brief midi module control function 0
 * @details This function handles control requests from user space
 * 
 * @param args arguments passed from user space
 * @param out_msg output message to user space
 * @param outlen length of output message buffer
 * @return long 0 on success, non-zero on failure
 */
static long midi_control0(const char *args, char *__user out_msg, int outlen)
{
    pr_info("kpm midi control0, args: %s\n", args);
    logkd("kpm midi control0 called\n");
    return 0;
}

/**
 * @brief midi module exit function
 * @details This function is called when the module is unloaded
 * 
 * @param reserved reserved parameter
 * @return long 0 on success, non-zero on failure
 */
static long midi_exit(void *__user reserved)
{
    pr_info("kpm midi exit\n");
    logkd("kpm midi module unloaded\n");
    return 0;
}

KPM_INIT(midi_init);
KPM_CTL0(midi_control0);
KPM_EXIT(midi_exit);