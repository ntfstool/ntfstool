# NTFSTool (รุ่นโอเพนซอร์ส)

NTFS แบบอ่าน-เขียนสำหรับ macOS ที่ใช้ได้ฟรีและเป็นโอเพนซอร์ส สร้างบน **FSKit** ของ Apple และเอนจิน NTFS ของ NTFSTool เอง (**libntfstool-driver**)

เสียบไดรฟ์ Windows (NTFS) แล้ว macOS จะเมานต์ให้เขียนได้ผ่านโมดูลไดรเวอร์ FSKit ของ NTFSTool — ไม่ต้องใช้ kernel
extension ไม่ต้องใช้ macFUSE ไม่ต้องลดระดับการตั้งค่าความปลอดภัย และไม่ต้องใช้รหัสผ่านผู้ดูแลระบบ ตัวแอปทำงานอยู่ใน sandbox อย่างสมบูรณ์

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · **ไทย** · [Tiếng Việt](README.vi.md)

> **มองหาเวอร์ชันเก่า?** เวอร์ชันก่อนหน้าที่ใช้ Electron/Fuse (2.x – 4.7.x) และประวัติทั้งหมดถูกเก็บถาวรไว้ใน [บรานช์ `V3`](https://github.com/ntfstool/ntfstool/tree/V3) ซึ่งเลิกดูแลแล้ว ส่วน `master` คือเวอร์ชันปัจจุบันที่ใช้ FSKit

[คู่มือสถาปัตยกรรม](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="หน้าต่างหลักของ NTFSTool: ไดรฟ์ NTFS แบบอ่าน-เขียนพร้อมแถบความจุและรายละเอียด" width="760">
</p>

<p align="center"><i>หากโปรเจกต์นี้มีประโยชน์หรือน่าสนใจสำหรับคุณ การกด ⭐ บน GitHub จะช่วยให้คนอื่นค้นพบได้ง่ายขึ้น</i></p>

## ภาพหน้าจอ

ภาพหน้าจอจริงของแอปที่สร้างจากรีโพซิทอรีนี้ (`make app`) ไดรฟ์ที่แสดงคือไดรฟ์ตัวอย่างของ
โหมดสาธิตในตัว (`make demo`) คุณจึงลองได้ทุกหน้าจอโดยไม่ต้องมีดิสก์จริง

### หน้าต่างหลัก

หน้าต่างเดียวสำหรับไดรฟ์ NTFS ทั้งหมดของคุณ แต่ละวอลุ่มแสดงสถานะให้เห็นได้ในพริบตา — **อ่านและเขียน** **อ่านอย่างเดียว** หรือ
**ยังไม่ได้เมานต์** — พร้อมแถบความจุ จุดเมานต์ บัส (USB / Thunderbolt) และรายละเอียดอุปกรณ์ รวมถึง *แสดงใน
Finder* และ *นำออก*

![หน้าต่างหลัก: ไดรฟ์ USB แบบอ่าน-เขียน](../screenshots/overview.png)

| SSD Thunderbolt แบบอ่านและเขียน | ไดรฟ์ที่ยังไม่ได้เมานต์ |
| --- | --- |
| ![ไดรฟ์ Thunderbolt](../screenshots/thunderbolt-drive.png) | ![ไดรฟ์ที่ยังไม่ได้เมานต์](../screenshots/not-mounted.png) |

### การตั้งค่าแบบมีคำแนะนำ: ไม่ต้องใช้รหัสผ่าน ไม่ต้องใช้ kernel extension

NTFSTool เป็นโมดูลระบบไฟล์ FSKit ธรรมดา สิ่งเดียวที่ macOS ขอคือเปิดสวิตช์หนึ่งตัวใน
**การตั้งค่าระบบ → ทั่วไป → รายการเข้าสู่ระบบและส่วนขยาย → ส่วนขยายระบบไฟล์** แอปจะอธิบายทีละ
ขั้นตอนและตรวจจับการเปลี่ยนแปลงได้เอง

![เปิดส่วนขยายระบบไฟล์](../screenshots/enable-extension.png)

### ปลอดภัยตั้งแต่ค่าเริ่มต้น

หาก Windows อยู่ในสถานะไฮเบอร์เนตหรือเปิด *Fast Startup* ไว้ การเขียนข้อมูลจะทำให้ดิสก์เสียหาย NTFSTool จะเมานต์ไดรฟ์ดังกล่าว
แบบ**อ่านอย่างเดียว** และบอกวิธีแก้ไขอย่างชัดเจน แทนที่จะเสี่ยงกับข้อมูลของคุณ

| ไดรฟ์ที่ Windows ทิ้งไว้ในสถานะไฮเบอร์เนต | สิ่งที่แอปแจ้งคุณ |
| --- | --- |
| ![ไดรฟ์ที่ไฮเบอร์เนต](../screenshots/hibernated-windows.png) | ![ปิด Windows ให้สมบูรณ์](../screenshots/dialog-shut-down-windows.png) |

### การเสียบไดรฟ์ใหม่อีกครั้ง

macOS อนุญาตให้ระบบเมานต์ไดรฟ์จริงได้ตอนที่เสียบเท่านั้น แอปจึงนำไดรฟ์ออกอย่างปลอดภัยและ
ขอให้คุณเสียบกลับเข้าไปใหม่ จากนั้นจะเมานต์แบบอ่าน-เขียน

![เสียบไดรฟ์ใหม่อีกครั้ง](../screenshots/dialog-plug-in-again.png)

### โหมดมืด

| หน้าต่างหลัก | คู่มือส่วนขยาย |
| --- | --- |
| ![โหมดมืด หน้าต่างหลัก](../screenshots/dark-overview.png) | ![โหมดมืด คู่มือส่วนขยาย](../screenshots/dark-enable-extension.png) |

### เลย์เอาต์แบบกะทัดรัด สถานะว่าง และหน้าเกี่ยวกับ

| ซ่อนแถบด้านข้าง | กำลังรอไดรฟ์ |
| --- | --- |
| ![ซ่อนแถบด้านข้าง](../screenshots/sidebar-hidden.png) | ![ไม่มีไดรฟ์ที่เชื่อมต่อ](../screenshots/no-drive.png) |

![เกี่ยวกับ NTFSTool](../screenshots/about.png)

อินเทอร์เฟซมีให้เลือก 20 ภาษา รวมถึงเลย์เอาต์จากขวาไปซ้ายสำหรับภาษาอาหรับและเปอร์เซีย: English,
简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی,
Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย และ Tiếng Việt

## รีโพซิทอรีนี้คืออะไร

นี่คือ**รุ่นโอเพนซอร์สสำหรับการศึกษา / อ้างอิง**ของ NTFSTool พัฒนามาจาก NTFSTool แบบฟรีบน
Mac App Store และคงสถาปัตยกรรมเดิมไว้ แต่ตั้งใจทำให้มีขนาดเล็กและอ่านง่าย:

- ซอร์สไฟล์ทุกไฟล์มีคอมเมนต์เป็นภาษาอังกฤษ เพื่อให้คุณศึกษาได้ว่าโมดูลระบบไฟล์ FSKit, C bridge ที่อยู่บน
  เอนจิน NTFS และส่วนหน้า SwiftUI/AppKit ที่อยู่ใน sandbox ทำงานร่วมกันอย่างไร;
- ไดรเวอร์ใช้เส้นทาง I/O แบบตรงที่เรียบง่าย (ไม่มีชั้นแคชหรือการปรับแต่งประสิทธิภาพเฉพาะแพลตฟอร์ม);
- แอปทำเฉพาะสิ่งพื้นฐาน (ไม่มี telemetry ไม่อัปโหลดข้อมูลวินิจฉัย และไม่เข้าถึงเครือข่ายเลย);
- ไม่มีโค้ดเกี่ยวกับใบอนุญาต การเปิดใช้งาน หรือช่วงทดลองใช้

หากคุณต้องการประสบการณ์ที่ดีที่สุดในฐานะผู้ใช้ ให้ติดตั้งรุ่นทางการจาก [ntfstool.com](https://ntfstool.com)
NTFSTool Pro (ซอร์สปิด) เพิ่มไดรเวอร์ตัวที่สองสำหรับ macOS รุ่นเก่า การซ่อมแซมดิสก์ และเครื่องมืออื่น ๆ

## คุณสมบัติ

- อ่าน**และเขียน**วอลุ่ม NTFS ได้ macOS เมานต์ให้อัตโนมัติผ่านโมดูลไดรเวอร์ FSKit
- แอปอยู่ใน sandbox ไดรเวอร์ทำงานเป็นโมดูล FSKit ที่อยู่ใน sandbox ในพื้นที่ผู้ใช้
- แสดงสถานะของแต่ละวอลุ่ม NTFS: อ่าน-เขียน, อ่านอย่างเดียว (Windows ไฮเบอร์เนต / Fast Startup), อ่านอย่างเดียวโดย
  ไดรเวอร์ในตัวของ Apple, ยังไม่ได้เมานต์
- แนะนำการเปิดส่วนขยายระบบไฟล์ในการตั้งค่าระบบ
- เปิดใน Finder, นำออก, ข้อมูลดิสก์
- อินเทอร์เฟซ 20 ภาษา
- แอตทริบิวต์ส่วนขยายถูกเก็บเป็น named stream ของ NTFS (ไม่มีไฟล์ `._`) ตัวเลือกเมานต์ `windows_names` (ไม่บังคับ)
  จะปฏิเสธชื่อไฟล์ที่ Windows ใช้ไม่ได้

## ความต้องการของระบบ

- macOS 15.6 หรือใหม่กว่าสำหรับการใช้งาน; **การเขียนต้องใช้ macOS 26 หรือใหม่กว่า** (บน macOS 15 ระบบจะยังคงใช้
  ไดรเวอร์ NTFS แบบอ่านอย่างเดียวในตัวของ Apple สำหรับดิสก์จริง)
- สำหรับการบิลด์: Xcode 16.3 หรือใหม่กว่า (FSKit SDK) พร้อม command line tools, Swift 5.9+

## บิลด์ — ด้วยคำสั่งเดียว

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

เป้าหมายอื่น ๆ:

| คำสั่ง | ทำอะไร |
| --- | --- |
| `make run` | บิลด์และเปิดแอป |
| `make demo` | รัน UI ด้วยดิสก์สาธิตในหน่วยความจำ — ไม่ต้องใช้ไดรเวอร์และไม่ต้องมีบัญชี Apple Developer |
| `make test` | รันยูนิตเทสต์ |
| `make app UNIVERSAL=1` | บิลด์ arm64 + x86_64 |
| `make scan` | ตรวจสอบว่าไม่มีวัสดุสำหรับลงนามหรือตัวระบุส่วนตัวอยู่ในโครงสร้างไฟล์ |

เทสต์ของไดรเวอร์จะฟอร์แมตและตรวจสอบอิมเมจดิสก์ NTFS จริงด้วยคำสั่งย่อย `mkntfs` และ `ntfsfix` ของไบนารี `ntfstool-driver` เทสต์จะถูกข้าม
เว้นแต่จะพบเครื่องมือทั้งสองใน `bin`/`sbin` ของ Homebrew หรือในไดเรกทอรีที่ระบุด้วย `NTFSTOOL_NTFS_TOOLS=<dir>`

## การรันไดรเวอร์ (ต้องมีบัญชี Apple Developer ของคุณเอง)

โมดูล FSKit ใช้ entitlement ที่ถูกจำกัด `com.apple.developer.fskit.fsmodule` macOS จะโหลดเฉพาะโมดูล
ที่ลายเซ็นได้รับการรองรับโดย provisioning profile ที่อนุญาตสิทธิ์นั้น บิลด์แบบ ad-hoc แสดง UI ได้ครบ แต่
ระบบจะไม่โหลดไดรเวอร์ นี่คือกฎของแพลตฟอร์ม Apple ที่ใช้กับทุกโปรเจกต์ FSKit

1. ในบัญชี Apple Developer ของคุณ สร้าง App ID สองตัว เช่น `com.yourname.ntfs` และ `com.yourname.ntfs.fskit`
   เปิดความสามารถ **FSKit Module** ที่ตัวที่สอง และจะเพิ่ม App Group
   (เช่น `TEAMID.com.yourname.ntfs`) ให้ทั้งสองตัวด้วยก็ได้ เพื่อให้แอปอ่านล็อกของไดรเวอร์ได้
2. สร้าง provisioning profile สำหรับการพัฒนา (หรือ Developer ID) ของทั้งสองตัวแล้วดาวน์โหลด
3. บิลด์ด้วยข้อมูลประจำตัวของคุณ:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. คัดลอก `build/NTFSTool.app` ไปที่ `/Applications` เปิดหนึ่งครั้ง แล้วเปิดไดรเวอร์ใน
   **การตั้งค่าระบบ → ทั่วไป → รายการเข้าสู่ระบบและส่วนขยาย → ส่วนขยายระบบไฟล์**
5. เสียบไดรฟ์ NTFS ใหม่ จากนั้น `mount` ควรแสดงไดรฟ์นั้นด้วยชนิด `ntfstooloss`

ตัวเลือกการบิลด์ทั้งหมดมีอธิบายไว้ที่ส่วนบนของ [`scripts/build-app.sh`](../../scripts/build-app.sh)
provisioning profile และคีย์สำหรับลงนามต้องไม่อยู่ในรีโพซิทอรีนี้ `make scan` จะตรวจสอบเรื่องนี้

## โครงสร้าง

| เส้นทาง | คืออะไร |
| --- | --- |
| `Sources/NTFSTool` | จุดเริ่มต้นของแอป (`main.swift`) |
| `Sources/NTFSToolApp` | เปลือก AppKit, วิว SwiftUI, โมเดลของแอป, บริการระบบ, การแปล 20 ภาษา |
| `Sources/NTFSToolCore` | ลอจิกล้วนที่ไม่พึ่ง AppKit: โมเดลดิสก์, รายการภาษา, ที่เก็บการตั้งค่า, บันทึกเหตุการณ์ในเครื่อง |
| `Sources/NTFSToolBridge` | C bridge (`ntfstool_bridge.c`) ที่เปิดให้ใช้การดำเนินการแบบอิง inode บน `libntfstool-driver` |
| `Sources/NTFSToolDriver` | แกน Swift ของไดรเวอร์ (ไม่พึ่ง FSKit ทดสอบยูนิตได้บน macOS ใดก็ได้) |
| `FSKitModule/NTFSToolFS` | โมดูล FSKit (`NTFSToolFS.appex`) ที่ macOS โหลดเพื่อเมานต์วอลุ่ม NTFS |
| `Tests/` | ยูนิตเทสต์สำหรับโมเดลของแอปและสำหรับไดรเวอร์ (อิมเมจ NTFS จริง) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

ดู [docs/ARCHITECTURE.md](../ARCHITECTURE.md) เพื่อทราบว่าการเขียนข้อมูลเดินทางจาก Finder ไปถึงดิสก์อย่างไร

## การมีส่วนร่วม

ยินดีรับ issue และ pull request กรุณาเซ็นรับรองคอมมิตของคุณ (`git commit -s`) เพื่อรับรอง
[Developer Certificate of Origin](https://developercertificate.org/) — ดู [CONTRIBUTING.md](../../CONTRIBUTING.md)

## สัญญาอนุญาต

MIT License — ดู [LICENSE](../../LICENSE) และ [NOTICE](../../NOTICE)

## เครื่องหมายการค้า

**โปรดอย่านำแอปนี้หรือฟอร์กของแอปนี้ไปเผยแพร่บน Mac App Store** — เพราะจะแข่งกับเวอร์ชันฟรีอย่างเป็นทางการและทำให้ผู้ใช้สับสน
