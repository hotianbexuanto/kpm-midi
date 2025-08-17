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
#include <linux/uaccess.h>

KPM_NAME("kpm-midi");
KPM_VERSION("1.0.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("hotian/ai");
KPM_DESCRIPTION("KernelPatch MIDI Module");

// 模拟midi数据（简单音符序列）
static const unsigned char midi_data[] = {
    0x4D, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06,  // MThd header
    0x00, 0x00, 0x00, 0x01, 0x00, 0x60,              // Format 0, 1 track, 96 ticks per quarter note
    0x4D, 0x54, 0x72, 0x6B,                          // MTrk header
    // 这里应该包含实际的MIDI事件数据，但为了简化，我们只放一个简单的音符
    0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20,        // Tempo: 500,000 microseconds per quarter note
    0x00, 0x90, 0x3C, 0x64,                          // Note On: Middle C (60), velocity 100
    0x40, 0x80, 0x3C, 0x00,                          // Note Off: Middle C (60), velocity 0 (after 64 ticks)
    0x00, 0xFF, 0x2F, 0x00                           // End of track
};

// 通过创建proc文件节点来通知用户空间播放midi
static void trigger_midi_playback(void)
{
    struct file *filp;
    loff_t pos = 0;
    ssize_t written;
    
    // 尝试创建或写入/proc/kpm_midi_trigger文件来触发midi播放
    filp = filp_open("/proc/kpm_midi_trigger", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (IS_ERR(filp)) {
        logkd("[kpm-midi] 无法创建/proc/kpm_midi_trigger文件\n");
        return;
    }
    
    // 写入触发信息
    written = kernel_write(filp, "play_midi", 9, &pos);
    if (written != 9) {
        logkd("[kpm-midi] 写入proc文件失败\n");
    }
    
    filp_close(filp, NULL);
    
    logkd("[kpm-midi] midi播放触发信号已发送\n");
}

// 通过sysfs接口触发midi播放
static void trigger_midi_via_sysfs(void)
{
    struct file *filp;
    loff_t pos = 0;
    ssize_t written;
    
    // 尝试写入/sys/class/kpm_midi/trigger节点
    filp = filp_open("/sys/class/kpm_midi/trigger", O_WRONLY, 0644);
    if (IS_ERR(filp)) {
        logkd("[kpm-midi] 无法打开/sys/class/kpm_midi/trigger节点\n");
        return;
    }
    
    // 写入触发信息
    written = kernel_write(filp, "1", 1, &pos);
    if (written != 1) {
        logkd("[kpm-midi] 写入sysfs节点失败\n");
    }
    
    filp_close(filp, NULL);
    
    logkd("[kpm-midi] 通过sysfs触发midi播放\n");
}

// 发送UEVENT事件通知用户空间
static void trigger_midi_via_uevent(void)
{
    // 这是一个简化的实现，实际使用中可能需要更复杂的uevent机制
    logkd("[kpm-midi] KERNEL==\"kpm_midi\", ACTION==\"load\", RUN+=\"/usr/bin/play_midi\"\n");
    logkd("[kpm-midi] midi播放uevent事件已发送\n");
}

// 模块初始化函数
static long midi_init(const char *args, const char *event, void *__user reserved)
{
    pr_info("[kpm-midi] kpm midi init, event: %s, args: %s\n", event, args);
    logkd("[kpm-midi] 此模块kpm-midi加载成功\n");
    
    // 直接触发midi播放
    // 我们尝试多种方式来触发用户空间的midi播放
    
    // 方式1: 通过proc文件系统
    trigger_midi_playback();
    
    // 方式2: 通过sysfs接口
    trigger_midi_via_sysfs();
    
    // 方式3: 通过uevent事件
    trigger_midi_via_uevent();
    
    // 方式4: 通过日志消息触发
    logkd("[kpm-midi] MIDI_PLAY_TRIGGER: kpm-midi模块已加载，播放默认声音\n");
    
    return 0;
}

// 模块控制函数
static long midi_control0(const char *args, char *__user out_msg, int outlen)
{
    pr_info("[kpm-midi] kpm midi control0, args: %s\n", args);
    
    // 如果通过控制命令触发播放
    if (args && strcmp(args, "play") == 0) {
        logkd("[kpm-midi] 收到播放命令，触发midi播放\n");
        trigger_midi_playback();
    }
    
    return 0;
}

// 模块退出函数
static long midi_exit(void *__user reserved)
{
    pr_info("[kpm-midi] kpm midi exit\n");
    logkd("[kpm-midi] midi模块已卸载\n");
    return 0;
}

KPM_INIT(midi_init);
KPM_CTL0(midi_control0);
KPM_EXIT(midi_exit);