# NTFSTool (édition open source)

NTFS en lecture-écriture pour macOS, gratuit et open source, construit sur **FSKit** d'Apple et sur le moteur NTFS propre à NTFSTool (**libntfstool-driver**).

Branchez un disque Windows (NTFS) et macOS le monte en écriture grâce au module de pilote FSKit de NTFSTool — sans extension du noyau, sans macFUSE,
sans réduire les réglages de sécurité, sans mot de passe administrateur. L'application elle-même est entièrement sandboxée.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · **Français** · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

[Guide d'architecture](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="Fenêtre principale de NTFSTool : un disque NTFS en lecture-écriture avec barre de capacité et détails" width="760">
</p>

<p align="center"><i>Si ce projet vous est utile ou vous intéresse, une ⭐ sur GitHub aide d'autres personnes à le découvrir.</i></p>

## Captures d'écran

Captures d'écran réelles de l'application compilée à partir de ce dépôt (`make app`). Les disques affichés sont les disques d'exemple du mode démo
intégré (`make demo`) : vous pouvez ainsi essayer chaque écran sans disque physique.

### Fenêtre principale

Une seule fenêtre pour tous vos disques NTFS. Chaque volume affiche son état en un coup d'œil — **Lecture et écriture**, **Lecture seule** ou **Non
monté** — avec une barre de capacité, le point de montage, le bus (USB / Thunderbolt) et les détails de l'appareil, ainsi que *Afficher dans le Finder*
et *Éjecter*.

![Fenêtre principale : un disque USB en lecture-écriture](../screenshots/overview.png)

| Un SSD Thunderbolt, lecture et écriture | Un disque non monté |
| --- | --- |
| ![Disque Thunderbolt](../screenshots/thunderbolt-drive.png) | ![Disque non monté](../screenshots/not-mounted.png) |

### Configuration guidée : sans mot de passe, sans extension du noyau

NTFSTool est un module de système de fichiers FSKit ordinaire. La seule chose que macOS demande est d'activer un interrupteur dans **Réglages Système →
Général → Éléments de connexion et extensions → Extensions de système de fichiers**. L'application l'explique pas à pas et détecte d'elle-même le
changement.

![Activer l'extension de système de fichiers](../screenshots/enable-extension.png)

### Sûr par défaut

Si Windows a été mis en veille prolongée ou si le *démarrage rapide* est activé, l'écriture corromprait le disque. NTFSTool monte un tel disque en
**lecture seule** et vous indique précisément comment corriger le problème, plutôt que de risquer vos données.

| Disque laissé en veille prolongée par Windows | Ce que l'application vous indique |
| --- | --- |
| ![Disque en veille prolongée](../screenshots/hibernated-windows.png) | ![Arrêter complètement Windows](../screenshots/dialog-shut-down-windows.png) |

### Rebrancher un disque

macOS ne laisse le système monter les disques physiques qu'au moment où ils sont branchés ; l'application éjecte donc le disque en toute sécurité et
vous demande de le rebrancher, après quoi il est monté en lecture-écriture.

![Rebranchez le disque](../screenshots/dialog-plug-in-again.png)

### Mode sombre

| Fenêtre principale | Guide de l'extension |
| --- | --- |
| ![Mode sombre, fenêtre principale](../screenshots/dark-overview.png) | ![Mode sombre, guide de l'extension](../screenshots/dark-enable-extension.png) |

### Affichage compact, état vide et À propos

| Barre latérale masquée | En attente d'un disque |
| --- | --- |
| ![Barre latérale masquée](../screenshots/sidebar-hidden.png) | ![Aucun disque connecté](../screenshots/no-drive.png) |

![À propos de NTFSTool](../screenshots/about.png)

L'interface est disponible en 20 langues, avec prise en charge des dispositions de droite à gauche pour l'arabe et le persan : English, 简体中文, 繁體中文, 日本語,
한국어, Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی, Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย et Tiếng Việt.

## Ce qu'est ce dépôt

Il s'agit de l'**édition open source d'apprentissage et de référence** de NTFSTool. Elle dérive de NTFSTool gratuit sur le Mac App Store et conserve la
même architecture, mais elle est volontairement restée petite et lisible :

- chaque fichier source est commenté en anglais, afin que vous puissiez étudier comment s'articulent un module de système de fichiers FSKit, un pont C
  au-dessus du moteur NTFS et une interface SwiftUI/AppKit sandboxée ;
- le pilote utilise le chemin d'E/S simple et direct (pas de couche de cache ni d'optimisation de performances propre à une plateforme) ;
- l'application se limite à l'essentiel (pas de télémétrie, pas d'envoi de diagnostics, aucun accès réseau) ;
- il n'y a aucun code de licence, d'activation ou d'essai.

Si vous voulez simplement la meilleure expérience en tant qu'utilisateur, installez les versions officielles depuis
[ntfstool.com](https://ntfstool.com). NTFSTool Pro (code fermé) ajoute un second pilote pour les anciennes versions de macOS, la réparation de disque et
d'autres outils.

## Fonctionnalités

- Lecture **et écriture** des volumes NTFS ; macOS les monte automatiquement grâce au module de pilote FSKit
- Application sandboxée ; le pilote s'exécute comme un module FSKit sandboxé dans l'espace utilisateur
- Affiche l'état de chaque volume NTFS : lecture-écriture, lecture seule (Windows en veille prolongée / démarrage rapide), lecture seule via le pilote
  intégré d'Apple, non monté
- Vous guide pour activer l'extension de système de fichiers dans Réglages Système
- Ouvrir dans le Finder, éjecter, informations sur le disque
- 20 langues d'interface
- Attributs étendus stockés sous forme de flux nommés NTFS (pas de fichiers `._`) ; l'option de montage facultative `windows_names` refuse les noms de
  fichiers que Windows ne peut pas utiliser

## Prérequis

- macOS 15.6 ou version ultérieure pour l'exécution ; **l'écriture nécessite macOS 26 ou version ultérieure** (sous macOS 15, le système continue
  d'utiliser le pilote NTFS en lecture seule d'Apple pour les disques physiques)
- Pour compiler : Xcode 16.3 ou version ultérieure (SDK FSKit) avec ses outils en ligne de commande, Swift 5.9+

## Compilation — une seule commande

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Autres cibles :

| Commande | Ce qu'elle fait |
| --- | --- |
| `make run` | compile et ouvre l'application |
| `make demo` | lance l'interface avec des disques de démonstration en mémoire — ni pilote ni compte Apple Developer nécessaires |
| `make test` | exécute les tests unitaires |
| `make app UNIVERSAL=1` | compile arm64 + x86_64 |
| `make scan` | vérifie qu'aucun matériel de signature ni identifiant privé ne se trouve dans l'arborescence |

Les tests du pilote formatent et vérifient de véritables images disque NTFS avec les sous-commandes `mkntfs` et `ntfsfix` du binaire `ntfstool-driver`.
Ils sont ignorés sauf si les deux outils se trouvent dans `bin`/`sbin` de Homebrew ou dans le répertoire indiqué par `NTFSTOOL_NTFS_TOOLS=<dir>`.

## Exécuter le pilote (nécessite votre propre compte Apple Developer)

Le module FSKit utilise l'entitlement restreint `com.apple.developer.fskit.fsmodule`. macOS ne charge un module que si sa signature est adossée à un
profil de provisionnement qui l'accorde. Une compilation ad hoc affiche toute l'interface, mais le système ne chargera pas le pilote. Il s'agit d'une
règle de la plateforme Apple qui s'applique à tout projet FSKit.

1. Dans votre compte Apple Developer, créez deux App ID, par ex. `com.yourname.ntfs` et `com.yourname.ntfs.fskit`. Activez la capacité **FSKit Module**
   sur le second et, facultativement, un App Group (par ex. `TEAMID.com.yourname.ntfs`) sur les deux afin que l'application puisse lire le journal du
   pilote.
2. Créez des profils de provisionnement de développement (ou Developer ID) pour les deux et téléchargez-les.
3. Compilez avec votre identité :

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Copiez `build/NTFSTool.app` dans `/Applications`, ouvrez-le une fois, puis activez le pilote dans **Réglages Système → Général → Éléments de
   connexion et extensions → Extensions de système de fichiers**.
5. Rebranchez le disque NTFS. `mount` doit maintenant le lister avec le type `ntfstooloss`.

Toutes les options de compilation sont documentées en tête de [`scripts/build-app.sh`](../../scripts/build-app.sh). Les profils de provisionnement et les clés
de signature n'ont pas leur place dans ce dépôt ; `make scan` les recherche.

## Organisation

| Chemin | Ce que c'est |
| --- | --- |
| `Sources/NTFSTool` | Point d'entrée de l'application (`main.swift`) |
| `Sources/NTFSToolApp` | Couche AppKit, vues SwiftUI, modèle de l'application, services système, 20 localisations |
| `Sources/NTFSToolCore` | Logique pure sans AppKit : modèle de disque, liste des langues, stockage des réglages, journal d'événements local |
| `Sources/NTFSToolBridge` | Pont C (`ntfstool_bridge.c`) exposant des opérations basées sur les inodes au-dessus de `libntfstool-driver` |
| `Sources/NTFSToolDriver` | Cœur Swift du pilote (sans dépendance à FSKit, testable unitairement sur tout macOS) |
| `FSKitModule/NTFSToolFS` | Le module FSKit (`NTFSToolFS.appex`) que macOS charge pour monter les volumes NTFS |
| `Tests/` | Tests unitaires du modèle de l'application et du pilote (images NTFS réelles) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

Consultez [docs/ARCHITECTURE.md](../ARCHITECTURE.md) pour voir comment une écriture voyage du Finder jusqu'au disque.

## Contribuer

Les issues et les pull requests sont les bienvenues. Merci de signer vos commits (`git commit -s`) pour certifier le
[Developer Certificate of Origin](https://developercertificate.org/) — voir [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Licence

Licence MIT — voir [LICENSE](../../LICENSE) et [NOTICE](../../NOTICE). Le moteur NTFS inclus est le `libntfstool-driver` propre à NTFSTool, fourni sous forme de
bibliothèque statique précompilée (voir
[Vendor/README.md](../../Vendor/README.md)).

Les versions antérieures de ce dépôt (l'application Electron) restent disponibles dans l'historique git.

## Marques

La licence MIT vous donne le code, pas la marque. « NTFSTool » ainsi que les icônes et logos NTFSTool sont des marques du projet NTFSTool et ne sont
**pas** concédés sous licence MIT.

- Vous pouvez compiler et utiliser ce code source pour vous-même, et le désigner comme « basé sur NTFSTool ».
- Si vous **distribuez** une version modifiée ou un fork (y compris sur un app store), vous devez lui donner un nom et une icône différents, retirer les
  logos NTFSTool et utiliser votre propre identifiant de bundle — elle ne doit ni ressembler à une version officielle de NTFSTool, ni prétendre en être
  une.
- Les compilations non modifiées de ce dépôt ne peuvent être redistribuées sous le nom NTFSTool qu'avec une autorisation préalable.

NTFS et Windows sont des marques de Microsoft Corporation ; macOS et FSKit sont des marques d'Apple Inc. Ce projet n'est affilié ni à Microsoft ni à Apple.
