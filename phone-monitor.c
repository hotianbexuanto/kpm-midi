/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2025 Your Name. All Rights Reserved.
 */

#include <compiler.h>
#include <kpmodule.h>
#include <linux/printk.h>
#include <log.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <asm/current.h>
#include <kputils.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/time.h>
#include <linux/cred.h>

///< The name of the module, each KPM must has a unique name.
KPM_NAME("kpm-phone-monitor");

///< The version of the module.
KPM_VERSION("1.0.0");

///< The license type.
KPM_LICENSE("GPL v2");

///< The author.
KPM_AUTHOR("Your Name");

///< The description.
KPM_DESCRIPTION("KernelPatch Module for Monitoring Phone Information");

// Proc entry for exposing information
static struct proc_dir_entry *proc_entry;

// Buffer to store phone information
static char info_buffer[8192];
static int info_len = 0;

/**
 * @brief Collect phone information
 * @details This function collects various phone information such as process list, memory usage, etc.
 */
static void collect_phone_info()
{
    logkd("Collecting phone information...\n");
    
    // Reset buffer
    memset(info_buffer, 0, sizeof(info_buffer));
    info_len = 0;
    
    // Add header
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<html><head><title>Phone Monitor</title>"
                        "<meta charset=\"utf-8\">"
                        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                        "<style>"
                        "body { font-family: Arial, sans-serif; margin: 20px; background-color: #f0f0f0; }"
                        ".container { max-width: 800px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }"
                        "h1 { color: #333; text-align: center; }"
                        "h2 { color: #666; border-bottom: 1px solid #eee; padding-bottom: 10px; }"
                        "p { margin: 10px 0; }"
                        ".info-item { background-color: #f9f9f9; border: 1px solid #ddd; padding: 15px; margin: 15px 0; border-radius: 5px; }"
                        "</style>"
                        "</head><body>");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<div class=\"container\">");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<h1>Phone Information</h1>");
    
    // Add current time
    struct timespec64 ts;
    ktime_get_real_ts64(&ts);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<div class=\"info-item\">");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<h2>Timestamp</h2>");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p>%lld.%09ld</p>", ts.tv_sec, ts.tv_nsec);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "</div>");
    
    // Add current process information
    struct task_struct *task = current;
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<div class=\"info-item\">");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<h2>Current Process</h2>");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p><strong>PID:</strong> %d</p>", task->pid);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p><strong>Name:</strong> %s</p>", task->comm);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p><strong>Parent PID:</strong> %d</p>", task->real_parent->pid);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "</div>");
    
    // Add UID/GID information
    const struct cred *cred = current_cred();
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<div class=\"info-item\">");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<h2>User Information</h2>");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p><strong>UID:</strong> %d</p>", cred->uid.val);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p><strong>GID:</strong> %d</p>", cred->gid.val);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p><strong>EUID:</strong> %d</p>", cred->euid.val);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p><strong>EGID:</strong> %d</p>", cred->egid.val);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "</div>");
    
    // Add kernel information
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<div class=\"info-item\">");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<h2>Kernel Information</h2>");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<p><strong>Version:</strong> %s</p>", linux_proc_banner);
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "</div>");
    
    // Add refresh button
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<div style=\"text-align: center; margin: 20px 0;\">");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "<button onclick=\"location.reload()\" style=\"background-color: #4CAF50; border: none; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 5px;\">Refresh Information</button>");
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "</div>");
    
    // End HTML
    info_len += snprintf(info_buffer + info_len, sizeof(info_buffer) - info_len, 
                        "</div></body></html>");
    
    logkd("Phone information collected\n");
}

/**
 * @brief Proc read operation
 * @details This function is called when the proc entry is read
 */
static int phone_info_proc_show(struct seq_file *m, void *v)
{
    // Collect fresh information
    collect_phone_info();
    
    // Output the information
    seq_printf(m, "%s", info_buffer);
    return 0;
}

/**
 * @brief Proc open operation
 */
static int phone_info_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, phone_info_proc_show, NULL);
}

/**
 * @brief Proc file operations
 */
static const struct proc_ops phone_info_proc_ops = {
    .proc_open = phone_info_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

/**
 * @brief phone monitor module initialization
 * @details This function is called when the module is loaded
 * 
 * @param args arguments passed to the module
 * @param event event that triggered the module loading
 * @param reserved reserved parameter
 * @return long 0 on success, non-zero on failure
 */
static long phone_monitor_init(const char *args, const char *event, void *__user reserved)
{
    pr_info("kpm phone monitor init, event: %s, args: %s\n", event, args);
    logkd("此模块kpm-phone-monitor加载成功\n");
    
    // Create proc entry
    proc_entry = proc_create("phone_monitor", 0444, NULL, &phone_info_proc_ops);
    if (!proc_entry) {
        logke("Failed to create proc entry\n");
        return -1;
    }
    
    // Collect initial phone information
    collect_phone_info();
    
    logki("Phone monitor module loaded successfully. Access information at /proc/phone_monitor\n");
    return 0;
}

/**
 * @brief phone monitor module control function 0
 * @details This function handles control requests from user space
 * 
 * @param args arguments passed from user space
 * @param out_msg output message to user space
 * @param outlen length of output message buffer
 * @return long 0 on success, non-zero on failure
 */
static long phone_monitor_control0(const char *args, char *__user out_msg, int outlen)
{
    pr_info("kpm phone monitor control0, args: %s\n", args);
    logkd("kpm phone monitor control0 called\n");
    
    // Refresh phone information
    collect_phone_info();
    
    // Copy information to user space
    if (out_msg && outlen > 0) {
        int copy_len = info_len < outlen ? info_len : outlen;
        compat_copy_to_user(out_msg, info_buffer, copy_len);
        return copy_len;
    }
    
    return 0;
}

/**
 * @brief phone monitor module exit function
 * @details This function is called when the module is unloaded
 * 
 * @param reserved reserved parameter
 * @return long 0 on success, non-zero on failure
 */
static long phone_monitor_exit(void *__user reserved)
{
    pr_info("kpm phone monitor exit\n");
    logkd("kpm phone monitor module unloaded\n");
    
    // Remove proc entry
    if (proc_entry) {
        proc_remove(proc_entry);
    }
    
    // Clear buffer
    memset(info_buffer, 0, sizeof(info_buffer));
    info_len = 0;
    
    return 0;
}

KPM_INIT(phone_monitor_init);
KPM_CTL0(phone_monitor_control0);
KPM_EXIT(phone_monitor_exit);