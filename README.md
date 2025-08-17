# KPM Phone Monitor

这是一个基于 `SukiSU_KernelPatch_patch` 的内核模块，用于监控手机信息并通过网页展示。

## 功能

- 在内核空间收集手机信息
- 通过 `/proc/phone_monitor` 文件直接提供HTML格式的信息
- 支持动态加载和卸载内核模块
- 加载后可直接通过浏览器查看手机信息，无需额外的用户空间程序

## 项目结构

```
kpm-phone-monitor/
├── phone-monitor.c          # 内核模块源代码
├── Makefile                 # 编译配置文件
├── .gitignore               # Git忽略文件
├── README.md                # 本说明文件
├── .github/
│   └── workflows/
│       └── build.yml        # GitHub Actions构建配置
└── user/
    └── phone_monitor_server.py  # 用户空间网页服务器 (可选)
```

## 编译内核模块

### 环境要求

- ARM64 Linux 系统
- `SukiSU_KernelPatch_patch` 源代码
- 交叉编译工具链

### 编译步骤

1. 克隆 `SukiSU_KernelPatch_patch` 仓库：
   ```bash
   git clone https://github.com/Sukikebi/SukiSU_KernelPatch_patch.git
   ```

2. 设置环境变量：
   ```bash
   export TARGET_COMPILE=aarch64-linux-gnu-  # 或其他交叉编译工具链前缀
   export KP_DIR=/path/to/SukiSU_KernelPatch_patch
   ```

3. 编译内核模块：
   ```bash
   make
   ```

4. 生成的模块文件为 `phone-monitor.kpm`

## 使用方法

### 在 Android 设备上使用

1. 确保设备已 root 并安装了支持 `SukiSU_KernelPatch_patch` 的内核

2. 将 `phone-monitor.kpm` 传输到设备

3. 使用 `kptools` 加载模块：
   ```bash
   ./kptools load phone-monitor.kpm
   ```

4. 直接在浏览器中访问以下地址查看手机信息：
   ```
   http://localhost/proc/phone_monitor
   ```
   
   或者如果设备有公网IP，可以通过以下地址访问：
   ```
   http://设备IP/proc/phone_monitor
   ```

5. 要卸载模块，使用以下命令：
   ```bash
   ./kptools unload phone-monitor.kpm
   ```

### 网页显示的信息

加载模块后，网页将显示以下信息：
- 当前时间戳
- 当前进程信息（PID、进程名、父进程PID）
- 用户信息（UID、GID、EUID、EGID）
- 内核版本信息
- 刷新按钮

## 内核模块功能

内核模块会收集以下信息：
- 当前进程信息（PID、进程名、父进程）
- 用户和组信息（UID、GID、EUID、EGID）
- 内核版本信息
- 时间戳

可以通过扩展模块代码来收集更多系统信息，如：
- 进程列表
- 内存使用情况
- 网络连接信息
- 电池状态
- 存储空间使用情况

## 技术实现

内核模块通过以下方式实现Web访问：
1. 创建 `/proc/phone_monitor` 文件节点
2. 当该文件被读取时，动态收集并返回HTML格式的手机信息
3. 通过浏览器直接访问 `/proc/phone_monitor` 即可查看信息

## 安全注意事项

- 此模块需要 root 权限才能加载
- 仅在开发和测试环境中使用
- 不要在生产环境中使用此模块
- 使用后请及时卸载模块

## 许可证

本项目基于 GPL v2 许可证发布。