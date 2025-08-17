/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2025 hotian/ai. All Rights Reserved.
 */

#include <compiler.h>
#include <kpmodule.h>
#include <linux/printk.h>
#include <log.h>
#include <linux/umh.h>
#include <linux/slab.h>

///< The name of the module, each KPM must has a unique name.
KPM_NAME("kpm-midi");

///< The version of the module.
KPM_VERSION("1.0.0");

///< The license type.
KPM_LICENSE("GPL v2");

///< The author.
KPM_AUTHOR("hotian/ai");

///< The description.
KPM_DESCRIPTION("KernelPatch MIDI Module");

/**
 * @brief Play a midi sound using usermode helper
 * @details This function calls tinyplay to play a midi sound
 * 
 * @return int 0 on success, non-zero on failure
 */
static int play_midi_sound(void)
{
    char *argv[] = { "/system/bin/tinyplay", "/system/media/audio/notifications/test.mid", NULL };
    char *envp[] = { "PATH=/system/bin:/system/xbin", NULL };
    
    // Try to call tinyplay to play a midi sound
    int ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
    
    // If tinyplay is not available, try am command
    if (ret < 0) {
        char *argv_am[] = { "/system/bin/am", "start", 
                           "--user", "0", 
                           "--activity-clear-top", 
                           "-a", "android.intent.action.VIEW", 
                           "-d", "file:///system/media/audio/notifications/test.mid", 
                           "-t", "audio/midi", NULL };
        ret = call_usermodehelper(argv_am[0], argv_am, envp, UMH_NO_WAIT);
    }
    
    return ret;
}

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
    logkd("[kpm-midi]此模块kpm-midi加载成功\n");
    
    // Play midi sound
    int ret = play_midi_sound();
    if (ret < 0) {
        logkd("[kpm-midi]Failed to play midi sound, error: %d\n", ret);
    } else {
        logkd("[kpm-midi]Midi sound played successfully\n");
    }
    
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
    logkd("[kpm-midi]kpm midi control0 called\n");
    
    // Play midi sound when control is called
    int ret = play_midi_sound();
    if (ret < 0) {
        logkd("[kpm-midi]Failed to play midi sound from control, error: %d\n", ret);
    } else {
        logkd("[kpm-midi]Midi sound played successfully from control\n");
    }
    
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
    logkd("[kpm-midi]kpm midi module unloaded\n");
    return 0;
}

KPM_INIT(midi_init);
KPM_CTL0(midi_control0);
KPM_EXIT(midi_exit);