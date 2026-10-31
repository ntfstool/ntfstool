# NTFSTool (edición de código abierto)

NTFS de lectura y escritura para macOS, gratuito y de código abierto, construido sobre **FSKit** de Apple y sobre el motor NTFS propio de NTFSTool
(**libntfstool-driver**).

Conecta una unidad de Windows (NTFS) y macOS la monta con permiso de escritura mediante el módulo de controlador FSKit de NTFSTool: sin extensión del
kernel, sin macFUSE, sin reducir los ajustes de seguridad y sin contraseña de administrador. La propia aplicación se ejecuta totalmente en un entorno
aislado (sandbox).

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · **Español** · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

[Guía de arquitectura](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="Ventana principal de NTFSTool: una unidad NTFS de lectura y escritura con barra de capacidad y detalles" width="760">
</p>

<p align="center"><i>Si este proyecto te resulta útil o interesante, una ⭐ en GitHub ayuda a que otros lo encuentren.</i></p>

## Capturas de pantalla

Capturas reales de la aplicación compilada a partir de este repositorio (`make app`). Las unidades que se muestran son las unidades de ejemplo del modo
demo integrado (`make demo`), así que puedes probar todas las pantallas sin un disco físico.

### Ventana principal

Una sola ventana para todas tus unidades NTFS. Cada volumen muestra su estado de un vistazo — **Lectura y escritura**, **Solo lectura** o **No montado**
— con una barra de capacidad, el punto de montaje, el bus (USB / Thunderbolt) y los detalles del dispositivo, además de *Mostrar en el Finder* y
*Expulsar*.

![Ventana principal: una unidad USB de lectura y escritura](../screenshots/overview.png)

| Un SSD Thunderbolt, lectura y escritura | Una unidad no montada |
| --- | --- |
| ![Unidad Thunderbolt](../screenshots/thunderbolt-drive.png) | ![Unidad no montada](../screenshots/not-mounted.png) |

### Configuración guiada: sin contraseña, sin extensión del kernel

NTFSTool es un módulo de sistema de archivos FSKit corriente. Lo único que pide macOS es activar un interruptor en **Ajustes del Sistema → General →
Ítems de inicio y extensiones → Extensiones del sistema de archivos**. La aplicación lo explica paso a paso y detecta el cambio por sí sola.

![Activa la extensión del sistema de archivos](../screenshots/enable-extension.png)

### Seguro por defecto

Si Windows quedó en hibernación o el *inicio rápido* está activado, escribir dañaría el disco. NTFSTool monta una unidad así en **solo lectura** y te
indica exactamente cómo solucionarlo, en lugar de poner en riesgo tus datos.

| Unidad que Windows dejó hibernada | Lo que te indica la aplicación |
| --- | --- |
| ![Unidad hibernada](../screenshots/hibernated-windows.png) | ![Apagar Windows por completo](../screenshots/dialog-shut-down-windows.png) |

### Volver a conectar una unidad

macOS solo permite que el sistema monte las unidades físicas en el momento en que se conectan, así que la aplicación expulsa la unidad de forma segura y
te pide que la vuelvas a conectar; después se monta con lectura y escritura.

![Vuelve a conectar la unidad](../screenshots/dialog-plug-in-again.png)

### Modo oscuro

| Ventana principal | Guía de la extensión |
| --- | --- |
| ![Modo oscuro, ventana principal](../screenshots/dark-overview.png) | ![Modo oscuro, guía de la extensión](../screenshots/dark-enable-extension.png) |

### Diseño compacto, estado vacío y Acerca de

| Barra lateral oculta | Esperando una unidad |
| --- | --- |
| ![Barra lateral oculta](../screenshots/sidebar-hidden.png) | ![Ninguna unidad conectada](../screenshots/no-drive.png) |

![Acerca de NTFSTool](../screenshots/about.png)

La interfaz está disponible en 20 idiomas, con diseños de derecha a izquierda para árabe y persa: English, 简体中文, 繁體中文, 日本語, 한국어, Deutsch, Français,
Español, Italiano, Português, Русский, العربية, فارسی, Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย y Tiếng Việt.

## Qué es este repositorio

Esta es la **edición de código abierto para aprendizaje y referencia** de NTFSTool. Deriva del NTFSTool gratuito de la Mac App Store y conserva la misma
arquitectura, pero se mantiene deliberadamente pequeña y legible:

- cada archivo fuente está comentado en inglés, para que puedas estudiar cómo encajan un módulo de sistema de archivos FSKit, un puente en C sobre el
  motor NTFS y una interfaz SwiftUI/AppKit aislada en sandbox;
- el controlador usa la ruta de E/S simple y directa (sin capa de caché ni ajustes de rendimiento específicos de la plataforma);
- la aplicación solo hace lo básico (sin telemetría, sin envío de diagnósticos, sin ningún acceso a la red);
- no hay código de licencias, activación ni prueba.

Si lo que quieres es la mejor experiencia como usuario, instala las versiones oficiales desde
[ntfstool.com](https://ntfstool.com). NTFSTool Pro (código cerrado) añade un segundo controlador para versiones antiguas de macOS, reparación de discos
y otras herramientas.

## Características

- Lee **y escribe** en volúmenes NTFS; macOS los monta automáticamente mediante el módulo de controlador FSKit
- Aplicación en sandbox; el controlador se ejecuta como un módulo FSKit en sandbox en el espacio de usuario
- Muestra el estado de cada volumen NTFS: lectura y escritura, solo lectura (Windows hibernado / inicio rápido), solo lectura por el controlador
  integrado de Apple, no montado
- Te guía para activar la extensión del sistema de archivos en Ajustes del Sistema
- Abrir en el Finder, expulsar, información del disco
- 20 idiomas de interfaz
- Atributos extendidos almacenados como flujos con nombre de NTFS (sin archivos `._`); la opción de montaje opcional `windows_names` rechaza los nombres
  de archivo que Windows no puede usar

## Requisitos

- macOS 15.6 o posterior para ejecutarlo; **la escritura requiere macOS 26 o posterior** (en macOS 15 el sistema sigue usando el controlador NTFS de
  solo lectura de Apple para los discos físicos)
- Para compilar: Xcode 16.3 o posterior (SDK de FSKit) con sus herramientas de línea de comandos, Swift 5.9+

## Compilación: un solo comando

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Otros destinos:

| Comando | Qué hace |
| --- | --- |
| `make run` | compila y abre la aplicación |
| `make demo` | ejecuta la interfaz con discos de demostración en memoria: no necesita controlador ni cuenta de Apple Developer |
| `make test` | ejecuta las pruebas unitarias |
| `make app UNIVERSAL=1` | compila arm64 + x86_64 |
| `make scan` | comprueba que en el árbol no haya material de firma ni identificadores privados |

Las pruebas del controlador formatean y comprueban imágenes de disco NTFS reales con los subcomandos `mkntfs` y `ntfsfix` del binario `ntfstool-driver`.
Se omiten a menos que ambas herramientas se encuentren en `bin`/`sbin` de Homebrew o en el directorio indicado por `NTFSTOOL_NTFS_TOOLS=<dir>`.

## Ejecutar el controlador (requiere tu propia cuenta de Apple Developer)

El módulo FSKit usa el entitlement restringido `com.apple.developer.fskit.fsmodule`. macOS solo carga un módulo cuya firma esté respaldada por un perfil
de aprovisionamiento que lo conceda. Una compilación ad hoc muestra toda la interfaz, pero el sistema no cargará el controlador. Es una regla de la
plataforma de Apple que se aplica a todo proyecto FSKit.

1. En tu cuenta de Apple Developer crea dos App ID, por ejemplo `com.yourname.ntfs` y `com.yourname.ntfs.fskit`. Activa la capacidad **FSKit Module** en
   el segundo y, opcionalmente, un App Group (por ejemplo `TEAMID.com.yourname.ntfs`) en ambos para que la aplicación pueda leer el registro del
   controlador.
2. Crea perfiles de aprovisionamiento de desarrollo (o de Developer ID) para ambos y descárgalos.
3. Compila con tu identidad:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Copia `build/NTFSTool.app` a `/Applications`, ábrela una vez y después activa el controlador en **Ajustes del Sistema → General → Ítems de inicio y
   extensiones → Extensiones del sistema de archivos**.
5. Vuelve a conectar la unidad NTFS. Ahora `mount` debería listarla con el tipo `ntfstooloss`.

Todas las opciones de compilación están documentadas al principio de [`scripts/build-app.sh`](../../scripts/build-app.sh). Los perfiles de aprovisionamiento y
las claves de firma nunca deben estar en este repositorio; `make scan` lo comprueba.

## Estructura

| Ruta | Qué es |
| --- | --- |
| `Sources/NTFSTool` | Punto de entrada de la aplicación (`main.swift`) |
| `Sources/NTFSToolApp` | Capa AppKit, vistas SwiftUI, el modelo de la aplicación, servicios del sistema, 20 localizaciones |
| `Sources/NTFSToolCore` | Lógica pura sin AppKit: modelo de disco, lista de idiomas, almacén de ajustes, registro local de eventos |
| `Sources/NTFSToolBridge` | Puente en C (`ntfstool_bridge.c`) que expone operaciones basadas en inodos sobre `libntfstool-driver` |
| `Sources/NTFSToolDriver` | Núcleo en Swift del controlador (sin dependencia de FSKit, probable con pruebas unitarias en cualquier macOS) |
| `FSKitModule/NTFSToolFS` | El módulo FSKit (`NTFSToolFS.appex`) que macOS carga para montar volúmenes NTFS |
| `Tests/` | Pruebas unitarias del modelo de la aplicación y del controlador (imágenes NTFS reales) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

Consulta [docs/ARCHITECTURE.md](../ARCHITECTURE.md) para ver cómo viaja una escritura desde el Finder hasta el disco.

## Contribuir

Las incidencias (issues) y las pull requests son bienvenidas. Firma tus commits (`git commit -s`) para certificar el
[Developer Certificate of Origin](https://developercertificate.org/) — consulta [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Licencia

Licencia MIT — consulta [LICENSE](../../LICENSE) y [NOTICE](../../NOTICE). El motor NTFS incluido es el `libntfstool-driver` propio de NTFSTool, distribuido como
biblioteca estática precompilada (consulta
[Vendor/README.md](../../Vendor/README.md)).

Las versiones anteriores de este repositorio (la aplicación Electron) siguen disponibles en el historial de git.

## Marcas registradas

La licencia MIT te da el código, no la marca. «NTFSTool» y los iconos y logotipos de NTFSTool son marcas registradas del proyecto NTFSTool y **no** se
licencian bajo la licencia MIT.

- Puedes compilar y usar este código fuente para ti mismo, y referirte a él como «basado en NTFSTool».
- Si **distribuyes** una versión modificada o un fork (incluso en cualquier tienda de aplicaciones), debes darle un nombre y un icono distintos,
  eliminar los logotipos de NTFSTool y usar tu propio identificador de bundle: no debe parecerse a una versión oficial de NTFSTool ni afirmar serlo.
- Las compilaciones sin modificar de este repositorio solo pueden redistribuirse con el nombre NTFSTool con permiso previo.

NTFS y Windows son marcas registradas de Microsoft Corporation; macOS y FSKit son marcas registradas de Apple Inc. Este proyecto no está afiliado a
Microsoft ni a Apple.
