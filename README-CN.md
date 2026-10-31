# NTFSTool 开源版

**NTFSTool Mac App Store 免费版的开源版本。** 在 Mac 上读写 Windows（NTFS）硬盘，基于苹果 **FSKit** 文件系统框架和 NTFSTool 自有的 NTFS 引擎（**libntfstool-driver**），
不装内核扩展、不需要 macFUSE、不降低系统安全设置、不要管理员密码。

本仓库面向**自用、研究和学习**：你可以自己编译使用，也可以把它当作 macOS 文件系统开发的完整参考工程来读。

[架构说明](docs/ARCHITECTURE.md) · [官网 ntfstool.com](https://ntfstool.com/?utm_source=github_opensource) · [NTFSTool Pro](#ntfstool-pro专业版)

[English](README.md) · **简体中文** · [繁體中文](docs/readme/README.zh-Hant.md) · [日本語](docs/readme/README.ja.md) · [한국어](docs/readme/README.ko.md) · [Deutsch](docs/readme/README.de.md) · [Français](docs/readme/README.fr.md) · [Español](docs/readme/README.es.md) · [Italiano](docs/readme/README.it.md) · [Português](docs/readme/README.pt.md) · [Русский](docs/readme/README.ru.md) · [العربية](docs/readme/README.ar.md) · [فارسی](docs/readme/README.fa.md) · [Türkçe](docs/readme/README.tr.md) · [हिन्दी](docs/readme/README.hi.md) · [বাংলা](docs/readme/README.bn.md) · [Bahasa Indonesia](docs/readme/README.id.md) · [Kiswahili](docs/readme/README.sw.md) · [ไทย](docs/readme/README.th.md) · [Tiếng Việt](docs/readme/README.vi.md)

<p align="center">
  <img src="docs/screenshots/overview.png" alt="NTFSTool 主界面：一个可读写的 NTFS 硬盘，含容量条和详细信息" width="760">
</p>

<p align="center"><i>如果这个项目对你有用或有启发，欢迎在 GitHub 上点个 ⭐，让更多人看到。</i></p>

## 界面截图

以下是用本仓库源码编译（`make app`）出来的真实界面截图。硬盘是内置演示模式（`make demo`）里的示例硬盘，没有物理硬盘也能体验每个界面。界面截图统一使用英文界面，应用本身支持 20 种语言。

### 主界面

所有 NTFS 硬盘集中在一个窗口里：**可读写**、**只读**、**未挂载**一目了然，附带容量条、挂载点、接口（USB / Thunderbolt）和设备信息，并可「在访达中显示」或「推出」。

![主界面：可读写的 USB 硬盘](docs/screenshots/overview.png)

| 可读写的 Thunderbolt 固态硬盘 | 未挂载的硬盘 |
| --- | --- |
| ![Thunderbolt 硬盘](docs/screenshots/thunderbolt-drive.png) | ![未挂载的硬盘](docs/screenshots/not-mounted.png) |

### 引导式设置：无需密码，无需内核扩展

NTFSTool 是标准的 FSKit 文件系统模块。macOS 只需要你在 **系统设置 → 通用 → 登录项与扩展 → 文件系统扩展** 里打开一个开关。应用会一步步引导，并自动检测到你已开启。

![打开文件系统扩展](docs/screenshots/enable-extension.png)

### 默认保证数据安全

如果 Windows 处于休眠或开启了「快速启动」，直接写入会损坏磁盘。NTFSTool 会以**只读**方式挂载这类硬盘，并明确告诉你如何处理，而不是冒数据风险。

| Windows 休眠导致只读 | 应用给出的提示 |
| --- | --- |
| ![休眠的硬盘](docs/screenshots/hibernated-windows.png) | ![请完全关闭 Windows](docs/screenshots/dialog-shut-down-windows.png) |

### 重新插拔硬盘

macOS 只允许系统在硬盘插入时挂载物理硬盘，所以应用会先安全推出，再请你重新插入，之后即以读写方式挂载。

![请重新插入硬盘](docs/screenshots/dialog-plug-in-again.png)

### 深色模式

| 主界面 | 扩展引导 |
| --- | --- |
| ![深色模式主界面](docs/screenshots/dark-overview.png) | ![深色模式扩展引导](docs/screenshots/dark-enable-extension.png) |

### 精简布局、空状态与关于

| 隐藏边栏 | 等待插入硬盘 |
| --- | --- |
| ![隐藏边栏](docs/screenshots/sidebar-hidden.png) | ![未连接硬盘](docs/screenshots/no-drive.png) |

![关于 NTFSTool](docs/screenshots/about.png)

## 项目简介

NTFSTool 自 2020 年开始开发，开源项目在 GitHub 上已有 4,000+ Star。从 5.0 起，NTFSTool 完全
重写为苹果新一代的 FSKit 架构：

- **App** 是运行在沙盒里的 SwiftUI / AppKit 程序，负责列出 NTFS 磁盘、显示状态、引导开启驱动、在访达中打开和推出；
- **驱动** 是一个 FSKit 文件系统模块（`.appex`），由 macOS 在插盘时自动加载并挂载，运行在用户态沙盒里；
- **NTFS 引擎** 是经过 C 桥接层封装的 libntfstool-driver，以 MFT 记录号为单位读写文件。

本仓库的代码就是 Mac App Store 上 NTFSTool 免费版的基础功能：同样的 App、同样的驱动架构、同样的 NTFS 引擎，所有源文件都附有完整的
英文注释。

## 版本选择

NTFSTool 有三个版本，面向不同的使用场景：

| | 开源版（本仓库） | App Store 免费版 | **NTFSTool Pro** |
| --- | --- | --- | --- |
| 定位 | 自用、研究、学习 | 日常免费使用 | 专业、省心、全兼容 |
| 获取方式 | 自行编译 | [Mac App Store](https://apps.apple.com/app/id1510209857) | [ntfstool.com](https://ntfstool.com/?utm_source=github_opensource) |
| 读写 NTFS | ✓ | ✓ | ✓ |
| 插盘自动挂载 | ✓ | ✓ | ✓ |
| 安装即用 | 需自备苹果开发者账号签名驱动 | ✓（苹果审核签名） | ✓（苹果公证签名） |
| 支持系统 | macOS 15.6 及以上 | macOS 15.6 及以上 | macOS 12 及以上（10.15–11 自动提供经典版） |
| 首次设置 | 在系统设置里打开文件系统扩展 | 在系统设置里打开文件系统扩展 | 无需设置 |
| 读写驱动 | FSKit 驱动 | FSKit 驱动 | 双驱动：驱动 1 默认，驱动 2（FSKit）可选，失败自动切换 |
| 读写性能优化 | — | ✓ | ✓（缓存与读写调优） |
| Windows 休眠 / 未正常关机的磁盘 | 识别并说明 | 识别并说明 | 识别并一键修复 |
| 抹掉为 NTFS | — | — | ✓ |
| S.M.A.R.T. 健康检查、测速 | — | — | ✓ |
| 菜单栏控制、登录时静默启动、按盘设置 | — | — | ✓ |
| 自动更新 | — | 通过 App Store | ✓ |
| 人工邮件支持 | GitHub Issues | — | 48 小时内回复 |
| 价格 | 免费 | 免费 | 一次买断，终身更新 |

## NTFSTool Pro（专业版）

**开源版和免费版回答的是「我的 Mac 能不能写这块盘」；Pro 解决的是「以后再也不用操心这件事」。**

如果你每天都要在 Mac 和 Windows 之间交换文件、管理移动硬盘，或者 Mac 系统版本较老，推荐使用 NTFSTool Pro：

- **更广的兼容**：macOS 12 起即可使用，更老的 Mac（macOS 10.15–11）自动获得经典版；Intel 与 Apple 芯片通用。
- **装好就能用**：不需要到系统设置里找扩展开关，插盘即可读写。
- **双驱动，更稳**：默认驱动覆盖所有支持的系统，FSKit 驱动可选；一个挂不上时自动换另一个。
- **磁盘工具**：Windows 留下的脏盘、休眠盘一键修复；抹掉为 NTFS；S.M.A.R.T. 健康；内置测速；USB 2.0 慢速接口提醒。
- **一直在身边**：菜单栏控制、登录时静默启动、每块盘单独设置。
- **安全**：同样不装内核扩展、不进恢复模式、不降低安全设置，Apple Pay 照常可用。
- **买得放心**：先在自己的盘上免费试用 3 次挂载；一次买断、终身更新（含未来的 macOS 大版本）；一个授权覆盖家里所有 Mac；
  45 天读写保障，读写不了且我们修不好，全额退款；真人邮件 48 小时内回复。

👉 了解和下载：[ntfstool.com](https://ntfstool.com/?utm_source=github_opensource) · 免费版与 Pro 对比：[ntfstool.com/free](https://ntfstool.com/free/?utm_source=github_opensource)

开源版 App 里也会在合适的地方（例如磁盘因 Windows 休眠只能只读时）给出 Pro 的链接；它只在你点击时打开浏览器，App 本身不联网。

## 功能

- 读**写** NTFS 卷，插盘后由 macOS 通过 FSKit 驱动自动挂载
- 实时显示每个 NTFS 卷的状态：可读写、只读（Windows 休眠 / 快速启动）、被苹果自带驱动只读挂载、未挂载
- 驱动未开启时，图文引导你到「系统设置」打开文件系统扩展
- 在访达中打开、推出、查看磁盘信息（容量、可用空间、挂载点、是否可写等）
- 插入 NTFS 磁盘时发送系统通知
- 20 种界面语言，阿拉伯语、波斯语自动从右到左排版
- 扩展属性存为 NTFS 命名数据流，不在盘上留下 `._` 文件
- 可选挂载参数 `windows_names`，拒绝 Windows 无法识别的文件名，保证拿回 Windows 后能正常打开

## 数据安全

NTFS 写入出错会直接损坏用户数据，驱动层遵循常见 FUSE NTFS 驱动的规则：

- **同一条 MFT 记录同一时刻只打开一份**：每个操作打开 inode、处理、关闭，避免两份内存副本互相覆盖目录索引；
- **重命名 = 建链接 + 删旧名**，只改大小写时经过临时名中转；禁止把目录移动到它自己的子目录里；
- **Windows 休眠或开启了「快速启动」的盘自动只读挂载**，不会写坏 Windows 留下的休眠状态；
- 底层 I/O 按块对齐、直写到磁盘；
- 驱动核心有基于真实 NTFS 磁盘映像的单元测试，每次读写后用 `ntfsfix` 检查文件系统一致性。

## 工作原理

```
 访达 / 任意 App
        │  POSIX 文件调用（open、write、rename …）
        ▼
 macOS VFS ──► fskitd ──► NTFSToolFS.appex        FSKit 模块（FSKitModule/NTFSToolFS）
                              ▼
                        NTFSVolume（Swift）        驱动核心（Sources/NTFSToolDriver）
                              ▼
                        ntfstool_bridge.c          C 桥接层（Sources/NTFSToolBridge）
                              ▼
                        libntfstool-driver         NTFS 引擎（Vendor/NTFSToolNTFS.xcframework，预编译）
                              ▼
                        FSKit 交给驱动的原始磁盘设备

 NTFSTool.app（沙盒）：通过 DiskArbitration 列出磁盘、显示状态、引导开启驱动。
 它从不直接碰磁盘——挂载由 macOS 自己完成。
```

完整的分层说明、状态判定规则和演示模式见 [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)。

## 运行环境

- **运行**：macOS 15.6 及以上；**写入需要 macOS 26 及以上**（macOS 15 上系统对实体盘仍使用苹果自带的只读 NTFS 驱动）
- **编译**：Xcode 16.3 及以上（含 FSKit SDK）和命令行工具，Swift 5.9+
- Apple 芯片与 Intel 均可编译

## 一键编译

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app（ad-hoc 签名，本机架构）
```

| 命令 | 作用 |
| --- | --- |
| `make run` | 编译并打开 App |
| `make demo` | 用内存里的演示磁盘运行界面，不需要驱动，也不需要苹果开发者账号 |
| `make test` | 运行全部单元测试 |
| `make app UNIVERSAL=1` | 编译 arm64 + x86_64 通用版 |
| `make scan` | 检查仓库里没有签名材料或私有标识 |
| `make clean` | 清理编译产物 |

驱动测试会用 `ntfstool-driver` 二进制的 `mkntfs`、`ntfsfix` 子命令 格式化并检查真实的 NTFS 磁盘映像；找不到这两个工具（Homebrew 的
`bin`/`sbin`，或 `NTFSTOOL_NTFS_TOOLS=<目录>`）时自动跳过。

## 让驱动真正工作（需要你自己的苹果开发者账号）

FSKit 模块需要受限权限 `com.apple.developer.fskit.fsmodule`，macOS 只加载签名带有对应描述文件的模块。ad-hoc
编译出来的 App 界面完整可用，但系统不会加载驱动。这是苹果平台的规定，所有 FSKit 项目都一样。
不想折腾签名的话，可以直接使用 [App Store 免费版](https://apps.apple.com/app/id1510209857) 或 [NTFSTool Pro](https://ntfstool.com/?utm_source=github_opensource)。

1. 在苹果开发者账号里建两个 App ID，例如 `com.yourname.ntfs` 和 `com.yourname.ntfs.fskit`，给第二个打开
   **FSKit Module** 能力；想让 App 读到驱动日志，再给两个都加同一个 App Group（如 `TEAMID.com.yourname.ntfs`）。
2. 为两个 App ID 创建开发（或 Developer ID）描述文件并下载。
3. 用你的身份编译：

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. 把 `build/NTFSTool.app` 拷到「应用程序」，打开一次，然后在
   **系统设置 → 通用 → 登录项与扩展 → 文件系统扩展** 里打开驱动。
5. 重新插拔 NTFS 硬盘，`mount` 里应能看到类型为 `ntfstooloss` 的卷。

全部编译选项见 [`scripts/build-app.sh`](scripts/build-app.sh) 开头的说明。描述文件和签名私钥绝不能提交进仓库，
`make scan` 会检查。

## 目录结构

| 路径 | 内容 |
| --- | --- |
| `Sources/NTFSTool` | App 入口（`main.swift`） |
| `Sources/NTFSToolApp` | AppKit 外壳、SwiftUI 界面、App 模型、系统服务、20 种语言 |
| `Sources/NTFSToolCore` | 不依赖 AppKit 的纯逻辑：磁盘模型、语言列表、驱动名、本地事件日志 |
| `Sources/NTFSToolBridge` | C 桥接层（`ntfstool_bridge.c`），在 libntfstool-driver 之上提供按 inode 号操作的接口 |
| `Sources/NTFSToolDriver` | 驱动的 Swift 核心（不依赖 FSKit，任何 macOS 上都能单元测试） |
| `Vendor/NTFSToolNTFS.xcframework` | 预编译的 NTFS 引擎（libntfstool-driver 静态库，arm64 + x86_64，含头文件），说明见 [Vendor/README.md](Vendor/README.md) |
| `FSKitModule/NTFSToolFS` | macOS 加载的 FSKit 模块（`NTFSToolFS.appex`） |
| `Tests/` | App 模型测试、驱动测试（真实 NTFS 磁盘映像） |
| `scripts/` | `build-app.sh`、`build-fskit.sh`、`scan-secrets.sh` |

## 适合用来学习什么

- 如何用 FSKit 写一个完整的读写文件系统模块（probe、挂载、lookup、读写、重命名、扩展属性）
- 如何把为 FUSE 设计的 C 库（libntfstool-driver）桥接到按条目（item）工作的 FSKit
- 沙盒 App 如何通过 DiskArbitration 监听磁盘、判断挂载状态，并和系统扩展配合
- SwiftUI 与 AppKit 混合的 Mac 原生界面，运行时切换 20 种语言（含从右到左）
- 用演示模式（`NTFSTOOL_DEMO=1`）在没有磁盘和驱动的情况下开发、截图和测试界面

## 隐私

开源版不发起任何网络请求，没有遥测、没有诊断上传、没有授权验证。官网、GitHub、Pro 等链接只在你点击时用浏览器打开。
App 的权限只有 App Sandbox。

## 参与贡献

欢迎提交 Issue 和 Pull Request。提交时请加上 `git commit -s` 签署
[开发者原创声明（DCO）](https://developercertificate.org/)，详见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 使用范围与许可证

- **用途**：本仓库供个人自用、研究和学习。
- **商业使用须授权**：以 NTFSTool 的名称、图标或品牌进行任何商业使用（销售、上架应用商店、预装或捆绑分发、
  基于它提供付费服务等），须事先获得 NTFSTool 的书面授权。授权联系：service@ntfstool.com。
- **代码许可证**：MIT 许可证，见 [LICENSE](LICENSE) 和 [NOTICE](NOTICE)。内置的 NTFS 引擎是 NTFSTool 自有的
  libntfstool-driver，以预编译静态库形式提供，见 [Vendor/README.md](Vendor/README.md)。
- 本仓库早期的 Electron 版本（MIT 许可）保存在 `archive/electron-master` 分支。

## 商标

「NTFSTool」名称和图标是 NTFSTool 的商标，不在 MIT 授权范围内。

- 你可以自己编译、使用本仓库的代码，并注明「基于 NTFSTool」。
- **分发**修改版或分支（包括上架任何应用商店）时，必须换用不同的名称、图标和 bundle id，不能看起来像或自称官方 NTFSTool。
- 未经许可，不得以 NTFSTool 名义再分发本仓库的未修改版本。

NTFS 和 Windows 是微软公司的商标；macOS 和 FSKit 是苹果公司的商标。本项目与微软、苹果无关。

## 联系我们

- 官网：[ntfstool.com](https://ntfstool.com/?utm_source=github_opensource)
- 邮箱：service@ntfstool.com
- 问题反馈：[GitHub Issues](https://github.com/ntfstool/ntfstool/issues)
