# NTFS Tool

<a href="https://ntfstool.com">
  <img src="https://store.alfw.com/ntfstool/ntfs-logo%402x.png" /></a>

[English](./README.md) | 简体中文

## 为苹果电脑提供NTFS读写支持的一款免费软件
## NTFS Tool Free For Mac

​       我们是一群热爱生活热爱编程的软件技术从业者，利用零散时间，开发出这款免费的 NTFS Tool for Mac 工具。

​       NTFS Tool 是一款纯净版的NTFS 工具，支持NTFS磁盘读写、挂载，推出、管理等功能。它的界面简洁易用，希望这款工具能够为你的工作和生活带来便利👻。

如果这款软件对你有帮助，欢迎 Star 关注。

✈️ 去 [官网](https://ntfstool.com) 逛逛  |  📖 查看 [帮助手册](http://docs.ntfstool.com)

## 🧰 NTFS Tool 团队的更多免费 Mac 工具

- 📸 **[wxShot](https://github.com/ntfstool/wxShot)**：微信截图替代品，不登录微信也能用的 macOS 截图工具（默认 ⌥A，长截图、离线提取文字，MIT 开源）。⭐ 如果对你有帮助，欢迎到 [ntfstool/wxShot](https://github.com/ntfstool/wxShot) 点个 Star。
- 💽 **[NTFS2exFAT](https://github.com/ntfstool/ntfs2exfat)**：在 Mac 上把 NTFS 硬盘安全转换为 exFAT，带校验的拷贝。

## 💽 安装稳定版

[GitHub](https://github.com/ntfstool/ntfstool/releases) 和 [官网](https://ntfstool.com) 提供了已经编译好的稳定版安装包，当然你也可以自己克隆代码编译打包。

### 平台支持

作者初衷是为MacOS系统操作NTFS磁盘提供方便，目前仅支持MacOS系统。

### macOS

macOS 用户可以点击下方链接跳转到官网直接下载。

[点击下载](https://ntfstool.com) 

## ✨ 特性

- 🕹 简洁明了的图形操作界面
- 🦄 支持USB挂载NTFS磁盘读写操作
- ☑️ 支持查看可读写磁盘容量
- 💾 支持磁盘信息自主刷新
- 🎛 支持镜像磁盘文件空间占用提示
- 🚀 支持镜像磁盘文件卸载
- 🔔 操作完成后通知
- 💻 支持触控栏快捷键
- 🤖 常驻系统托盘，操作更加便捷【TODO】
- 🌑 深色模式【TODO】
- 🌍 国际化，[查看已可选的语言](#-国际化)【待完善】
- 🎏 ...

## 🖥 应用界面

![ntfstool-main1.png](https://store.alfw.com/ntfstool/ntfs-display.jpg)

![ntfstool-main1.png](https://store.alfw.com/ntfstool/ntfstool-main2.png)

## ⌨️ 本地开发

### 克隆代码

```bash
git clone git@github.com:ntfstool/ntfstool.git
```

### 安装 & 编译

大陆用户建议使用淘宝的 npm 源

```bash
npm install nrm -g
nrm use taobao
nrm test npm  测试速度
```

设置 electron 源

```bash
npm config set electron_mirror https://cdn.npm.taobao.org/dist/electron/ 
```

安装依赖

```bash
cd ntfstool
npm install
```

大陆用户建议使用淘宝的 npm 源

```bash
npm install nrm -g
nrm use taobao
nrm test npm  测试速度
```

如果喜欢 [Yarn](https://yarnpkg.com/)，也可以使用 `yarn` 安装依赖

### 开发模式

```bash
npm run dev
```

### 编译打包

```bash
npm run build
```

完成之后可以在项目的 `release` 目录看到编译打包好的应用文件

## 🛠 技术栈

- [Electron](https://electronjs.org/)
- [Vue](https://vuejs.org/) + [VueX](https://vuex.vuejs.org/) + [Element](https://element.eleme.io)

## ☑️ TODO

...

## 🤝 参与共建 [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat)](http://makeapullrequest.com)

如果你有兴趣参与共同开发，欢迎 FORK 和 PR。

## 🌍 国际化

欢迎大家将 NtfsTool 翻译成更多的语言版本 🧐

| Key   | Name                | Status |
| ----- | :------------------ | :----- |
| ca    | Català              | 🚧      |
| de    | Deutsch             | 🚧      |
| en-US | English             | ✔️      |
| fa    | فارسی               | 🚧      |
| fr    | Français            | 🚧      |
| ja    | 日本語              | ✔️      |
| ko    | 한국어              | ✔️      |
| pt-BR | Portuguese (Brazil) | 🚧      |
| ru    | Русский             | 🚧      |
| tr    | Türkçe              | 🚧      |
| uk    | Українська          | 🚧      |
| zh-CN | 简体中文            | ✔️      |
| zh-TW | 繁體中文            | ✔️      |

## 📜 开源许可

基于 [MIT license](https://opensource.org/licenses/MIT) 许可进行开源。