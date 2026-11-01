# NTFSTool (Open-Source-Edition)

Kostenloser, quelloffener Lese- und Schreibzugriff auf NTFS für macOS, gebaut auf Apples **FSKit** und der eigenen NTFS-Engine von NTFSTool (**libntfstool-driver**).

Schließen Sie ein Windows-Laufwerk (NTFS) an, und macOS bindet es über das FSKit-Treibermodul von NTFSTool beschreibbar ein – ohne Kernel-
Erweiterung, ohne macFUSE, ohne abgesenkte Sicherheitseinstellungen, ohne Administratorpasswort. Die App selbst läuft vollständig in der Sandbox.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · **Deutsch** · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

> **Auf der Suche nach der alten Version?** Die früheren Electron/Fuse-Versionen (2.x – 4.7.x) samt vollständiger Historie sind im [`V3`-Branch](https://github.com/ntfstool/ntfstool/tree/V3) archiviert. Sie werden nicht mehr gepflegt; `master` ist die aktuelle FSKit-basierte Ausgabe.

[Architekturleitfaden](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="NTFSTool-Hauptfenster: ein lesbares und beschreibbares NTFS-Laufwerk mit Kapazitätsbalken und Details" width="760">
</p>

<p align="center"><i>Wenn Ihnen dieses Projekt nützlich oder interessant erscheint, hilft ein ⭐ auf GitHub anderen, es zu finden.</i></p>

## Screenshots

Echte Screenshots der App, die aus diesem Repository gebaut wurde (`make app`). Die gezeigten Laufwerke sind die Beispiellaufwerke
des integrierten Demomodus (`make demo`), sodass Sie jede Ansicht auch ohne physische Festplatte ausprobieren können.

### Hauptfenster

Ein Fenster für alle Ihre NTFS-Laufwerke. Jedes Volume zeigt seinen Zustand auf einen Blick – **Lesen & Schreiben**, **Schreibgeschützt** oder
**Nicht eingebunden** – mit Kapazitätsbalken, Einhängepunkt, Bus (USB / Thunderbolt) und Gerätedetails sowie *Im
Finder anzeigen* und *Auswerfen*.

![Hauptfenster: ein beschreibbares USB-Laufwerk](../screenshots/overview.png)

| Eine Thunderbolt-SSD, lesen & schreiben | Ein nicht eingebundenes Laufwerk |
| --- | --- |
| ![Thunderbolt drive](../screenshots/thunderbolt-drive.png) | ![Not mounted drive](../screenshots/not-mounted.png) |

### Geführte Einrichtung: kein Passwort, keine Kernel-Erweiterung

NTFSTool ist ein gewöhnliches FSKit-Dateisystemmodul. Das Einzige, was macOS verlangt, ist ein Schalter unter
**Systemeinstellungen → Allgemein → Anmeldeobjekte & Erweiterungen → Dateisystemerweiterungen**. Die App erklärt es Schritt für
Schritt und bemerkt die Änderung selbstständig.

![Dateisystemerweiterung aktivieren](../screenshots/enable-extension.png)

### Standardmäßig sicher

Wenn Windows in den Ruhezustand versetzt wurde oder der *Schnellstart* aktiviert ist, würde Schreiben die Festplatte beschädigen. NTFSTool bindet ein solches Laufwerk
**schreibgeschützt** ein und sagt Ihnen genau, wie Sie das beheben, anstatt Ihre Daten zu riskieren.

| Von Windows im Ruhezustand zurückgelassenes Laufwerk | Was die App Ihnen mitteilt |
| --- | --- |
| ![Hibernated drive](../screenshots/hibernated-windows.png) | ![Shut Down Windows Completely](../screenshots/dialog-shut-down-windows.png) |

### Laufwerk erneut anschließen

macOS lässt das Einbinden physischer Laufwerke durch das System nur beim Anstecken zu. Deshalb wirft die App das Laufwerk sicher aus und
bittet Sie, es wieder anzuschließen; anschließend wird es les- und schreibbar eingebunden.

![Laufwerk erneut anschließen](../screenshots/dialog-plug-in-again.png)

### Dunkelmodus

| Hauptfenster | Erweiterungsanleitung |
| --- | --- |
| ![Dark mode, main window](../screenshots/dark-overview.png) | ![Dark mode, extension guide](../screenshots/dark-enable-extension.png) |

### Kompaktes Layout, leerer Zustand und „Über“

| Seitenleiste ausgeblendet | Warten auf ein Laufwerk |
| --- | --- |
| ![Sidebar hidden](../screenshots/sidebar-hidden.png) | ![No drive connected](../screenshots/no-drive.png) |

![Über NTFSTool](../screenshots/about.png)

Die Oberfläche ist in 20 Sprachen verfügbar, einschließlich Rechts-nach-links-Layouts für Arabisch und Persisch: English,
简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی,
Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย und Tiếng Việt.

## Was dieses Repository ist

Dies ist die **quelloffene Lern- und Referenzedition** von NTFSTool. Sie leitet sich vom kostenlosen NTFSTool im
Mac App Store ab und behält dieselbe Architektur bei, ist aber bewusst klein und gut lesbar gehalten:

- Jede Quelldatei ist auf Englisch kommentiert, sodass Sie nachvollziehen können, wie ein FSKit-Dateisystemmodul, eine C-Brücke auf der
  NTFS-Engine und ein Sandbox-Frontend in SwiftUI/AppKit zusammenspielen;
- der Treiber nutzt den einfachen, direkten E/A-Pfad (keine Cache-Schicht und kein plattformspezifisches Performance-Tuning);
- die App beschränkt sich auf das Wesentliche (keine Telemetrie, kein Hochladen von Diagnosedaten, überhaupt kein Netzwerkzugriff);
- es gibt keinen Code für Lizenzierung, Aktivierung oder Testversionen.

Wenn Sie als Nutzer einfach das beste Erlebnis wünschen, installieren Sie die offiziellen Builds von [ntfstool.com](https://ntfstool.com).
NTFSTool Pro (Closed Source) bringt zusätzlich einen zweiten Treiber für ältere macOS-Versionen, Datenträgerreparatur und weitere Werkzeuge mit.

## Funktionen

- NTFS-Volumes lesen **und schreiben**; macOS bindet sie automatisch über das FSKit-Treibermodul ein
- App in der Sandbox; der Treiber läuft als FSKit-Modul in der Sandbox im Benutzerbereich
- Zeigt den Zustand jedes NTFS-Volumes an: les- und schreibbar, schreibgeschützt (Windows im Ruhezustand / Schnellstart),
  schreibgeschützt durch Apples integrierten Treiber, nicht eingebunden
- Führt Sie dabei, die Dateisystemerweiterung in den Systemeinstellungen zu aktivieren
- Im Finder öffnen, auswerfen, Laufwerksinformationen
- 20 Oberflächensprachen
- Erweiterte Attribute werden als benannte NTFS-Streams gespeichert (keine `._`-Dateien); die optionale Mount-Option `windows_names`
  lehnt Dateinamen ab, die Windows nicht verwenden kann

## Voraussetzungen

- macOS 15.6 oder neuer zum Ausführen; **Schreibunterstützung erfordert macOS 26 oder neuer** (unter macOS 15 verwendet das System für physische
  Datenträger weiterhin Apples schreibgeschützten NTFS-Treiber)
- Zum Bauen: Xcode 16.3 oder neuer (FSKit SDK) mit den Befehlszeilenwerkzeugen, Swift 5.9+

## Bauen – ein Befehl

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Weitere Ziele:

| Befehl | Was er bewirkt |
| --- | --- |
| `make run` | baut die App und öffnet sie |
| `make demo` | startet die Oberfläche mit Demo-Datenträgern im Arbeitsspeicher – kein Treiber und kein Apple-Developer-Konto nötig |
| `make test` | führt die Unit-Tests aus |
| `make app UNIVERSAL=1` | baut arm64 + x86_64 |
| `make scan` | prüft, dass sich kein Signaturmaterial und keine privaten Kennungen im Verzeichnisbaum befinden |

Die Treibertests formatieren und prüfen echte NTFS-Datenträgerabbilder mit den Unterbefehlen `mkntfs` und `ntfsfix` des Binärprogramms `ntfstool-driver`. Sie werden übersprungen,
sofern nicht beide Werkzeuge in Homebrews `bin`/`sbin` oder in dem mit `NTFSTOOL_NTFS_TOOLS=<dir>` angegebenen Verzeichnis gefunden werden.

## Den Treiber ausführen (benötigt Ihr eigenes Apple-Developer-Konto)

Das FSKit-Modul verwendet die eingeschränkte Berechtigung `com.apple.developer.fskit.fsmodule`. macOS lädt nur Module,
deren Signatur durch ein Provisioning-Profil gedeckt ist, das sie gewährt. Ein Ad-hoc-Build zeigt die vollständige Oberfläche, aber das
System lädt den Treiber nicht. Das ist eine Regel der Apple-Plattform, die für jedes FSKit-Projekt gilt.

1. Legen Sie in Ihrem Apple-Developer-Konto zwei App-IDs an, z. B. `com.yourname.ntfs` und `com.yourname.ntfs.fskit`.
   Aktivieren Sie bei der zweiten die Capability **FSKit Module** und optional bei beiden eine App Group
   (z. B. `TEAMID.com.yourname.ntfs`), damit die App das Protokoll des Treibers lesen kann.
2. Erstellen Sie Entwicklungs- (oder Developer-ID-)Provisioning-Profile für beide und laden Sie sie herunter.
3. Bauen Sie mit Ihrer Identität:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Kopieren Sie `build/NTFSTool.app` nach `/Applications`, öffnen Sie die App einmal und aktivieren Sie dann den Treiber unter
   **Systemeinstellungen → Allgemein → Anmeldeobjekte & Erweiterungen → Dateisystemerweiterungen**.
5. Schließen Sie das NTFS-Laufwerk erneut an. `mount` sollte es nun mit dem Typ `ntfstooloss` auflisten.

Alle Build-Optionen sind am Anfang von [`scripts/build-app.sh`](../../scripts/build-app.sh) dokumentiert.
Provisioning-Profile und Signaturschlüssel gehören nie in dieses Repository; `make scan` prüft das.

## Aufbau

| Pfad | Was es ist |
| --- | --- |
| `Sources/NTFSTool` | Einstiegspunkt der App (`main.swift`) |
| `Sources/NTFSToolApp` | AppKit-Hülle, SwiftUI-Ansichten, das App-Modell, Systemdienste, 20 Lokalisierungen |
| `Sources/NTFSToolCore` | Reine Logik ohne AppKit: Datenträgermodell, Sprachliste, Einstellungsspeicher, lokales Ereignisprotokoll |
| `Sources/NTFSToolBridge` | C-Brücke (`ntfstool_bridge.c`), die inode-basierte Operationen auf Basis von `libntfstool-driver` bereitstellt |
| `Sources/NTFSToolDriver` | Swift-Kern des Treibers (ohne FSKit-Abhängigkeit, auf jedem macOS unit-testbar) |
| `FSKitModule/NTFSToolFS` | Das FSKit-Modul (`NTFSToolFS.appex`), das macOS zum Einbinden von NTFS-Volumes lädt |
| `Tests/` | Unit-Tests für das App-Modell und für den Treiber (echte NTFS-Abbilder) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

In [docs/ARCHITECTURE.md](../ARCHITECTURE.md) steht, wie ein Schreibvorgang vom Finder bis auf den Datenträger gelangt.

## Mitwirken

Issues und Pull Requests sind willkommen. Bitte signieren Sie Ihre Commits (`git commit -s`), um das
[Developer Certificate of Origin](https://developercertificate.org/) zu bestätigen – siehe [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Lizenz

MIT-Lizenz – siehe [LICENSE](../../LICENSE) und [NOTICE](../../NOTICE).

## Marken

**Bitte veröffentlichen Sie diese App oder einen Fork davon nicht im Mac App Store** – das würde mit der offiziellen kostenlosen Version konkurrieren und Nutzer verwirren.
