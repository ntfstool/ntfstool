# NTFSTool (açık kaynak sürümü)

macOS için ücretsiz, açık kaynaklı, okuma-yazma destekli NTFS; Apple'ın **FSKit**'i
ve NTFSTool'un kendi NTFS motoru (**libntfstool-driver**) üzerine kurulu.

Bir Windows (NTFS) sürücüsünü takın; macOS onu NTFSTool'un FSKit sürücü modülü üzerinden yazılabilir olarak bağlasın — çekirdek
uzantısı yok, macFUSE yok, düşürülmüş güvenlik ayarı yok, yönetici parolası
yok. Uygulamanın kendisi tamamen korumalı alanda (sandbox) çalışır.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · **Türkçe** · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

> **Eski sürümü mü arıyorsunuz?** Önceki Electron/Fuse tabanlı sürümler (2.x – 4.7.x) ve tüm geçmişleri [`V3` dalında](https://github.com/ntfstool/ntfstool/tree/V3) arşivlenmiştir; artık bakımı yapılmıyor. `master`, FSKit tabanlı güncel sürümdür.

[Mimari rehberi](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="NTFSTool ana penceresi: kapasite çubuğu ve ayrıntılarla okuma-yazma modunda bir NTFS sürücüsü" width="760">
</p>

<p align="center"><i>Bu proje size faydalı veya ilginç geldiyse, GitHub'da bir ⭐ başkalarının onu bulmasına yardımcı olur.</i></p>

## Ekran görüntüleri

Bu depodan derlenen uygulamanın gerçek ekran görüntüleri (`make app`). Gösterilen sürücüler, yerleşik demo
modunun (`make demo`) örnek sürücüleridir; böylece fiziksel bir disk olmadan her ekranı deneyebilirsiniz.

### Ana pencere

Tüm NTFS sürücüleriniz için tek pencere. Her birim durumunu tek bakışta gösterir — **Okuma ve Yazma**, **Salt Okunur** veya **Bağlı Değil** — kapasite çubuğu, bağlama noktası, veri yolu (USB / Thunderbolt) ve aygıt ayrıntılarıyla birlikte; ayrıca *Finder'da Göster* ve *Çıkar* seçenekleri.

![Ana pencere: okuma-yazma modunda bir USB sürücü](../screenshots/overview.png)

| Okuma ve yazma modunda bir Thunderbolt SSD | Bağlı olmayan bir sürücü |
| --- | --- |
| ![Thunderbolt sürücüsü](../screenshots/thunderbolt-drive.png) | ![Bağlı olmayan sürücü](../screenshots/not-mounted.png) |

### Rehberli kurulum: parola yok, çekirdek uzantısı yok

NTFSTool sıradan bir FSKit dosya sistemi modülüdür. macOS'in istediği tek şey,
**Sistem Ayarları → Genel → Giriş Öğeleri ve Uzantılar → Dosya Sistemi Uzantıları** altındaki tek bir anahtardır. Uygulama bunu adım adım
anlatır ve değişikliği kendiliğinden fark eder.

![Dosya sistemi uzantısını açın](../screenshots/enable-extension.png)

### Varsayılan olarak güvenli

Windows hazırda bekletme (hibernate) durumuna geçirdiyse veya *Hızlı Başlatma* (Fast Startup) açıksa, yazmak diski bozar. NTFSTool böyle bir sürücüyü
**salt okunur** olarak bağlar ve verilerinizi riske atmak yerine sorunu nasıl gidereceğinizi tam olarak söyler.

| Windows'un hazırda beklemede bıraktığı sürücü | Uygulamanın size söyledikleri |
| --- | --- |
| ![Hazırda bekletilmiş sürücü](../screenshots/hibernated-windows.png) | ![Windows'u tamamen kapatın](../screenshots/dialog-shut-down-windows.png) |

### Sürücüyü yeniden takma

macOS, fiziksel sürücüleri yalnızca takıldıkları anda sistemin bağlamasına izin verir; bu yüzden uygulama sürücüyü güvenle çıkarır
ve yeniden takmanızı ister; ardından sürücü okuma-yazma modunda bağlanır.

![Sürücüyü yeniden takın](../screenshots/dialog-plug-in-again.png)

### Koyu mod

| Ana pencere | Uzantı rehberi |
| --- | --- |
| ![Koyu mod, ana pencere](../screenshots/dark-overview.png) | ![Koyu mod, uzantı rehberi](../screenshots/dark-enable-extension.png) |

### Kompakt düzen, boş durum ve Hakkında

| Kenar çubuğu gizli | Sürücü bekleniyor |
| --- | --- |
| ![Kenar çubuğu gizli](../screenshots/sidebar-hidden.png) | ![Bağlı sürücü yok](../screenshots/no-drive.png) |

![NTFSTool Hakkında](../screenshots/about.png)

Arayüz, Arapça ve Farsça için sağdan sola düzenler dahil 20 dilde sunulur: English,
简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی,
Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย ve Tiếng Việt.

## Bu depo nedir

Bu, NTFSTool'un **açık kaynaklı öğrenme / referans sürümüdür**. Mac App Store'daki ücretsiz NTFSTool'dan türetilmiştir ve aynı mimariyi korur, ancak bilinçli olarak küçük ve okunabilir tutulmuştur:

- her kaynak dosya İngilizce yorumlanmıştır; böylece bir FSKit dosya sistemi modülünün, NTFS motorunun üstündeki bir C köprüsünün
  ve korumalı alandaki bir SwiftUI/AppKit ön yüzünün nasıl bir araya geldiğini inceleyebilirsiniz;
- sürücü basit, doğrudan G/Ç yolunu kullanır (önbellek katmanı veya platforma özgü performans ayarı yok);
- uygulama yalnızca temel işleri yapar (telemetri yok, tanılama yüklemesi yok, hiç ağ erişimi yok);
- lisanslama, etkinleştirme veya deneme sürümü kodu yok.

Kullanıcı olarak sadece en iyi deneyimi istiyorsanız, resmî sürümleri [ntfstool.com](https://ntfstool.com) adresinden yükleyin.
NTFSTool Pro (kapalı kaynak), eski macOS sürümleri için ikinci bir sürücü, disk onarımı ve diğer araçları ekler.

## Özellikler

- NTFS birimlerini okuyun **ve yazın**; macOS bunları FSKit sürücü modülü üzerinden otomatik olarak bağlar
- Korumalı alandaki uygulama; sürücü, kullanıcı alanında korumalı alandaki bir FSKit modülü olarak çalışır
- Her NTFS biriminin durumunu gösterir: okuma-yazma, salt okunur (Windows hazırda beklemede / Hızlı Başlatma), Apple'ın yerleşik
  sürücüsü tarafından salt okunur, bağlı değil
- Dosya sistemi uzantısını Sistem Ayarları'nda açmanız için size yol gösterir
- Finder'da aç, çıkar, disk bilgisi
- 20 arayüz dili
- Genişletilmiş öznitelikler adlandırılmış NTFS akışları olarak saklanır (`._` dosyaları yok); isteğe bağlı `windows_names` bağlama seçeneği
  Windows'un kullanamayacağı dosya adlarını reddeder

## Gereksinimler

- Çalıştırmak için macOS 15.6 veya üzeri; **yazma desteği macOS 26 veya üzeri gerektirir** (macOS 15'te sistem fiziksel diskler için
  Apple'ın salt okunur NTFS sürücüsünü kullanmaya devam eder)
- Derlemek için: Xcode 16.3 veya üzeri (FSKit SDK) ve komut satırı araçları, Swift 5.9+

## Derleme — tek komut

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Diğer hedefler:

| Komut | Ne yapar |
| --- | --- |
| `make run` | uygulamayı derler ve açar |
| `make demo` | arayüzü bellek içi demo disklerle çalıştırır — sürücü ve Apple Developer hesabı gerekmez |
| `make test` | birim testlerini çalıştırır |
| `make app UNIVERSAL=1` | arm64 + x86_64 derler |
| `make scan` | ağaçta imzalama materyali veya özel tanımlayıcı olmadığını denetler |

Sürücü testleri, `ntfstool-driver` ikili dosyasının `mkntfs` ve `ntfsfix` alt komutlarıyla gerçek NTFS disk imajlarını biçimlendirir ve denetler. Her iki araç da Homebrew'un `bin`/`sbin` dizininde
veya `NTFSTOOL_NTFS_TOOLS=<dir>` ile verilen dizinde bulunmadıkça atlanır.

## Sürücüyü çalıştırma (kendi Apple Developer hesabınız gerekir)

FSKit modülü, kısıtlı `com.apple.developer.fskit.fsmodule` yetkisini (entitlement) kullanır. macOS yalnızca imzası, bu yetkiyi veren
bir provisioning profile ile desteklenen modülü yükler. Ad-hoc bir derleme arayüzün tamamını gösterir, ancak
sistem sürücüyü yüklemez. Bu, her FSKit projesi için geçerli olan bir Apple platform kuralıdır.

1. Apple Developer hesabınızda iki App ID oluşturun, örn. `com.yourname.ntfs` ve `com.yourname.ntfs.fskit`.
   İkincisinde **FSKit Module** özelliğini, isteğe bağlı olarak her ikisinde de uygulamanın sürücü günlüğünü okuyabilmesi için bir App Group
   (örn. `TEAMID.com.yourname.ntfs`) etkinleştirin.
2. Her ikisi için geliştirme (veya Developer ID) provisioning profile'ları oluşturun ve indirin.
3. Kendi kimliğinizle derleyin:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. `build/NTFSTool.app` dosyasını `/Applications` içine kopyalayın, bir kez açın, ardından sürücüyü
   **Sistem Ayarları → Genel → Giriş Öğeleri ve Uzantılar → Dosya Sistemi Uzantıları** altında açın.
5. NTFS sürücüsünü yeniden takın. `mount` artık onu `ntfstooloss` türüyle listelemelidir.

Tüm derleme seçenekleri [`scripts/build-app.sh`](../../scripts/build-app.sh) dosyasının başında belgelenmiştir.
Provisioning profile'lar ve imzalama anahtarları asla bu depoda yer almamalıdır; `make scan` bunları denetler.

## Yapı

| Yol | Nedir |
| --- | --- |
| `Sources/NTFSTool` | Uygulama giriş noktası (`main.swift`) |
| `Sources/NTFSToolApp` | AppKit kabuğu, SwiftUI görünümleri, uygulama modeli, sistem hizmetleri, 20 yerelleştirme |
| `Sources/NTFSToolCore` | AppKit içermeyen saf mantık: disk modeli, dil listesi, ayar deposu, yerel olay günlüğü |
| `Sources/NTFSToolBridge` | `libntfstool-driver` üzerinde inode tabanlı işlemleri sunan C köprüsü (`ntfstool_bridge.c`) |
| `Sources/NTFSToolDriver` | Sürücünün Swift çekirdeği (FSKit bağımlılığı yok, her macOS'te birim testi yapılabilir) |
| `FSKitModule/NTFSToolFS` | macOS'in NTFS birimlerini bağlamak için yüklediği FSKit modülü (`NTFSToolFS.appex`) |
| `Tests/` | Uygulama modeli ve sürücü için birim testleri (gerçek NTFS imajlarıyla) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

Bir yazma işleminin Finder'dan diske nasıl ulaştığını görmek için [docs/ARCHITECTURE.md](../ARCHITECTURE.md) dosyasına bakın.

## Katkıda bulunma

Issue'lar ve pull request'ler memnuniyetle karşılanır. [Developer Certificate of Origin](https://developercertificate.org/) belgesini
onayladığınızı göstermek için lütfen commit'lerinizi imzalayın (`git commit -s`) — bkz. [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Lisans

MIT Lisansı — bkz. [LICENSE](../../LICENSE) ve [NOTICE](../../NOTICE).

## Ticari markalar

**Lütfen bu uygulamayı veya bir çatalını (fork) Mac App Store'da yayımlamayın** — resmî ücretsiz sürümle rekabet eder ve kullanıcıları karıştırır.
