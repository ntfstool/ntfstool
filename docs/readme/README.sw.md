# NTFSTool (toleo la chanzo huria)

NTFS ya kusoma na kuandika, bila malipo na ya chanzo huria kwa macOS, iliyojengwa juu ya **FSKit** ya Apple na injini ya NTFS ya NTFSTool yenyewe (**libntfstool-driver**).

Chomeka diski ya Windows (NTFS) na macOS inaiunganisha ikiwa na uwezo wa kuandika kupitia moduli ya kiendeshi cha FSKit ya NTFSTool — bila kernel
extension, bila macFUSE, bila kupunguza mipangilio ya usalama, bila nenosiri la msimamizi. Programu yenyewe inaendeshwa ndani ya sandbox kikamilifu.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · **Kiswahili** · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

> **Unatafuta toleo la zamani?** Matoleo ya awali yanayotegemea Electron/Fuse (2.x – 4.7.x) pamoja na historia yake yote yamehifadhiwa kwenye [tawi la `V3`](https://github.com/ntfstool/ntfstool/tree/V3). Hayatunzwi tena; `master` ni toleo la sasa linalotegemea FSKit.

[Mwongozo wa usanifu](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="Dirisha kuu la NTFSTool: diski ya NTFS ya kusoma na kuandika yenye upau wa uwezo na maelezo" width="760">
</p>

<p align="center"><i>Ikiwa mradi huu ni wa msaada au wa kuvutia kwako, ⭐ kwenye GitHub husaidia wengine kuugundua.</i></p>

## Picha za skrini

Picha halisi za skrini za programu iliyojengwa kutoka hazina hii (`make app`). Diski zinazoonyeshwa ni diski za mfano za
hali ya onyesho iliyojengewa ndani (`make demo`), kwa hivyo unaweza kujaribu kila skrini bila diski halisi.

### Dirisha kuu

Dirisha moja kwa diski zako zote za NTFS. Kila kiasi kinaonyesha hali yake kwa haraka — **Kusoma na Kuandika**, **Kusoma Tu** au
**Haijaunganishwa** — pamoja na upau wa uwezo, mahali pa kuunganisha, basi (USB / Thunderbolt) na maelezo ya kifaa, pamoja na *Onyesha
kwenye Finder* na *Toa*.

![Dirisha kuu: diski ya USB ya kusoma na kuandika](../screenshots/overview.png)

| SSD ya Thunderbolt, kusoma na kuandika | Diski ambayo haijaunganishwa |
| --- | --- |
| ![Diski ya Thunderbolt](../screenshots/thunderbolt-drive.png) | ![Diski isiyounganishwa](../screenshots/not-mounted.png) |

### Usanidi wa kuongozwa: bila nenosiri, bila kernel extension

NTFSTool ni moduli ya kawaida ya mfumo wa faili wa FSKit. Kitu pekee ambacho macOS inaomba ni swichi moja katika
**Mipangilio ya Mfumo → Jumla → Vipengee vya Kuingia na Viendelezi → Viendelezi vya Mfumo wa Faili**. Programu inaelezea hatua kwa
hatua na inatambua mabadiliko yenyewe.

![Washa kiendelezi cha mfumo wa faili](../screenshots/enable-extension.png)

### Salama kwa chaguo-msingi

Ikiwa Windows ilikuwa imelazwa (hibernated) au *Fast Startup* imewashwa, kuandika kungeharibu diski. NTFSTool huunganisha diski kama hiyo
kwa **kusoma tu** na hukuambia hasa jinsi ya kulirekebisha, badala ya kuhatarisha data yako.

| Diski iliyoachwa katika hali ya hibernate na Windows | Programu inachokuambia |
| --- | --- |
| ![Diski iliyo katika hibernate](../screenshots/hibernated-windows.png) | ![Zima Windows Kabisa](../screenshots/dialog-shut-down-windows.png) |

### Kuchomeka diski tena

macOS huruhusu mfumo kuunganisha diski halisi tu zinapochomekwa, kwa hivyo programu hutoa diski kwa usalama na
hukuomba uichomeke tena; kisha huunganishwa kwa kusoma na kuandika.

![Chomeka diski tena](../screenshots/dialog-plug-in-again.png)

### Hali ya giza

| Dirisha kuu | Mwongozo wa kiendelezi |
| --- | --- |
| ![Hali ya giza, dirisha kuu](../screenshots/dark-overview.png) | ![Hali ya giza, mwongozo wa kiendelezi](../screenshots/dark-enable-extension.png) |

### Mpangilio mfupi, hali tupu na Kuhusu

| Utepe wa pembeni umefichwa | Inasubiri diski |
| --- | --- |
| ![Utepe wa pembeni umefichwa](../screenshots/sidebar-hidden.png) | ![Hakuna diski iliyounganishwa](../screenshots/no-drive.png) |

![Kuhusu NTFSTool](../screenshots/about.png)

Kiolesura kinapatikana katika lugha 20, ikiwa ni pamoja na mipangilio ya kulia-kwenda-kushoto kwa Kiarabu na Kiajemi: English,
简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی,
Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย na Tiếng Việt.

## Hazina hii ni nini

Hili ni **toleo la chanzo huria la kujifunzia / rejea** la NTFSTool. Limetokana na NTFSTool ya bure kwenye
Mac App Store na linadumisha usanifu ule ule, lakini limekusudiwa kuwa dogo na rahisi kusomeka:

- kila faili la chanzo lina maelezo kwa Kiingereza, ili uweze kujifunza jinsi moduli ya mfumo wa faili wa FSKit, daraja la C juu ya
  injini ya NTFS, na sehemu ya mbele ya SwiftUI/AppKit iliyo ndani ya sandbox zinavyofanya kazi pamoja;
- kiendeshi kinatumia njia rahisi na ya moja kwa moja ya I/O (hakuna safu ya akiba wala urekebishaji wa utendaji maalum wa jukwaa);
- programu inafanya mambo ya msingi tu (hakuna telemetry, hakuna upakiaji wa uchunguzi, hakuna ufikiaji wa mtandao kabisa);
- hakuna msimbo wa leseni, uwezeshaji au kipindi cha majaribio.

Ikiwa unataka tu uzoefu bora kama mtumiaji, sakinisha matoleo rasmi kutoka [ntfstool.com](https://ntfstool.com).
NTFSTool Pro (chanzo kilichofungwa) inaongeza kiendeshi cha pili kwa matoleo ya zamani ya macOS, ukarabati wa diski na zana nyingine.

## Vipengele

- Soma **na andika** viasi vya NTFS; macOS huviunganisha kiotomatiki kupitia moduli ya kiendeshi cha FSKit
- Programu iko ndani ya sandbox; kiendeshi kinaendeshwa kama moduli ya FSKit iliyo ndani ya sandbox katika nafasi ya mtumiaji
- Inaonyesha hali ya kila kiasi cha NTFS: kusoma na kuandika, kusoma tu (Windows imelazwa / Fast Startup), kusoma tu na
  kiendeshi kilichojengewa ndani cha Apple, haijaunganishwa
- Inakuongoza kuwasha kiendelezi cha mfumo wa faili katika Mipangilio ya Mfumo
- Fungua katika Finder, toa, taarifa za diski
- Lugha 20 za kiolesura
- Sifa zilizopanuliwa huhifadhiwa kama mitiririko iliyopewa majina ya NTFS (hakuna faili za `._`); chaguo la hiari la kuunganisha `windows_names`
  hukataa majina ya faili ambayo Windows haiwezi kutumia

## Mahitaji

- macOS 15.6 au mpya zaidi ili kuendesha; **usaidizi wa kuandika unahitaji macOS 26 au mpya zaidi** (kwenye macOS 15 mfumo unaendelea kutumia
  kiendeshi cha NTFS cha kusoma tu cha Apple kwa diski halisi)
- Ili kujenga: Xcode 16.3 au mpya zaidi (FSKit SDK) pamoja na zana zake za mstari wa amri, Swift 5.9+

## Jenga — amri moja

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Malengo mengine:

| Amri | Inafanya nini |
| --- | --- |
| `make run` | hujenga na kufungua programu |
| `make demo` | huendesha UI kwa diski za onyesho zilizo kwenye kumbukumbu — bila kiendeshi na bila akaunti ya Apple Developer |
| `make test` | huendesha majaribio ya kitengo |
| `make app UNIVERSAL=1` | hujenga arm64 + x86_64 |
| `make scan` | hukagua kuwa hakuna nyenzo za kusaini wala vitambulisho vya siri kwenye mti wa faili |

Majaribio ya kiendeshi hupanga na kukagua picha halisi za diski za NTFS kwa kutumia amri ndogo `mkntfs` na `ntfsfix` za faili la binary `ntfstool-driver`. Hurukwa
isipokuwa zana zote mbili zimepatikana katika `bin`/`sbin` ya Homebrew au katika saraka iliyotolewa na `NTFSTOOL_NTFS_TOOLS=<dir>`.

## Kuendesha kiendeshi (kunahitaji akaunti yako mwenyewe ya Apple Developer)

Moduli ya FSKit inatumia entitlement yenye vikwazo `com.apple.developer.fskit.fsmodule`. macOS hupakia moduli tu
ambayo sahihi yake inaungwa mkono na provisioning profile inayoitoa. Muundo wa ad-hoc huonyesha UI kamili, lakini
mfumo hautapakia kiendeshi. Hii ni kanuni ya jukwaa la Apple inayotumika kwa kila mradi wa FSKit.

1. Katika akaunti yako ya Apple Developer unda App ID mbili, k.m. `com.yourname.ntfs` na `com.yourname.ntfs.fskit`.
   Washa uwezo wa **FSKit Module** kwenye ya pili, na kwa hiari App Group
   (k.m. `TEAMID.com.yourname.ntfs`) kwenye zote mbili ili programu iweze kusoma kumbukumbu ya kiendeshi.
2. Unda provisioning profile za uundaji (au Developer ID) kwa zote mbili na uzipakue.
3. Jenga kwa kutumia utambulisho wako:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Nakili `build/NTFSTool.app` kwenye `/Applications`, ifungue mara moja, kisha washa kiendeshi katika
   **Mipangilio ya Mfumo → Jumla → Vipengee vya Kuingia na Viendelezi → Viendelezi vya Mfumo wa Faili**.
5. Chomeka tena diski ya NTFS. `mount` sasa inapaswa kuiorodhesha ikiwa na aina `ntfstooloss`.

Chaguo zote za ujenzi zimeandikwa juu ya [`scripts/build-app.sh`](../../scripts/build-app.sh).
Provisioning profile na funguo za kusaini hazipaswi kamwe kuwa katika hazina hii; `make scan` hukagua hilo.

## Mpangilio

| Njia | Ni nini |
| --- | --- |
| `Sources/NTFSTool` | Sehemu ya kuingilia ya programu (`main.swift`) |
| `Sources/NTFSToolApp` | Ganda la AppKit, mionekano ya SwiftUI, modeli ya programu, huduma za mfumo, tafsiri 20 |
| `Sources/NTFSToolCore` | Mantiki safi bila AppKit: modeli ya diski, orodha ya lugha, hifadhi ya mipangilio, kumbukumbu ya matukio ya ndani |
| `Sources/NTFSToolBridge` | Daraja la C (`ntfstool_bridge.c`) linalotoa operesheni zinazotegemea inode juu ya `libntfstool-driver` |
| `Sources/NTFSToolDriver` | Kiini cha Swift cha kiendeshi (hakina utegemezi wa FSKit, kinaweza kujaribiwa kwa kitengo kwenye macOS yoyote) |
| `FSKitModule/NTFSToolFS` | Moduli ya FSKit (`NTFSToolFS.appex`) ambayo macOS hupakia ili kuunganisha viasi vya NTFS |
| `Tests/` | Majaribio ya kitengo ya modeli ya programu na ya kiendeshi (picha halisi za NTFS) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

Tazama [docs/ARCHITECTURE.md](../ARCHITECTURE.md) kuona jinsi uandishi unavyosafiri kutoka Finder hadi diskini.

## Kuchangia

Issue na pull request zinakaribishwa. Tafadhali tia sahihi commit zako (`git commit -s`) ili kuthibitisha
[Developer Certificate of Origin](https://developercertificate.org/) — tazama [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Leseni


## Alama za biashara

**Tafadhali usichapishe programu hii, wala fork yake, kwenye Mac App Store** — itashindana na toleo rasmi lisilolipiwa na kuwachanganya watumiaji.
