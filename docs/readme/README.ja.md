# NTFSTool（オープンソース版）

Apple の **FSKit** と NTFSTool 独自の NTFS エンジン（**libntfstool-driver**）の上に作られた、macOS 向けの無料・オープンソースの NTFS 読み書きツールです。

Windows（NTFS）のドライブを接続すると、macOS は NTFSTool の FSKit ドライバモジュールを通じて書き込み可能な状態でマウントします。
カーネル拡張も macFUSE も、セキュリティ設定の引き下げも、管理者パスワードも不要です。アプリ自体は完全にサンドボックス化されています。

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · **日本語** · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

[アーキテクチャガイド](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="NTFSTool のメインウィンドウ：容量バーと詳細を表示した読み書き可能な NTFS ドライブ" width="760">
</p>

<p align="center"><i>このプロジェクトが役に立った、または面白いと感じたら、GitHub で ⭐ を付けていただけると、より多くの人に見つけてもらえます。</i></p>

## スクリーンショット

このリポジトリからビルドしたアプリ（`make app`）の実際のスクリーンショットです。表示されているドライブは、
内蔵のデモモード（`make demo`）のサンプルドライブなので、物理ディスクがなくてもすべての画面を試せます。

### メインウィンドウ

すべての NTFS ドライブを 1 つのウィンドウで管理できます。各ボリュームの状態――**読み書き可能**、**読み取り専用**、
**未マウント**――が容量バー、マウントポイント、バス（USB / Thunderbolt）、デバイスの詳細とともに一目でわかり、
*Finder で表示*と*取り出し*も行えます。

![メインウィンドウ：読み書き可能な USB ドライブ](../screenshots/overview.png)

| 読み書き可能な Thunderbolt SSD | マウントされていないドライブ |
| --- | --- |
| ![Thunderbolt drive](../screenshots/thunderbolt-drive.png) | ![Not mounted drive](../screenshots/not-mounted.png) |

### ガイド付きセットアップ：パスワードもカーネル拡張も不要

NTFSTool は通常の FSKit ファイルシステムモジュールです。macOS が求めるのは、
**システム設定 → 一般 → ログイン項目と拡張機能 → ファイルシステム拡張機能** にあるスイッチを 1 つオンにすることだけです。
アプリが手順をひとつずつ説明し、変更も自動で検出します。

![ファイルシステム拡張機能をオンにする](../screenshots/enable-extension.png)

### デフォルトで安全

Windows がハイバネート状態だったり*高速スタートアップ*が有効だったりすると、書き込みによってディスクが破損します。
NTFSTool はそのようなドライブを**読み取り専用**でマウントし、データを危険にさらす代わりに、対処方法を具体的にお伝えします。

| Windows によってハイバネートされたままのドライブ | アプリからのメッセージ |
| --- | --- |
| ![Hibernated drive](../screenshots/hibernated-windows.png) | ![Shut Down Windows Completely](../screenshots/dialog-shut-down-windows.png) |

### ドライブを接続し直す

macOS は物理ドライブを接続した時にしかシステムによるマウントを許可しないため、アプリはドライブを安全に取り出し、
接続し直すよう案内します。その後は読み書き可能な状態でマウントされます。

![ドライブを接続し直す](../screenshots/dialog-plug-in-again.png)

### ダークモード

| メインウィンドウ | 拡張機能ガイド |
| --- | --- |
| ![Dark mode, main window](../screenshots/dark-overview.png) | ![Dark mode, extension guide](../screenshots/dark-enable-extension.png) |

### コンパクトなレイアウト、空の状態、About

| サイドバーを非表示 | ドライブを待機中 |
| --- | --- |
| ![Sidebar hidden](../screenshots/sidebar-hidden.png) | ![No drive connected](../screenshots/no-drive.png) |

![NTFSTool について](../screenshots/about.png)

インターフェースは 20 言語に対応しており、アラビア語とペルシア語の右から左へのレイアウトにも対応しています：English、
简体中文、繁體中文、日本語、한국어、Deutsch、Français、Español、Italiano、Português、Русский、العربية、فارسی、
Türkçe、हिन्दी、বাংলা、Bahasa Indonesia、Kiswahili、ไทย、Tiếng Việt。

## このリポジトリについて

これは NTFSTool の**オープンソースの学習・リファレンス版**です。Mac App Store の無料版 NTFSTool から派生しており、
同じアーキテクチャを保ちつつ、意図的に小さく読みやすくしています。

- すべてのソースファイルに英語のコメントが付いているので、FSKit ファイルシステムモジュール、NTFS エンジンの上に載る C ブリッジ、
  サンドボックス化された SwiftUI/AppKit のフロントエンドがどのように組み合わさるかを学べます。
- ドライバはシンプルで直接的な I/O 経路を使用します（キャッシュ層やプラットフォーム固有の性能チューニングはありません）。
- アプリは基本機能のみです（テレメトリなし、診断情報のアップロードなし、ネットワークアクセスは一切なし）。
- ライセンス認証、アクティベーション、トライアルのコードはありません。

ユーザーとして最高の体験を求めるなら、[ntfstool.com](https://ntfstool.com) から公式ビルドをインストールしてください。
NTFSTool Pro（クローズドソース）には、古い macOS バージョン向けの第 2 のドライバ、ディスク修復、その他のツールが加わります。

## 機能

- NTFS ボリュームの読み取り**と書き込み**。macOS が FSKit ドライバモジュールを通じて自動的にマウントします
- アプリはサンドボックス化。ドライバはサンドボックス化された FSKit モジュールとしてユーザー空間で動作します
- 各 NTFS ボリュームの状態を表示：読み書き可能、読み取り専用（Windows のハイバネート / 高速スタートアップ）、
  Apple 標準ドライバによる読み取り専用、未マウント
- システム設定でファイルシステム拡張機能をオンにする手順を案内
- Finder で開く、取り出し、ディスク情報
- 20 の UI 言語
- 拡張属性は NTFS の名前付きストリームとして保存（`._` ファイルは作られません）。オプションの `windows_names` マウントオプションで、
  Windows で使えないファイル名を拒否できます

## 動作環境

- 実行には macOS 15.6 以降が必要です。**書き込みのサポートには macOS 26 以降が必要です**（macOS 15 では、
  物理ディスクに対してシステムが引き続き Apple の読み取り専用 NTFS ドライバを使用します）
- ビルドには、Xcode 16.3 以降（FSKit SDK）とそのコマンドラインツール、Swift 5.9 以上が必要です

## ビルド：コマンド 1 つで

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

その他のターゲット：

| コマンド | 内容 |
| --- | --- |
| `make run` | ビルドしてアプリを開く |
| `make demo` | メモリ上のデモディスクで UI を実行――ドライバも Apple Developer アカウントも不要 |
| `make test` | ユニットテストを実行 |
| `make app UNIVERSAL=1` | arm64 + x86_64 をビルド |
| `make scan` | 署名用の資材や非公開の識別子がツリーに含まれていないか確認 |

ドライバのテストでは、`ntfstool-driver` バイナリの `mkntfs` と `ntfsfix` サブコマンドを使って、実際の NTFS ディスクイメージをフォーマットし、検査します。Homebrew の `bin`/`sbin`、または `NTFSTOOL_NTFS_TOOLS=<dir>` で指定したディレクトリに両方のツールが見つからない限り、これらのテストはスキップされます。

## ドライバの実行（ご自身の Apple Developer アカウントが必要）

FSKit モジュールは制限付きエンタイトルメント `com.apple.developer.fskit.fsmodule` を使用します。macOS は、それを許可するプロビジョニングプロファイルに
裏付けられた署名を持つモジュールしか読み込みません。ad-hoc ビルドでも UI はすべて表示されますが、
システムはドライバを読み込みません。これはすべての FSKit プロジェクトに適用される Apple プラットフォームのルールです。

1. Apple Developer アカウントで、`com.yourname.ntfs` と `com.yourname.ntfs.fskit` のように 2 つの App ID を作成します。
   2 つ目に **FSKit Module** ケイパビリティを有効にし、必要に応じて両方に App Group
   （例：`TEAMID.com.yourname.ntfs`）を設定すると、アプリがドライバのログを読めるようになります。
2. 両方について開発用（または Developer ID）のプロビジョニングプロファイルを作成し、ダウンロードします。
3. ご自身の ID でビルドします：

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. `build/NTFSTool.app` を `/Applications` にコピーして一度開き、
   **システム設定 → 一般 → ログイン項目と拡張機能 → ファイルシステム拡張機能** でドライバをオンにします。
5. NTFS ドライブを挿し直します。`mount` の一覧にタイプ `ntfstooloss` で表示されるはずです。

すべてのビルドオプションは [`scripts/build-app.sh`](../../scripts/build-app.sh) の冒頭に記載されています。
プロビジョニングプロファイルや署名鍵はこのリポジトリに含めてはいけません。`make scan` がそれを検査します。

## ディレクトリ構成

| パス | 内容 |
| --- | --- |
| `Sources/NTFSTool` | アプリのエントリポイント（`main.swift`） |
| `Sources/NTFSToolApp` | AppKit シェル、SwiftUI ビュー、アプリモデル、システムサービス、20 言語のローカライズ |
| `Sources/NTFSToolCore` | AppKit に依存しない純粋なロジック：ディスクモデル、言語リスト、設定ストア、ローカルのイベントログ |
| `Sources/NTFSToolBridge` | `libntfstool-driver` の上に inode ベースの操作を公開する C ブリッジ（`ntfstool_bridge.c`） |
| `Sources/NTFSToolDriver` | ドライバの Swift コア（FSKit への依存なし、どの macOS でもユニットテスト可能） |
| `FSKitModule/NTFSToolFS` | macOS が NTFS ボリュームをマウントするために読み込む FSKit モジュール（`NTFSToolFS.appex`） |
| `Tests/` | アプリモデルとドライバのユニットテスト（実際の NTFS イメージを使用） |
| `scripts/` | `build-app.sh`、`build-fskit.sh`、`scan-secrets.sh` |

書き込みが Finder からディスクへどのように届くかは、[docs/ARCHITECTURE.md](../ARCHITECTURE.md) をご覧ください。

## コントリビューション

Issue とプルリクエストを歓迎します。[Developer Certificate of Origin](https://developercertificate.org/) を証明するために、
コミットには署名（`git commit -s`）を付けてください。詳しくは [CONTRIBUTING.md](../../CONTRIBUTING.md) をご覧ください。

## ライセンス

MIT License ――[LICENSE](../../LICENSE) と [NOTICE](../../NOTICE) をご覧ください。
同梱の NTFS エンジンは NTFSTool 独自の `libntfstool-driver` で、ビルド済みの静的ライブラリとして提供されます
（[Vendor/README.md](../../Vendor/README.md) を参照）。

このリポジトリの以前のバージョン（Electron アプリ）は git の履歴から引き続き参照できます。

## 商標

MIT License が与えるのはコードであり、ブランドではありません。「NTFSTool」および NTFSTool のアイコンとロゴは
NTFSTool プロジェクトの商標であり、MIT License の**対象外**です。

- このソースをご自身用にビルドして使用すること、また「NTFSTool をベースにした」と紹介することは自由です。
- 改変版やフォークを（あらゆるアプリストアを含め）**配布**する場合は、別の名前とアイコンを付け、NTFSTool のロゴを取り除き、
  独自のバンドル識別子を使用してください。公式の NTFSTool リリースのように見えたり、そう名乗ったりしてはなりません。
- このリポジトリの無改変ビルドは、事前の許可がある場合に限り、NTFSTool の名前で再配布できます。

NTFS と Windows は Microsoft Corporation の商標であり、macOS と FSKit は Apple Inc. の商標です。このプロジェクトは
Microsoft および Apple とは無関係です。
