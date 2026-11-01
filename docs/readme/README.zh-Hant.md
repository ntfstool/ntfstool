# NTFSTool（開源版）

免費、開源的 macOS NTFS 讀寫工具，基於 Apple 的 **FSKit** 與 NTFSTool 自行開發的 NTFS 引擎（**libntfstool-driver**）。

插入 Windows（NTFS）磁碟後，macOS 會透過 NTFSTool 的 FSKit 驅動模組以可寫入方式掛載——無需核心
擴充功能、無需 macFUSE、無需降低安全性設定、無需管理員密碼。App 本身完全運行於沙盒之中。

[English](../../README.md) · [简体中文](../../README-CN.md) · **繁體中文** · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

> **找舊版？** 先前以 Electron/Fuse 為基礎的舊版本（2.x – 4.7.x）及其完整提交歷史已封存於 [`V3` 分支](https://github.com/ntfstool/ntfstool/tree/V3)，不再維護；目前的 `master` 是以 FSKit 為基礎的新版。

[架構指南](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="NTFSTool 主視窗：一個可讀寫的 NTFS 磁碟，附容量列與詳細資訊" width="760">
</p>

<p align="center"><i>如果這個專案對你有用或有趣，歡迎在 GitHub 上點個 ⭐，讓更多人發現它。</i></p>

## 螢幕截圖

以下是由本儲存庫建置的 App（`make app`）的真實截圖。畫面中的磁碟是內建示範模式（`make demo`）的
範例磁碟，因此即使沒有實體磁碟，你也能體驗每個畫面。

### 主視窗

一個視窗管理你所有的 NTFS 磁碟。每個磁碟區的狀態一目了然——**可讀寫**、**唯讀**或
**未掛載**——並附有容量列、掛載點、匯流排（USB / Thunderbolt）與裝置詳細資訊，以及*在
Finder 中顯示*與*推出*。

![主視窗：可讀寫的 USB 磁碟](../screenshots/overview.png)

| 可讀寫的 Thunderbolt SSD | 未掛載的磁碟 |
| --- | --- |
| ![Thunderbolt drive](../screenshots/thunderbolt-drive.png) | ![Not mounted drive](../screenshots/not-mounted.png) |

### 引導式設定：無需密碼，無需核心擴充功能

NTFSTool 是一個標準的 FSKit 檔案系統模組。macOS 唯一要求的，是在
**系統設定 → 一般 → 登入項目與延伸功能 → 檔案系統延伸功能** 中打開一個開關。App 會逐步
說明操作方式，並自動偵測到設定的變更。

![開啟檔案系統延伸功能](../screenshots/enable-extension.png)

### 預設即安全

如果 Windows 處於休眠狀態或開啟了*快速啟動*，寫入會損毀磁碟。NTFSTool 會以
**唯讀**方式掛載這類磁碟，並明確告訴你如何修復，而不是拿你的資料冒險。

| 被 Windows 休眠而遺留的磁碟 | App 給你的提示 |
| --- | --- |
| ![Hibernated drive](../screenshots/hibernated-windows.png) | ![Shut Down Windows Completely](../screenshots/dialog-shut-down-windows.png) |

### 重新插入磁碟

macOS 只允許系統在實體磁碟插入時進行掛載，因此 App 會安全地推出磁碟並
請你重新插入；之後便會以可讀寫方式掛載。

![重新插入磁碟](../screenshots/dialog-plug-in-again.png)

### 深色模式

| 主視窗 | 延伸功能引導 |
| --- | --- |
| ![Dark mode, main window](../screenshots/dark-overview.png) | ![Dark mode, extension guide](../screenshots/dark-enable-extension.png) |

### 精簡版面、空狀態與「關於」

| 隱藏側邊欄 | 等待磁碟 |
| --- | --- |
| ![Sidebar hidden](../screenshots/sidebar-hidden.png) | ![No drive connected](../screenshots/no-drive.png) |

![關於 NTFSTool](../screenshots/about.png)

介面支援 20 種語言，包含阿拉伯文與波斯文的由右至左版面：English、
简体中文、繁體中文、日本語、한국어、Deutsch、Français、Español、Italiano、Português、Русский、العربية、فارسی、
Türkçe、हिन्दी、বাংলা、Bahasa Indonesia、Kiswahili、ไทย 與 Tiếng Việt。

## 本儲存庫是什麼

這是 NTFSTool 的**開源學習／參考版本**。它源自 Mac App Store 上的免費版 NTFSTool，
沿用相同的架構，但刻意保持精簡易讀：

- 每個原始碼檔案都有英文註解，方便你研究 FSKit 檔案系統模組、建構於
  NTFS 引擎之上的 C 橋接層，以及沙盒化的 SwiftUI/AppKit 前端是如何協同運作的；
- 驅動使用簡單、直接的 I/O 路徑（沒有快取層，也沒有針對特定平台的效能調校）；
- App 只提供基本功能（沒有遙測、沒有診斷資料上傳，完全沒有網路存取）；
- 沒有授權、啟用或試用相關的程式碼。

如果你只是想以使用者身分獲得最佳體驗，請從 [ntfstool.com](https://ntfstool.com) 安裝官方版本。
NTFSTool Pro（閉源）另外提供適用於較舊 macOS 版本的第二套驅動、磁碟修復與其他工具。

## 功能

- 讀取**並寫入** NTFS 磁碟區；macOS 會透過 FSKit 驅動模組自動掛載
- App 運行於沙盒中；驅動以沙盒化 FSKit 模組的形式在使用者空間運行
- 顯示每個 NTFS 磁碟區的狀態：可讀寫、唯讀（Windows 休眠／快速啟動）、
  被 Apple 內建驅動設為唯讀、未掛載
- 引導你在系統設定中開啟檔案系統延伸功能
- 在 Finder 中開啟、推出、磁碟資訊
- 20 種介面語言
- 延伸屬性以 NTFS 具名資料流儲存（不會產生 `._` 檔案）；可選的 `windows_names` 掛載選項
  會拒絕 Windows 無法使用的檔名

## 系統需求

- 執行需要 macOS 15.6 或更新版本；**寫入支援需要 macOS 26 或更新版本**（在 macOS 15 上，系統對實體磁碟
  仍使用 Apple 內建的唯讀 NTFS 驅動）
- 建置需要：Xcode 16.3 或更新版本（含 FSKit SDK）及其命令列工具，Swift 5.9+

## 建置——一個指令

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

其他目標：

| 指令 | 作用 |
| --- | --- |
| `make run` | 建置並開啟 App |
| `make demo` | 以記憶體內的示範磁碟執行介面——無需驅動，也無需 Apple Developer 帳號 |
| `make test` | 執行單元測試 |
| `make app UNIVERSAL=1` | 建置 arm64 + x86_64 |
| `make scan` | 檢查樹中是否含有簽署資料或私人識別碼 |

驅動測試會使用 `ntfstool-driver` 二進位檔的 `mkntfs` 與 `ntfsfix` 子指令，來格式化並檢查真實的 NTFS 磁碟映像。除非在 Homebrew 的 `bin`/`sbin` 或由 `NTFSTOOL_NTFS_TOOLS=<dir>` 指定的目錄中找到這兩個工具，否則這些測試會被略過。

## 執行驅動（需要你自己的 Apple Developer 帳號）

FSKit 模組使用受限權利 `com.apple.developer.fskit.fsmodule`。macOS 只會載入簽章由授予該權利的
佈建描述檔所支持的模組。Ad-hoc 簽署的建置版本可以顯示完整介面，但系統不會載入驅動。
這是 Apple 平台的規則，適用於每一個 FSKit 專案。

1. 在你的 Apple Developer 帳號中建立兩個 App ID，例如 `com.yourname.ntfs` 與 `com.yourname.ntfs.fskit`。
   在第二個 App ID 上啟用 **FSKit Module** 功能，並可選擇在兩者上都啟用 App Group
   （例如 `TEAMID.com.yourname.ntfs`），讓 App 能讀取驅動的日誌。
2. 為兩者建立開發（或 Developer ID）佈建描述檔並下載。
3. 使用你的身分進行建置：

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. 將 `build/NTFSTool.app` 複製到 `/Applications`，開啟一次，然後在
   **系統設定 → 一般 → 登入項目與延伸功能 → 檔案系統延伸功能** 中開啟驅動。
5. 重新插入 NTFS 磁碟。此時 `mount` 應會以類型 `ntfstooloss` 列出它。

所有建置選項都記錄在 [`scripts/build-app.sh`](../../scripts/build-app.sh) 的開頭。
佈建描述檔與簽署金鑰絕不應放入本儲存庫；`make scan` 會檢查這一點。

## 目錄結構

| 路徑 | 說明 |
| --- | --- |
| `Sources/NTFSTool` | App 進入點（`main.swift`） |
| `Sources/NTFSToolApp` | AppKit 外殼、SwiftUI 檢視、App 模型、系統服務、20 種在地化 |
| `Sources/NTFSToolCore` | 不依賴 AppKit 的純邏輯：磁碟模型、語言清單、設定儲存、本機事件日誌 |
| `Sources/NTFSToolBridge` | C 橋接層（`ntfstool_bridge.c`），在 `libntfstool-driver` 之上提供以 inode 為基礎的操作 |
| `Sources/NTFSToolDriver` | 驅動的 Swift 核心（不依賴 FSKit，可在任何 macOS 上進行單元測試） |
| `FSKitModule/NTFSToolFS` | macOS 載入以掛載 NTFS 磁碟區的 FSKit 模組（`NTFSToolFS.appex`） |
| `Tests/` | App 模型與驅動的單元測試（使用真實 NTFS 映像） |
| `scripts/` | `build-app.sh`、`build-fskit.sh`、`scan-secrets.sh` |

關於一次寫入如何從 Finder 抵達磁碟，請參閱 [docs/ARCHITECTURE.md](../ARCHITECTURE.md)。

## 參與貢獻

歡迎提出 Issue 與 Pull Request。請為你的提交簽署（`git commit -s`），以證明遵循
[Developer Certificate of Origin](https://developercertificate.org/)——詳見 [CONTRIBUTING.md](../../CONTRIBUTING.md)。

## 授權條款

MIT License——詳見 [LICENSE](../../LICENSE) 與 [NOTICE](../../NOTICE)。

## 商標

**請不要把本專案或其衍生版本上架 Mac App Store**——會與官方免費版競爭並造成使用者混淆。
