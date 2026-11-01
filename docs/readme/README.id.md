# NTFSTool (edisi open-source)

NTFS baca-tulis gratis dan open-source untuk macOS, dibangun di atas **FSKit** milik Apple dan mesin NTFS buatan NTFSTool sendiri (**libntfstool-driver**).

Colokkan drive Windows (NTFS) dan macOS akan memasangnya dengan akses tulis melalui modul driver FSKit milik NTFSTool — tanpa kernel
extension, tanpa macFUSE, tanpa menurunkan pengaturan keamanan, tanpa kata sandi admin. Aplikasinya sendiri berjalan sepenuhnya dalam sandbox.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · **Bahasa Indonesia** · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

> **Mencari versi lama?** Rilis lama berbasis Electron/Fuse (2.x – 4.7.x) beserta seluruh riwayatnya diarsipkan di [branch `V3`](https://github.com/ntfstool/ntfstool/tree/V3). Tidak lagi dipelihara; `master` adalah edisi terkini berbasis FSKit.

[Panduan arsitektur](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="Jendela utama NTFSTool: drive NTFS baca-tulis dengan bilah kapasitas dan detailnya" width="760">
</p>

<p align="center"><i>Jika proyek ini bermanfaat atau menarik bagi Anda, ⭐ di GitHub membantu orang lain menemukannya.</i></p>

## Tangkapan layar

Tangkapan layar asli dari aplikasi yang dibangun dari repositori ini (`make app`). Drive yang ditampilkan adalah drive contoh dari
mode demo bawaan (`make demo`), sehingga Anda dapat mencoba setiap layar tanpa disk fisik.

### Jendela utama

Satu jendela untuk semua drive NTFS Anda. Setiap volume menampilkan statusnya sekilas — **Baca & Tulis**, **Hanya Baca** atau
**Tidak Terpasang** — dengan bilah kapasitas, titik pemasangan, bus (USB / Thunderbolt) dan detail perangkat, ditambah *Tampilkan di
Finder* dan *Keluarkan*.

![Jendela utama: drive USB baca-tulis](../screenshots/overview.png)

| SSD Thunderbolt, baca & tulis | Drive yang tidak terpasang |
| --- | --- |
| ![Drive Thunderbolt](../screenshots/thunderbolt-drive.png) | ![Drive tidak terpasang](../screenshots/not-mounted.png) |

### Penyiapan terpandu: tanpa kata sandi, tanpa kernel extension

NTFSTool adalah modul sistem berkas FSKit biasa. Satu-satunya hal yang diminta macOS adalah satu sakelar di
**Pengaturan Sistem → Umum → Item Login & Ekstensi → Ekstensi Sistem Berkas**. Aplikasi menjelaskannya langkah demi
langkah dan mendeteksi perubahan tersebut secara otomatis.

![Aktifkan ekstensi sistem berkas](../screenshots/enable-extension.png)

### Aman secara default

Jika Windows dalam keadaan hibernasi atau *Fast Startup* aktif, penulisan akan merusak disk. NTFSTool memasang drive seperti itu
dalam mode **hanya baca** dan memberi tahu Anda dengan tepat cara memperbaikinya, alih-alih mempertaruhkan data Anda.

| Drive yang ditinggal hibernasi oleh Windows | Yang ditampilkan aplikasi |
| --- | --- |
| ![Drive hibernasi](../screenshots/hibernated-windows.png) | ![Matikan Windows sepenuhnya](../screenshots/dialog-shut-down-windows.png) |

### Mencolokkan drive kembali

macOS hanya mengizinkan sistem memasang drive fisik saat drive dicolokkan, jadi aplikasi mengeluarkan drive dengan aman dan
meminta Anda mencolokkannya kembali; setelah itu drive terpasang dengan akses baca-tulis.

![Colokkan kembali drive](../screenshots/dialog-plug-in-again.png)

### Mode gelap

| Jendela utama | Panduan ekstensi |
| --- | --- |
| ![Mode gelap, jendela utama](../screenshots/dark-overview.png) | ![Mode gelap, panduan ekstensi](../screenshots/dark-enable-extension.png) |

### Tata letak ringkas, kondisi kosong, dan Tentang

| Bilah samping disembunyikan | Menunggu drive |
| --- | --- |
| ![Bilah samping disembunyikan](../screenshots/sidebar-hidden.png) | ![Tidak ada drive terhubung](../screenshots/no-drive.png) |

![Tentang NTFSTool](../screenshots/about.png)

Antarmuka tersedia dalam 20 bahasa, termasuk tata letak kanan-ke-kiri untuk bahasa Arab dan Persia: English,
简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی,
Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย dan Tiếng Việt.

## Apa itu repositori ini

Ini adalah **edisi open-source untuk belajar / referensi** dari NTFSTool. Edisi ini diturunkan dari NTFSTool gratis di
Mac App Store dan mempertahankan arsitektur yang sama, tetapi sengaja dibuat kecil dan mudah dibaca:

- setiap berkas sumber diberi komentar dalam bahasa Inggris, sehingga Anda dapat mempelajari bagaimana modul sistem berkas FSKit, jembatan C di atas
  mesin NTFS, dan antarmuka SwiftUI/AppKit yang disandbox saling menyatu;
- driver menggunakan jalur I/O langsung yang sederhana (tanpa lapisan cache atau penyetelan performa khusus platform);
- aplikasi hanya melakukan hal-hal dasar (tanpa telemetri, tanpa unggahan diagnostik, sama sekali tanpa akses jaringan);
- tidak ada kode lisensi, aktivasi, atau uji coba.

Jika Anda hanya ingin pengalaman terbaik sebagai pengguna, pasang versi resmi dari [ntfstool.com](https://ntfstool.com).
NTFSTool Pro (sumber tertutup) menambahkan driver kedua untuk versi macOS yang lebih lama, perbaikan disk, dan alat lainnya.

## Fitur

- Membaca **dan menulis** volume NTFS; macOS memasangnya secara otomatis melalui modul driver FSKit
- Aplikasi disandbox; driver berjalan sebagai modul FSKit yang disandbox di ruang pengguna
- Menampilkan status tiap volume NTFS: baca-tulis, hanya baca (Windows hibernasi / Fast Startup), hanya baca oleh
  driver bawaan Apple, tidak terpasang
- Memandu Anda mengaktifkan ekstensi sistem berkas di Pengaturan Sistem
- Buka di Finder, keluarkan, informasi disk
- 20 bahasa antarmuka
- Atribut yang diperluas disimpan sebagai aliran bernama NTFS (tanpa berkas `._`); opsi pemasangan `windows_names` yang opsional
  menolak nama berkas yang tidak dapat digunakan Windows

## Persyaratan

- macOS 15.6 atau lebih baru untuk menjalankan; **dukungan tulis memerlukan macOS 26 atau lebih baru** (di macOS 15 sistem tetap memakai
  driver NTFS baca-saja bawaan Apple untuk disk fisik)
- Untuk membangun: Xcode 16.3 atau lebih baru (FSKit SDK) beserta command line tools-nya, Swift 5.9+

## Build — satu perintah

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Target lainnya:

| Perintah | Fungsinya |
| --- | --- |
| `make run` | membangun dan membuka aplikasi |
| `make demo` | menjalankan UI dengan disk demo di memori — tanpa driver dan tanpa akun Apple Developer |
| `make test` | menjalankan unit test |
| `make app UNIVERSAL=1` | membangun arm64 + x86_64 |
| `make scan` | memeriksa bahwa tidak ada materi penandatanganan atau pengenal pribadi di dalam pohon berkas |

Pengujian driver memformat dan memeriksa image disk NTFS sungguhan dengan subperintah `mkntfs` dan `ntfsfix` dari biner `ntfstool-driver`. Pengujian dilewati
kecuali kedua alat ditemukan di `bin`/`sbin` Homebrew atau di direktori yang diberikan oleh `NTFSTOOL_NTFS_TOOLS=<dir>`.

## Menjalankan driver (memerlukan akun Apple Developer Anda sendiri)

Modul FSKit menggunakan entitlement terbatas `com.apple.developer.fskit.fsmodule`. macOS hanya memuat modul
yang tanda tangannya didukung oleh provisioning profile yang memberikannya. Build ad-hoc menampilkan seluruh UI, tetapi
sistem tidak akan memuat driver. Ini adalah aturan platform Apple yang berlaku untuk setiap proyek FSKit.

1. Di akun Apple Developer Anda buat dua App ID, misalnya `com.yourname.ntfs` dan `com.yourname.ntfs.fskit`.
   Aktifkan kapabilitas **FSKit Module** pada yang kedua, dan secara opsional sebuah App Group
   (misalnya `TEAMID.com.yourname.ntfs`) pada keduanya agar aplikasi dapat membaca log driver.
2. Buat provisioning profile pengembangan (atau Developer ID) untuk keduanya dan unduh.
3. Bangun dengan identitas Anda:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Salin `build/NTFSTool.app` ke `/Applications`, buka sekali, lalu aktifkan driver di
   **Pengaturan Sistem → Umum → Item Login & Ekstensi → Ekstensi Sistem Berkas**.
5. Colokkan ulang drive NTFS. `mount` sekarang seharusnya menampilkannya dengan tipe `ntfstooloss`.

Semua opsi build didokumentasikan di bagian atas [`scripts/build-app.sh`](../../scripts/build-app.sh).
Provisioning profile dan kunci penandatanganan tidak boleh berada di repositori ini; `make scan` memeriksanya.

## Struktur

| Jalur | Isinya |
| --- | --- |
| `Sources/NTFSTool` | Titik masuk aplikasi (`main.swift`) |
| `Sources/NTFSToolApp` | Shell AppKit, tampilan SwiftUI, model aplikasi, layanan sistem, 20 lokalisasi |
| `Sources/NTFSToolCore` | Logika murni tanpa AppKit: model disk, daftar bahasa, penyimpanan pengaturan, log peristiwa lokal |
| `Sources/NTFSToolBridge` | Jembatan C (`ntfstool_bridge.c`) yang mengekspos operasi berbasis inode di atas `libntfstool-driver` |
| `Sources/NTFSToolDriver` | Inti Swift dari driver (tanpa dependensi FSKit, dapat diuji unit di macOS mana pun) |
| `FSKitModule/NTFSToolFS` | Modul FSKit (`NTFSToolFS.appex`) yang dimuat macOS untuk memasang volume NTFS |
| `Tests/` | Unit test untuk model aplikasi dan driver (image NTFS sungguhan) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

Lihat [docs/ARCHITECTURE.md](../ARCHITECTURE.md) untuk mengetahui bagaimana sebuah penulisan berjalan dari Finder ke disk.

## Berkontribusi

Issue dan pull request sangat diterima. Mohon tanda tangani commit Anda (`git commit -s`) untuk menyatakan
[Developer Certificate of Origin](https://developercertificate.org/) — lihat [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Lisensi


## Merek dagang

**Mohon jangan menerbitkan aplikasi ini, atau fork-nya, di Mac App Store** — ini akan bersaing dengan versi gratis resmi dan membingungkan pengguna.
