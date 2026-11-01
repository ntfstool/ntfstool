# NTFSTool (phiên bản mã nguồn mở)

NTFS đọc-ghi miễn phí, mã nguồn mở cho macOS, xây dựng trên **FSKit** của Apple và engine NTFS riêng của NTFSTool (**libntfstool-driver**).

Cắm ổ đĩa Windows (NTFS) vào và macOS sẽ gắn kết ở chế độ có thể ghi thông qua mô-đun driver FSKit của NTFSTool — không cần kernel
extension, không cần macFUSE, không phải hạ thấp thiết lập bảo mật, không cần mật khẩu quản trị. Bản thân ứng dụng chạy hoàn toàn trong sandbox.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · **Tiếng Việt**

> **Tìm bản cũ?** Các bản trước dựa trên Electron/Fuse (2.x – 4.7.x) cùng toàn bộ lịch sử được lưu trữ ở [nhánh `V3`](https://github.com/ntfstool/ntfstool/tree/V3). Không còn được bảo trì; `master` là bản hiện tại dựa trên FSKit.

[Hướng dẫn kiến trúc](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="Cửa sổ chính của NTFSTool: một ổ NTFS đọc-ghi với thanh dung lượng và thông tin chi tiết" width="760">
</p>

<p align="center"><i>Nếu dự án này hữu ích hoặc thú vị với bạn, một ⭐ trên GitHub sẽ giúp người khác tìm thấy nó.</i></p>

## Ảnh chụp màn hình

Ảnh chụp thực tế của ứng dụng được xây dựng từ kho mã này (`make app`). Các ổ đĩa hiển thị là ổ đĩa mẫu của
chế độ demo tích hợp sẵn (`make demo`), nên bạn có thể thử mọi màn hình mà không cần đĩa vật lý.

### Cửa sổ chính

Một cửa sổ cho tất cả ổ NTFS của bạn. Mỗi phân vùng hiển thị trạng thái ngay trong nháy mắt — **Đọc & Ghi**, **Chỉ đọc** hoặc
**Chưa gắn kết** — kèm thanh dung lượng, điểm gắn kết, bus (USB / Thunderbolt) và thông tin thiết bị, cùng với *Hiện trong
Finder* và *Đẩy ra*.

![Cửa sổ chính: một ổ USB đọc-ghi](../screenshots/overview.png)

| Một SSD Thunderbolt, đọc & ghi | Một ổ đĩa chưa được gắn kết |
| --- | --- |
| ![Ổ Thunderbolt](../screenshots/thunderbolt-drive.png) | ![Ổ đĩa chưa gắn kết](../screenshots/not-mounted.png) |

### Cài đặt có hướng dẫn: không mật khẩu, không kernel extension

NTFSTool là một mô-đun hệ thống tệp FSKit thông thường. Điều duy nhất macOS yêu cầu là bật một công tắc trong
**Cài đặt Hệ thống → Cài đặt chung → Mục đăng nhập & Tiện ích mở rộng → Tiện ích mở rộng hệ thống tệp**. Ứng dụng giải thích từng
bước và tự nhận biết khi bạn thay đổi.

![Bật tiện ích mở rộng hệ thống tệp](../screenshots/enable-extension.png)

### An toàn theo mặc định

Nếu Windows đã ngủ đông hoặc *Fast Startup* đang bật, việc ghi sẽ làm hỏng đĩa. NTFSTool gắn kết ổ đĩa như vậy ở chế độ
**chỉ đọc** và chỉ cho bạn chính xác cách khắc phục, thay vì mạo hiểm với dữ liệu của bạn.

| Ổ đĩa bị Windows để lại ở trạng thái ngủ đông | Ứng dụng thông báo gì cho bạn |
| --- | --- |
| ![Ổ đĩa ngủ đông](../screenshots/hibernated-windows.png) | ![Tắt hoàn toàn Windows](../screenshots/dialog-shut-down-windows.png) |

### Cắm lại ổ đĩa

macOS chỉ cho phép hệ thống gắn kết các ổ đĩa vật lý khi chúng được cắm vào, nên ứng dụng sẽ đẩy ổ đĩa ra an toàn và
yêu cầu bạn cắm lại; sau đó ổ đĩa được gắn kết ở chế độ đọc-ghi.

![Cắm lại ổ đĩa](../screenshots/dialog-plug-in-again.png)

### Chế độ tối

| Cửa sổ chính | Hướng dẫn tiện ích mở rộng |
| --- | --- |
| ![Chế độ tối, cửa sổ chính](../screenshots/dark-overview.png) | ![Chế độ tối, hướng dẫn tiện ích mở rộng](../screenshots/dark-enable-extension.png) |

### Bố cục gọn, trạng thái trống và Giới thiệu

| Ẩn thanh bên | Đang chờ ổ đĩa |
| --- | --- |
| ![Ẩn thanh bên](../screenshots/sidebar-hidden.png) | ![Chưa kết nối ổ đĩa](../screenshots/no-drive.png) |

![Giới thiệu NTFSTool](../screenshots/about.png)

Giao diện có sẵn bằng 20 ngôn ngữ, bao gồm bố cục từ phải sang trái cho tiếng Ả Rập và tiếng Ba Tư: English,
简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی,
Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย và Tiếng Việt.

## Kho mã này là gì

Đây là **phiên bản mã nguồn mở dùng để học tập / tham khảo** của NTFSTool. Nó được phát triển từ NTFSTool miễn phí trên
Mac App Store và giữ nguyên kiến trúc, nhưng được cố ý giữ nhỏ gọn và dễ đọc:

- mọi tệp nguồn đều có chú thích bằng tiếng Anh, để bạn có thể tìm hiểu cách một mô-đun hệ thống tệp FSKit, một cầu nối C đặt trên
  engine NTFS, và giao diện SwiftUI/AppKit chạy trong sandbox khớp với nhau như thế nào;
- driver dùng đường dẫn I/O trực tiếp, đơn giản (không có lớp cache hay tinh chỉnh hiệu năng theo nền tảng);
- ứng dụng chỉ làm những việc cơ bản (không telemetry, không tải lên dữ liệu chẩn đoán, hoàn toàn không truy cập mạng);
- không có mã giấy phép, kích hoạt hay dùng thử.

Nếu bạn chỉ muốn trải nghiệm tốt nhất với tư cách người dùng, hãy cài các bản chính thức từ [ntfstool.com](https://ntfstool.com).
NTFSTool Pro (mã nguồn đóng) bổ sung driver thứ hai cho các phiên bản macOS cũ hơn, sửa chữa đĩa và các công cụ khác.

## Tính năng

- Đọc **và ghi** các phân vùng NTFS; macOS tự động gắn kết chúng thông qua mô-đun driver FSKit
- Ứng dụng chạy trong sandbox; driver chạy như một mô-đun FSKit trong sandbox ở không gian người dùng
- Hiển thị trạng thái của từng phân vùng NTFS: đọc-ghi, chỉ đọc (Windows ngủ đông / Fast Startup), chỉ đọc do
  driver tích hợp sẵn của Apple, chưa gắn kết
- Hướng dẫn bạn bật tiện ích mở rộng hệ thống tệp trong Cài đặt Hệ thống
- Mở trong Finder, đẩy ra, thông tin đĩa
- 20 ngôn ngữ giao diện
- Thuộc tính mở rộng được lưu dưới dạng luồng có tên của NTFS (không có tệp `._`); tùy chọn gắn kết `windows_names` (không bắt buộc)
  từ chối các tên tệp mà Windows không thể dùng

## Yêu cầu

- macOS 15.6 trở lên để chạy; **hỗ trợ ghi cần macOS 26 trở lên** (trên macOS 15, hệ thống tiếp tục dùng
  driver NTFS chỉ đọc tích hợp sẵn của Apple cho đĩa vật lý)
- Để build: Xcode 16.3 trở lên (FSKit SDK) cùng các công cụ dòng lệnh của nó, Swift 5.9+

## Build — một lệnh

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Các target khác:

| Lệnh | Tác dụng |
| --- | --- |
| `make run` | build và mở ứng dụng |
| `make demo` | chạy giao diện với các đĩa demo trong bộ nhớ — không cần driver và không cần tài khoản Apple Developer |
| `make test` | chạy unit test |
| `make app UNIVERSAL=1` | build arm64 + x86_64 |
| `make scan` | kiểm tra rằng trong cây thư mục không có tài liệu ký hay định danh riêng tư |

Các bài kiểm thử driver định dạng và kiểm tra các image đĩa NTFS thật bằng các lệnh con `mkntfs` và `ntfsfix` của tệp nhị phân `ntfstool-driver`. Chúng bị bỏ qua
trừ khi tìm thấy cả hai công cụ trong `bin`/`sbin` của Homebrew hoặc trong thư mục được chỉ định bởi `NTFSTOOL_NTFS_TOOLS=<dir>`.

## Chạy driver (cần tài khoản Apple Developer của riêng bạn)

Mô-đun FSKit dùng entitlement bị hạn chế `com.apple.developer.fskit.fsmodule`. macOS chỉ nạp một mô-đun
có chữ ký được hậu thuẫn bởi provisioning profile cấp quyền đó. Bản build ad-hoc hiển thị đầy đủ giao diện, nhưng
hệ thống sẽ không nạp driver. Đây là quy tắc nền tảng của Apple áp dụng cho mọi dự án FSKit.

1. Trong tài khoản Apple Developer của bạn, tạo hai App ID, ví dụ `com.yourname.ntfs` và `com.yourname.ntfs.fskit`.
   Bật khả năng **FSKit Module** cho cái thứ hai, và tùy chọn thêm App Group
   (ví dụ `TEAMID.com.yourname.ntfs`) cho cả hai để ứng dụng có thể đọc nhật ký của driver.
2. Tạo provisioning profile phát triển (hoặc Developer ID) cho cả hai và tải về.
3. Build bằng danh tính của bạn:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Sao chép `build/NTFSTool.app` vào `/Applications`, mở một lần, rồi bật driver trong
   **Cài đặt Hệ thống → Cài đặt chung → Mục đăng nhập & Tiện ích mở rộng → Tiện ích mở rộng hệ thống tệp**.
5. Cắm lại ổ NTFS. Lúc này `mount` sẽ liệt kê nó với kiểu `ntfstooloss`.

Mọi tùy chọn build được ghi chú ở đầu tệp [`scripts/build-app.sh`](../../scripts/build-app.sh).
Provisioning profile và khóa ký không bao giờ được đưa vào kho mã này; `make scan` sẽ kiểm tra điều đó.

## Cấu trúc

| Đường dẫn | Nội dung |
| --- | --- |
| `Sources/NTFSTool` | Điểm vào của ứng dụng (`main.swift`) |
| `Sources/NTFSToolApp` | Lớp vỏ AppKit, các view SwiftUI, mô hình ứng dụng, dịch vụ hệ thống, 20 bản địa hóa |
| `Sources/NTFSToolCore` | Logic thuần không phụ thuộc AppKit: mô hình đĩa, danh sách ngôn ngữ, kho lưu cài đặt, nhật ký sự kiện cục bộ |
| `Sources/NTFSToolBridge` | Cầu nối C (`ntfstool_bridge.c`) cung cấp các thao tác theo inode trên nền `libntfstool-driver` |
| `Sources/NTFSToolDriver` | Lõi Swift của driver (không phụ thuộc FSKit, có thể unit test trên mọi macOS) |
| `FSKitModule/NTFSToolFS` | Mô-đun FSKit (`NTFSToolFS.appex`) mà macOS nạp để gắn kết các phân vùng NTFS |
| `Tests/` | Unit test cho mô hình ứng dụng và cho driver (image NTFS thật) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

Xem [docs/ARCHITECTURE.md](../ARCHITECTURE.md) để biết một thao tác ghi đi từ Finder đến đĩa như thế nào.

## Đóng góp

Chào đón các issue và pull request. Vui lòng ký xác nhận các commit của bạn (`git commit -s`) để chứng nhận
[Developer Certificate of Origin](https://developercertificate.org/) — xem [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Giấy phép


## Nhãn hiệu

**Vui lòng không đưa ứng dụng này, hoặc bản fork của nó, lên Mac App Store** — việc đó cạnh tranh với bản miễn phí chính thức và gây nhầm lẫn cho người dùng.
