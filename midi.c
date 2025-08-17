/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2025 Your Name. All Rights Reserved.
 */

#include <compiler.h>
#include <kpmodule.h>
#include <linux/printk.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

KPM_NAME("kpm-midi");
KPM_VERSION("1.0.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("Your Name");
KPM_DESCRIPTION("KernelPatch Module for MIDI sound on load");

// 定时器用于播放MIDI音符
static struct timer_list midi_timer;
static int note_count = 0;

// 简单的MIDI音符频率映射 (C大调)
static int notes[] = {262, 294, 330, 349, 392, 440, 494, 523}; // C4 to C5
static int note_durations[] = {500, 500, 500, 500, 500, 500, 500, 1000}; // 毫秒

// 模拟MIDI声音播放的函数
static void play_midi_note(int frequency, int duration_ms)
{
    pr_info("Playing MIDI note: frequency=%d Hz, duration=%d ms\n", frequency, duration_ms);
    // 在实际实现中，这里会调用硬件相关的API来播放声音
    // 例如通过ALSA或直接操作音频硬件寄存器
}

// 定时器回调函数，用于顺序播放音符
static void midi_timer_callback(struct timer_list *t)
{
    if (note_count < sizeof(notes) / sizeof(notes[0])) {
        play_midi_note(notes[note_count], note_durations[note_count]);
        note_count++;
        
        // 设置下一个音符的定时器
        mod_timer(&midi_timer, jiffies + msecs_to_jiffies(note_durations[note_count-1]));
    } else {
        pr_info("MIDI playback completed\n");
    }
}

// 模块初始化函数
static long midi_init(const char *args, const char *event, void *__user reserved)
{
    pr_info("kpm-midi init, event: %s, args: %s\n", event, args);
    
    // 初始化定时器
    timer_setup(&midi_timer, midi_timer_callback, 0);
    
    // 立即开始播放第一个音符
    note_count = 0;
    play_midi_note(notes[note_count], note_durations[note_count]);
    note_count++;
    
    // 设置下一个音符的定时器
    if (note_count < sizeof(notes) / sizeof(notes[0])) {
        mod_timer(&midi_timer, jiffies + msecs_to_jiffies(note_durations[0]));
    }
    
    return 0;
}

// 模块控制函数
static long midi_control(const char *args, char *__user out_msg, int outlen)
{
    pr_info("kpm-midi control, args: %s\n", args);
    return 0;
}

// 模块退出函数
static long midi_exit(void *__user reserved)
{
    pr_info("kpm-midi exit\n");
    
    // 删除定时器
    del_timer(&midi_timer);
    
    return 0;
}

// 模块入口点定义
KPM_INIT(midi_init);
KPM_CTL0(midi_control);
KPM_EXIT(midi_exit);