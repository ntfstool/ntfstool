# NTFSTool (edição de código aberto)

NTFS com leitura e gravação para macOS, gratuito e de código aberto, construído sobre o **FSKit** da Apple e sobre o motor NTFS próprio do
NTFSTool (**libntfstool-driver**).

Conecte um disco do Windows (NTFS) e o macOS o monta com permissão de gravação por meio do módulo de driver FSKit do NTFSTool — sem extensão de
kernel, sem macFUSE, sem reduzir as configurações de segurança, sem senha de administrador. O próprio app roda totalmente em sandbox.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · **Português** · [Русский](README.ru.md) · [العربية](README.ar.md) · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

> **Procurando a versão antiga?** As versões anteriores baseadas em Electron/Fuse (2.x – 4.7.x) e todo o histórico estão arquivados no [branch `V3`](https://github.com/ntfstool/ntfstool/tree/V3). Não são mais mantidas; `master` é a edição atual baseada em FSKit.

[Guia de arquitetura](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="Janela principal do NTFSTool: um disco NTFS com leitura e gravação, com barra de capacidade e detalhes" width="760">
</p>

<p align="center"><i>Se este projeto é útil ou interessante para você, uma ⭐ no GitHub ajuda outras pessoas a encontrá-lo.</i></p>

## Capturas de tela

Capturas de tela reais do app compilado a partir deste repositório (`make app`). Os discos exibidos são os discos de exemplo do modo demo
integrado (`make demo`), então você pode testar todas as telas sem um disco físico.

### Janela principal

Uma única janela para todos os seus discos NTFS. Cada volume mostra seu estado de relance — **Leitura e gravação**, **Somente leitura** ou **Não
montado** — com uma barra de capacidade, o ponto de montagem, o barramento (USB / Thunderbolt) e os detalhes do dispositivo, além de *Mostrar no
Finder* e *Ejetar*.

![Janela principal: um disco USB com leitura e gravação](../screenshots/overview.png)

| Um SSD Thunderbolt, leitura e gravação | Um disco não montado |
| --- | --- |
| ![Disco Thunderbolt](../screenshots/thunderbolt-drive.png) | ![Disco não montado](../screenshots/not-mounted.png) |

### Configuração guiada: sem senha, sem extensão de kernel

O NTFSTool é um módulo de sistema de arquivos FSKit comum. A única coisa que o macOS pede é ativar um botão em **Ajustes do Sistema → Geral →
Itens de Login e Extensões → Extensões do Sistema de Arquivos**. O app explica o processo passo a passo e percebe a mudança por conta própria.

![Ative a extensão do sistema de arquivos](../screenshots/enable-extension.png)

### Seguro por padrão

Se o Windows foi hibernado ou a *Inicialização Rápida* está ativada, gravar corromperia o disco. O NTFSTool monta esse disco como **somente
leitura** e diz exatamente como corrigir, em vez de arriscar seus dados.

| Disco deixado hibernado pelo Windows | O que o app informa |
| --- | --- |
| ![Disco hibernado](../screenshots/hibernated-windows.png) | ![Desligue o Windows completamente](../screenshots/dialog-shut-down-windows.png) |

### Conectando o disco novamente

O macOS só permite que o sistema monte discos físicos no momento em que são conectados, então o app ejeta o disco com segurança e pede que você o
conecte de novo; depois disso, ele é montado com leitura e gravação.

![Conecte o disco novamente](../screenshots/dialog-plug-in-again.png)

### Modo escuro

| Janela principal | Guia da extensão |
| --- | --- |
| ![Modo escuro, janela principal](../screenshots/dark-overview.png) | ![Modo escuro, guia da extensão](../screenshots/dark-enable-extension.png) |

### Layout compacto, estado vazio e Sobre

| Barra lateral oculta | Aguardando um disco |
| --- | --- |
| ![Barra lateral oculta](../screenshots/sidebar-hidden.png) | ![Nenhum disco conectado](../screenshots/no-drive.png) |

![Sobre o NTFSTool](../screenshots/about.png)

A interface está disponível em 20 idiomas, incluindo layouts da direita para a esquerda para árabe e persa: English, 简体中文, 繁體中文, 日本語, 한국어,
Deutsch, Français, Español, Italiano, Português, Русский, العربية, فارسی, Türkçe, हिन्दी, বাংলা, Bahasa Indonesia, Kiswahili, ไทย e Tiếng Việt.

## O que é este repositório

Esta é a **edição de código aberto para estudo e referência** do NTFSTool. Ela deriva do NTFSTool gratuito da Mac App Store e mantém a mesma
arquitetura, mas é deliberadamente mantida pequena e legível:

- todo arquivo-fonte é comentado em inglês, para que você possa estudar como um módulo de sistema de arquivos FSKit, uma ponte em C sobre o motor
  NTFS e um front end SwiftUI/AppKit em sandbox se encaixam;
- o driver usa o caminho de E/S simples e direto (sem camada de cache nem ajustes de desempenho específicos da plataforma);
- o app faz apenas o básico (sem telemetria, sem envio de diagnósticos, sem nenhum acesso à rede);
- não há nenhum código de licenciamento, ativação ou período de teste.

Se você só quer a melhor experiência como usuário, instale as versões oficiais em
[ntfstool.com](https://ntfstool.com). O NTFSTool Pro (código fechado) adiciona um segundo driver para versões mais antigas do macOS, reparo de
disco e outras ferramentas.

## Recursos

- Lê **e grava** em volumes NTFS; o macOS os monta automaticamente por meio do módulo de driver FSKit
- App em sandbox; o driver roda como um módulo FSKit em sandbox no espaço do usuário
- Mostra o estado de cada volume NTFS: leitura e gravação, somente leitura (Windows hibernado / Inicialização Rápida), somente leitura pelo driver
  integrado da Apple, não montado
- Orienta você a ativar a extensão do sistema de arquivos em Ajustes do Sistema
- Abrir no Finder, ejetar, informações do disco
- 20 idiomas de interface
- Atributos estendidos armazenados como fluxos nomeados do NTFS (sem arquivos `._`); a opção de montagem opcional `windows_names` recusa nomes de
  arquivo que o Windows não consegue usar

## Requisitos

- macOS 15.6 ou posterior para executar; **a gravação exige macOS 26 ou posterior** (no macOS 15 o sistema continua usando o driver NTFS somente
  leitura da Apple para discos físicos)
- Para compilar: Xcode 16.3 ou posterior (SDK do FSKit) com suas ferramentas de linha de comando, Swift 5.9+

## Compilação — um único comando

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

Outros alvos:

| Comando | O que faz |
| --- | --- |
| `make run` | compila e abre o app |
| `make demo` | executa a interface com discos de demonstração em memória — sem driver e sem conta Apple Developer |
| `make test` | executa os testes unitários |
| `make app UNIVERSAL=1` | compila arm64 + x86_64 |
| `make scan` | verifica se não há material de assinatura nem identificadores privados na árvore |

Os testes do driver formatam e verificam imagens de disco NTFS reais com os subcomandos `mkntfs` e `ntfsfix` do binário `ntfstool-driver`. Eles
são ignorados a menos que as duas ferramentas sejam encontradas em `bin`/`sbin` do Homebrew ou no diretório indicado por
`NTFSTOOL_NTFS_TOOLS=<dir>`.

## Executando o driver (exige sua própria conta Apple Developer)

O módulo FSKit usa o entitlement restrito `com.apple.developer.fskit.fsmodule`. O macOS só carrega um módulo cuja assinatura seja respaldada por
um perfil de provisionamento que o conceda. Uma compilação ad hoc mostra toda a interface, mas o sistema não carregará o driver. É uma regra da
plataforma Apple que vale para todo projeto FSKit.

1. Na sua conta Apple Developer, crie dois App IDs, por exemplo `com.yourname.ntfs` e `com.yourname.ntfs.fskit`. Ative a capacidade **FSKit
   Module** no segundo e, opcionalmente, um App Group (por exemplo `TEAMID.com.yourname.ntfs`) em ambos, para que o app possa ler o log do driver.
2. Crie perfis de provisionamento de desenvolvimento (ou Developer ID) para os dois e faça o download.
3. Compile com a sua identidade:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. Copie `build/NTFSTool.app` para `/Applications`, abra-o uma vez e depois ative o driver em **Ajustes do Sistema → Geral → Itens de Login e
   Extensões → Extensões do Sistema de Arquivos**.
5. Conecte o disco NTFS novamente. Agora `mount` deve listá-lo com o tipo `ntfstooloss`.

Todas as opções de compilação estão documentadas no início de [`scripts/build-app.sh`](../../scripts/build-app.sh). Perfis de provisionamento e chaves
de assinatura nunca devem ficar neste repositório; `make scan` verifica isso.

## Estrutura

| Caminho | O que é |
| --- | --- |
| `Sources/NTFSTool` | Ponto de entrada do app (`main.swift`) |
| `Sources/NTFSToolApp` | Camada AppKit, views SwiftUI, o modelo do app, serviços do sistema, 20 localizações |
| `Sources/NTFSToolCore` | Lógica pura sem AppKit: modelo de disco, lista de idiomas, armazenamento de configurações, registro local de eventos |
| `Sources/NTFSToolBridge` | Ponte em C (`ntfstool_bridge.c`) que expõe operações baseadas em inodes sobre o `libntfstool-driver` |
| `Sources/NTFSToolDriver` | Núcleo em Swift do driver (sem dependência do FSKit, testável com testes unitários em qualquer macOS) |
| `FSKitModule/NTFSToolFS` | O módulo FSKit (`NTFSToolFS.appex`) que o macOS carrega para montar volumes NTFS |
| `Tests/` | Testes unitários do modelo do app e do driver (imagens NTFS reais) |
| `scripts/` | `build-app.sh`, `build-fskit.sh`, `scan-secrets.sh` |

Veja [docs/ARCHITECTURE.md](../ARCHITECTURE.md) para entender como uma gravação viaja do Finder até o disco.

## Contribuindo

Issues e pull requests são bem-vindos. Assine seus commits (`git commit -s`) para certificar o
[Developer Certificate of Origin](https://developercertificate.org/) — veja [CONTRIBUTING.md](../../CONTRIBUTING.md).

## Licença


## Marcas registradas

**Por favor, não publique este app, nem um fork dele, na Mac App Store**: isso competiria com a versão gratuita oficial e confundiria os usuários.
