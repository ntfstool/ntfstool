# NTFSTool (오픈 소스 에디션)

Apple의 **FSKit**과 NTFSTool 자체 NTFS 엔진(**libntfstool-driver**)을 기반으로 만든, macOS용 무료 오픈 소스 NTFS 읽기/쓰기 도구입니다.

Windows(NTFS) 드라이브를 연결하면 macOS가 NTFSTool의 FSKit 드라이버 모듈을 통해 쓰기 가능한 상태로 마운트합니다.
커널 확장도, macFUSE도, 보안 설정 낮추기도, 관리자 암호도 필요 없습니다. 앱 자체는 완전히 샌드박스 안에서 실행됩니다.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · **한국어** · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

[아키텍처 가이드](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="NTFSTool 메인 창: 용량 막대와 세부 정보가 표시된 읽기/쓰기 가능한 NTFS 드라이브" width="760">
</p>

<p align="center"><i>이 프로젝트가 유용하거나 흥미롭다면 GitHub에서 ⭐를 눌러 주세요. 더 많은 사람이 이 프로젝트를 찾는 데 도움이 됩니다.</i></p>

## 스크린샷

이 저장소에서 빌드한 앱(`make app`)의 실제 스크린샷입니다. 표시된 드라이브는 내장 데모 모드(`make demo`)의
샘플 드라이브이므로, 물리 디스크가 없어도 모든 화면을 직접 체험할 수 있습니다.

### 메인 창

하나의 창에서 모든 NTFS 드라이브를 관리합니다. 각 볼륨의 상태――**읽기 및 쓰기**, **읽기 전용**, **마운트되지 않음**――를
용량 막대, 마운트 지점, 버스(USB / Thunderbolt), 장치 세부 정보와 함께 한눈에 볼 수 있으며,
*Finder에서 보기*와 *추출*도 사용할 수 있습니다.

![메인 창: 읽기/쓰기 가능한 USB 드라이브](../screenshots/overview.png)

| 읽기 및 쓰기가 가능한 Thunderbolt SSD | 마운트되지 않은 드라이브 |
| --- | --- |
| ![Thunderbolt drive](../screenshots/thunderbolt-drive.png) | ![Not mounted drive](../screenshots/not-mounted.png) |

### 안내형 설정: 암호도, 커널 확장도 필요 없음

NTFSTool은 일반적인 FSKit 파일 시스템 모듈입니다. macOS가 요구하는 것은
**시스템 설정 → 일반 → 로그인 항목 및 확장 프로그램 → 파일 시스템 확장 프로그램**에서 스위치 하나를 켜는 것뿐입니다.
앱이 단계별로 안내하고, 설정이 바뀐 것도 스스로 감지합니다.

![파일 시스템 확장 프로그램 켜기](../screenshots/enable-extension.png)

### 기본적으로 안전

Windows가 최대 절전 모드에 들어갔거나 *빠른 시작*이 켜져 있으면 쓰기 작업이 디스크를 손상시킬 수 있습니다.
NTFSTool은 이런 드라이브를 **읽기 전용**으로 마운트하고, 데이터를 위험에 빠뜨리는 대신 해결 방법을 정확히 알려 줍니다.

| Windows가 최대 절전 모드로 남겨 둔 드라이브 | 앱이 알려 주는 내용 |
| --- | --- |
| ![Hibernated drive](../screenshots/hibernated-windows.png) | ![Shut Down Windows Completely](../screenshots/dialog-shut-down-windows.png) |

### 드라이브 다시 연결하기

macOS는 물리 드라이브가 연결되는 시점에만 시스템이 마운트하도록 허용하므로, 앱은 드라이브를 안전하게 추출하고
다시 연결해 달라고 요청합니다. 그러면 읽기/쓰기 가능한 상태로 마운트됩니다.

![드라이브 다시 연결하기](../screenshots/dialog-plug-in-again.png)

### 다크 모드

| 메인 창 | 확장 프로그램 안내 |
| --- | --- |
| ![Dark mode, main window](../screenshots/dark-overview.png) | ![Dark mode, extension guide](../screenshots/dark-enable-extension.png) |

### 컴팩트 레이아웃, 빈 상태, 정보

| 사이드바 숨김 | 드라이브 대기 중 |
| --- | --- |
| ![Sidebar hidden](../screenshots/sidebar-hidden.png) | ![No drive connected](../screenshots/no-drive.png) |

![NTFSTool 정보](../screenshots/about.png)

인터페이스는 아랍어와 페르시아어의 오른쪽에서 왼쪽 레이아웃을 포함해 20개 언어로 제공됩니다: English,
简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی,
Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย, Tiếng Việt.

## 이 저장소는 무엇인가요

이 저장소는 NTFSTool의 **오픈 소스 학습/참고용 에디션**입니다. Mac App Store의 무료 NTFSTool에서 파생되어
동일한 아키텍처를 유지하지만, 의도적으로 작고 읽기 쉽게 만들었습니다.

- 모든 소스 파일에 영어 주석이 달려 있어, FSKit 파일 시스템 모듈, NTFS 엔진 위의 C 브리지,
  샌드박스 SwiftUI/AppKit 프런트엔드가 어떻게 맞물리는지 공부할 수 있습니다.
- 드라이버는 단순하고 직접적인 I/O 경로를 사용합니다(캐시 계층이나 플랫폼별 성능 튜닝 없음).
- 앱은 기본 기능만 제공합니다(텔레메트리 없음, 진단 정보 업로드 없음, 네트워크 접근 전혀 없음).
- 라이선스, 활성화, 체험판 관련 코드는 없습니다.

사용자로서 최상의 경험을 원한다면 [ntfstool.com](https://ntfstool.com)에서 공식 빌드를 설치하세요.
NTFSTool Pro(비공개 소스)는 이전 macOS 버전용 두 번째 드라이버, 디스크 복구 및 기타 도구를 추가로 제공합니다.

## 기능

- NTFS 볼륨 읽기 **및 쓰기**. macOS가 FSKit 드라이버 모듈을 통해 자동으로 마운트합니다
- 샌드박스 앱. 드라이버는 사용자 공간에서 샌드박스 FSKit 모듈로 실행됩니다
- 각 NTFS 볼륨의 상태 표시: 읽기/쓰기, 읽기 전용(Windows 최대 절전 모드 / 빠른 시작),
  Apple 내장 드라이버에 의한 읽기 전용, 마운트되지 않음
- 시스템 설정에서 파일 시스템 확장 프로그램을 켜는 방법 안내
- Finder에서 열기, 추출, 디스크 정보
- 20개 UI 언어
- 확장 속성을 NTFS 명명된 스트림으로 저장(`._` 파일 없음). 선택 사항인 `windows_names` 마운트 옵션은
  Windows에서 사용할 수 없는 파일 이름을 거부합니다

## 요구 사항

- 실행하려면 macOS 15.6 이상이 필요합니다. **쓰기 지원에는 macOS 26 이상이 필요합니다**(macOS 15에서는 물리 디스크에 대해
  시스템이 계속 Apple의 읽기 전용 NTFS 드라이버를 사용합니다)
- 빌드하려면 Xcode 16.3 이상(FSKit SDK)과 해당 명령줄 도구, Swift 5.9 이상이 필요합니다

## 빌드 — 명령 하나로

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

기타 타깃:

| 명령 | 동작 |
| --- | --- |
| `make run` | 빌드 후 앱 열기 |
| `make demo` | 메모리 내 데모 디스크로 UI 실행 — 드라이버도 Apple Developer 계정도 필요 없음 |
| `make test` | 단위 테스트 실행 |
| `make app UNIVERSAL=1` | arm64 + x86_64 빌드 |
| `make scan` | 서명 자료나 비공개 식별자가 트리에 없는지 확인 |

드라이버 테스트는 `ntfstool-driver` 바이너리의 `mkntfs` 및 `ntfsfix` 하위 명령으로 실제 NTFS 디스크 이미지를 포맷하고 검사합니다. 두 도구가 Homebrew의 `bin`/`sbin` 또는 `NTFSTOOL_NTFS_TOOLS=<dir>`로 지정한 디렉터리에서 모두 발견되지 않으면 이 테스트는 건너뜁니다.

## 드라이버 실행 (본인의 Apple Developer 계정 필요)

FSKit 모듈은 제한된 권한 `com.apple.developer.fskit.fsmodule`을 사용합니다. macOS는 이 권한을 부여하는 프로비저닝 프로파일로
뒷받침되는 서명을 가진 모듈만 로드합니다. ad-hoc 빌드는 전체 UI를 보여 주지만,
시스템이 드라이버를 로드하지 않습니다. 이는 모든 FSKit 프로젝트에 적용되는 Apple 플랫폼의 규칙입니다.

1. Apple Developer 계정에서 `com.yourname.ntfs`와 `com.yourname.ntfs.fskit` 같은 App ID 두 개를 만듭니다.
   두 번째 App ID에는 **FSKit Module** 기능을 켜고, 앱이 드라이버 로그를 읽을 수 있도록 원한다면 두 App ID 모두에 App Group
   (예: `TEAMID.com.yourname.ntfs`)을 설정합니다.
2. 두 App ID에 대한 개발용(또는 Developer ID) 프로비저닝 프로파일을 만들어 다운로드합니다.
3. 본인의 ID로 빌드합니다:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. `build/NTFSTool.app`을 `/Applications`로 복사하고 한 번 연 다음,
   **시스템 설정 → 일반 → 로그인 항목 및 확장 프로그램 → 파일 시스템 확장 프로그램**에서 드라이버를 켭니다.
5. NTFS 드라이브를 다시 연결합니다. 이제 `mount`가 `ntfstooloss` 유형으로 이를 표시해야 합니다.

모든 빌드 옵션은 [`scripts/build-app.sh`](../../scripts/build-app.sh) 상단에 문서화되어 있습니다.
프로비저닝 프로파일과 서명 키는 이 저장소에 절대 포함해서는 안 되며, `make scan`이 이를 검사합니다.

## 구성

| 경로 | 설명 |
| --- | --- |
| `Sources/NTFSTool` | 앱 진입점(`main.swift`) |
| `Sources/NTFSToolApp` | AppKit 셸, SwiftUI 뷰, 앱 모델, 시스템 서비스, 20개 현지화 |
| `Sources/NTFSToolCore` | AppKit 없는 순수 로직: 디스크 모델, 언어 목록, 설정 저장소, 로컬 이벤트 로그 |
| `Sources/NTFSToolBridge` | `libntfstool-driver` 위에서 inode 기반 연산을 제공하는 C 브리지(`ntfstool_bridge.c`) |
| `Sources/NTFSToolDriver` | 드라이버의 Swift 코어(FSKit 의존성 없음, 어떤 macOS에서도 단위 테스트 가능) |
| `FSKitModule/NTFSToolFS` | macOS가 NTFS 볼륨을 마운트하기 위해 로드하는 FSKit 모듈(`NTFSToolFS.appex`) |
| `Tests/` | 앱 모델과 드라이버에 대한 단위 테스트(실제 NTFS 이미지 사용) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

쓰기 작업이 Finder에서 디스크까지 어떻게 전달되는지는 [docs/ARCHITECTURE.md](../ARCHITECTURE.md)를 참고하세요.

## 기여하기

이슈와 풀 리퀘스트를 환영합니다. [Developer Certificate of Origin](https://developercertificate.org/)을 보증하기 위해
커밋에 서명(`git commit -s`)해 주세요. 자세한 내용은 [CONTRIBUTING.md](../../CONTRIBUTING.md)를 참고하세요.

## 라이선스

MIT License — [LICENSE](../../LICENSE)와 [NOTICE](../../NOTICE)를 참고하세요.
번들로 포함된 NTFS 엔진은 NTFSTool 자체 `libntfstool-driver`이며, 미리 빌드된 정적 라이브러리로 제공됩니다
([Vendor/README.md](../../Vendor/README.md) 참고).

이 저장소의 이전 버전(Electron 앱)은 git 기록에 남아 있습니다.

## 상표

MIT License는 코드를 제공할 뿐, 브랜드를 제공하지 않습니다. "NTFSTool"과 NTFSTool 아이콘 및 로고는
NTFSTool 프로젝트의 상표이며 MIT License의 허가 대상이 **아닙니다**.

- 이 소스를 직접 빌드하여 사용할 수 있으며, "NTFSTool 기반"이라고 언급할 수 있습니다.
- 수정된 버전이나 포크를 (모든 앱 스토어를 포함하여) **배포**하는 경우, 다른 이름과 아이콘을 사용하고, NTFSTool 로고를 제거하고,
  자신의 번들 식별자를 사용해야 합니다. 공식 NTFSTool 릴리스처럼 보이거나 그렇게 주장해서는 안 됩니다.
- 이 저장소의 수정되지 않은 빌드는 사전 허가가 있는 경우에만 NTFSTool 이름으로 재배포할 수 있습니다.

NTFS와 Windows는 Microsoft Corporation의 상표이며, macOS와 FSKit은 Apple Inc.의 상표입니다. 이
프로젝트는 Microsoft 또는 Apple과 관련이 없습니다.
