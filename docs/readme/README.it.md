# NTFSTool (edizione open source)

NTFS in lettura e scrittura per macOS, gratuito e open source, basato su **FSKit** di Apple e sul motore NTFS di NTFSTool
(**libntfstool-driver**).

Collega un disco Windows (NTFS) e macOS lo monta in scrittura tramite il modulo driver FSKit di NTFSTool — senza estensioni del kernel,
senza macFUSE, senza ridurre le impostazioni di sicurezza, senza password di amministratore. L'app stessa è completamente in sandbox.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · **Italiano** · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

[Guida all'architettura](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="Finestra principale di NTFSTool: un disco NTFS in lettura e scrittura con barra della capacità e dettagli" width="760">
</p>

<p align="center"><i>Se questo progetto ti è utile o ti interessa, una ⭐ su GitHub aiuta altri a trovarlo.</i></p>

## Schermate

Schermate reali dell'app compilata da questo repository (`make app`). I dischi mostrati sono i dischi di esempio della modalità demo
integrata (`make demo`), così puoi provare ogni schermata senza un disco fisico.

### Finestra principale

Un'unica finestra per tutti i tuoi dischi NTFS. Ogni volume mostra il proprio stato a colpo d'occhio — **Lettura e scrittura**, **Sola
lettura** o **Non montato** — con una barra della capacità, il punto di mount, il bus (USB / Thunderbolt) e i dettagli del dispositivo,
oltre a *Mostra nel Finder* ed *Espelli*.

![Finestra principale: un disco USB in lettura e scrittura](../screenshots/overview.png)

| Un SSD Thunderbolt, lettura e scrittura | Un disco non montato |
| --- | --- |
| ![Disco Thunderbolt](../screenshots/thunderbolt-drive.png) | ![Disco non montato](../screenshots/not-mounted.png) |

### Configurazione guidata: senza password, senza estensioni del kernel

NTFSTool è un normale modulo di file system FSKit. L'unica cosa che macOS richiede è attivare un interruttore in **Impostazioni di Sistema →
Generali → Elementi login ed estensioni → Estensioni file system**. L'app lo spiega passo dopo passo e rileva da sola la modifica.

![Attiva l'estensione del file system](../screenshots/enable-extension.png)

### Sicuro per impostazione predefinita

Se Windows è stato ibernato o l'*avvio rapido* è attivo, scrivere danneggerebbe il disco. NTFSTool monta un disco di questo tipo in **sola
lettura** e ti spiega esattamente come risolvere, invece di rischiare i tuoi dati.

| Disco lasciato in ibernazione da Windows | Cosa ti dice l'app |
| --- | --- |
| ![Disco in ibernazione](../screenshots/hibernated-windows.png) | ![Arresta completamente Windows](../screenshots/dialog-shut-down-windows.png) |

### Ricollegare un disco

macOS lascia che il sistema monti i dischi fisici solo nel momento in cui vengono collegati, quindi l'app espelle il disco in sicurezza e ti
chiede di ricollegarlo; a quel punto viene montato in lettura e scrittura.

![Ricollega il disco](../screenshots/dialog-plug-in-again.png)

### Modalità scura

| Finestra principale | Guida all'estensione |
| --- | --- |
| ![Modalità scura, finestra principale](../screenshots/dark-overview.png) | ![Modalità scura, guida all'estensione](../screenshots/dark-enable-extension.png) |

### Layout compatto, stato vuoto e Informazioni

| Barra laterale nascosta | In attesa di un disco |
| --- | --- |
| ![Barra laterale nascosta](../screenshots/sidebar-hidden.png) | ![Nessun disco collegato](../screenshots/no-drive.png) |

![Informazioni su NTFSTool](../screenshots/about.png)

L'interfaccia è disponibile in 20 lingue, inclusi i layout da destra a sinistra per arabo e persiano: English, 简体中文, 繁體中文, 日本語, 한국어,
Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی, Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย e Tiếng
Việt.

## Che cos'è questo repository

Questa è l'**edizione open source didattica / di riferimento** di NTFSTool. Deriva dal NTFSTool gratuito del Mac App Store e mantiene la
stessa architettura, ma è volutamente tenuta piccola e leggibile:

- ogni file sorgente è commentato in inglese, così puoi studiare come si combinano un modulo di file system FSKit, un bridge C sopra il
  motore NTFS e un front end SwiftUI/AppKit in sandbox;
- il driver usa il percorso di I/O semplice e diretto (nessun livello di cache né ottimizzazioni delle prestazioni specifiche della piattaforma);
- l'app fa solo le cose essenziali (nessuna telemetria, nessun invio di diagnostica, nessun accesso alla rete);
- non c'è alcun codice di licenza, attivazione o prova.

Se vuoi semplicemente la migliore esperienza da utente, installa le versioni ufficiali da
[ntfstool.com](https://ntfstool.com). NTFSTool Pro (closed source) aggiunge un secondo driver per le versioni più vecchie di macOS, la
riparazione dei dischi e altri strumenti.

## Funzionalità

- Legge **e scrive** volumi NTFS; macOS li monta automaticamente tramite il modulo driver FSKit
- App in sandbox; il driver gira come modulo FSKit in sandbox nello spazio utente
- Mostra lo stato di ogni volume NTFS: lettura e scrittura, sola lettura (Windows in ibernazione / avvio rapido), sola lettura tramite il
  driver integrato di Apple, non montato
- Ti guida ad attivare l'estensione del file system in Impostazioni di Sistema
- Apri nel Finder, espelli, informazioni sul disco
- 20 lingue dell'interfaccia
- Attributi estesi memorizzati come stream con nome NTFS (nessun file `._`); l'opzione di mount facoltativa `windows_names` rifiuta i nomi
  di file che Windows non può usare

## Requisiti

- macOS 15.6 o successivo per l'esecuzione; **la scrittura richiede macOS 26 o successivo** (su macOS 15 il sistema continua a usare il
  driver NTFS di sola lettura di Apple per i dischi fisici)
- Per la compilazione: Xcode 16.3 o successivo (SDK FSKit) con i relativi strumenti da riga di comando, Swift 5.9+

## Compilazione — un solo comando

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Altri target:

| Comando | Cosa fa |
| --- | --- |
| `make run` | compila e apre l'app |
| `make demo` | avvia l'interfaccia con dischi dimostrativi in memoria — non servono driver né un account Apple Developer |
| `make test` | esegue i test unitari |
| `make app UNIVERSAL=1` | compila arm64 + x86_64 |
| `make scan` | verifica che nell'albero non ci siano materiale di firma né identificatori privati |

I test del driver formattano e controllano vere immagini disco NTFS con i sottocomandi `mkntfs` e `ntfsfix` del binario `ntfstool-driver`.
Vengono saltati a meno che entrambi gli strumenti si trovino in `bin`/`sbin` di Homebrew o nella directory indicata da
`NTFSTOOL_NTFS_TOOLS=<dir>`.

## Eseguire il driver (serve un tuo account Apple Developer)

Il modulo FSKit usa l'entitlement riservato `com.apple.developer.fskit.fsmodule`. macOS carica un modulo solo se la sua firma è supportata
da un provisioning profile che lo concede. Una build ad hoc mostra l'intera interfaccia, ma il sistema non caricherà il driver. È una regola
della piattaforma Apple che vale per ogni progetto FSKit.

1. Nel tuo account Apple Developer crea due App ID, ad es. `com.yourname.ntfs` e `com.yourname.ntfs.fskit`. Abilita la capability **FSKit
   Module** sul secondo e, facoltativamente, un App Group (ad es. `TEAMID.com.yourname.ntfs`) su entrambi, in modo che l'app possa leggere
   il log del driver.
2. Crea i provisioning profile di sviluppo (o Developer ID) per entrambi e scaricali.
3. Compila con la tua identità:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Copia `build/NTFSTool.app` in `/Applications`, aprila una volta, poi attiva il driver in **Impostazioni di Sistema → Generali → Elementi
   login ed estensioni → Estensioni file system**.
5. Ricollega il disco NTFS. Ora `mount` dovrebbe elencarlo con il tipo `ntfstooloss`.

Tutte le opzioni di compilazione sono documentate all'inizio di [`scripts/build-app.sh`](../../scripts/build-app.sh). I provisioning profile e le
chiavi di firma non devono mai finire in questo repository; `make scan` li cerca.

## Struttura

| Percorso | Che cos'è |
| --- | --- |
| `Sources/NTFSTool` | Punto di ingresso dell'app (`main.swift`) |
| `Sources/NTFSToolApp` | Livello AppKit, viste SwiftUI, modello dell'app, servizi di sistema, 20 localizzazioni |
| `Sources/NTFSToolCore` | Logica pura senza AppKit: modello dei dischi, elenco delle lingue, archivio delle impostazioni, registro eventi locale |
| `Sources/NTFSToolBridge` | Bridge C (`ntfstool_bridge.c`) che espone operazioni basate sugli inode sopra `libntfstool-driver` |
| `Sources/NTFSToolDriver` | Nucleo Swift del driver (nessuna dipendenza da FSKit, testabile con test unitari su qualsiasi macOS) |
| `FSKitModule/NTFSToolFS` | Il modulo FSKit (`NTFSToolFS.appex`) che macOS carica per montare i volumi NTFS |
| `Tests/` | Test unitari del modello dell'app e del driver (immagini NTFS reali) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

Consulta [docs/ARCHITECTURE.md](../ARCHITECTURE.md) per vedere come una scrittura viaggia dal Finder al disco.

## Contribuire

Issue e pull request sono benvenute. Firma i tuoi commit (`git commit -s`) per certificare il
[Developer Certificate of Origin](https://developercertificate.org/) — vedi [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Licenza

Licenza MIT — vedi [LICENSE](../../LICENSE) e [NOTICE](../../NOTICE). Il motore NTFS incluso è il `libntfstool-driver` di NTFSTool, distribuito come
libreria statica precompilata (vedi
[Vendor/README.md](../../Vendor/README.md)).

Le versioni precedenti di questo repository (l'app Electron) restano disponibili nella cronologia di git.

## Marchi

La licenza MIT ti dà il codice, non il marchio. «NTFSTool» e le icone e i loghi di NTFSTool sono marchi del progetto NTFSTool e **non** sono
concessi in licenza con la licenza MIT.

- Puoi compilare e usare questo codice sorgente per te stesso e citarlo come «basato su NTFSTool».
- Se **distribuisci** una versione modificata o un fork (anche su qualsiasi app store), devi dargli un nome e un'icona diversi, rimuovere i
  loghi di NTFSTool e usare un tuo bundle identifier — non deve sembrare una versione ufficiale di NTFSTool né dichiararsi tale.
- Le build non modificate di questo repository possono essere ridistribuite con il nome NTFSTool solo con autorizzazione preventiva.

NTFS e Windows sono marchi di Microsoft Corporation; macOS e FSKit sono marchi di Apple Inc. Questo progetto non è affiliato a Microsoft né
ad Apple.
