# NTFSTool (النسخة مفتوحة المصدر)

نظام NTFS مجاني ومفتوح المصدر للقراءة والكتابة على macOS، مبني على **FSKit**
من Apple ومحرك NTFS الخاص بـ NTFSTool (**libntfstool-driver**).

وصّل قرص Windows (NTFS) فيقوم macOS بتحميله قابلاً للكتابة عبر وحدة تشغيل FSKit الخاصة بـ NTFSTool — بلا امتداد نواة (kernel extension)،
وبلا macFUSE، وبلا خفض لإعدادات الأمان، وبلا كلمة مرور المسؤول. التطبيق نفسه يعمل داخل بيئة معزولة (sandbox) بالكامل.

[English](../../README.md) · [简体中文](../../README-CN.md) · [繁體中文](README.zh-Hant.md) · [日本語](README.ja.md) · [한국어](README.ko.md) · [Deutsch](README.de.md) · [Français](README.fr.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Português](README.pt.md) · [Русский](README.ru.md) · **العربية** · [فارسی](README.fa.md) · [Türkçe](README.tr.md) · [हिन्दी](README.hi.md) · [বাংলা](README.bn.md) · [Bahasa Indonesia](README.id.md) · [Kiswahili](README.sw.md) · [ไทย](README.th.md) · [Tiếng Việt](README.vi.md)

> **تبحث عن الإصدار القديم؟** الإصدارات السابقة المبنية على Electron/Fuse (من 2.x إلى 4.7.x) وسجلّها الكامل مؤرشفة في [فرع `V3`](https://github.com/ntfstool/ntfstool/tree/V3). لم تعد مدعومة؛ والفرع `master` هو الإصدار الحالي المبني على FSKit.

[دليل البنية المعمارية](../ARCHITECTURE.md)

<p align="center">
  <img src="../screenshots/overview.png" alt="النافذة الرئيسية لـ NTFSTool: قرص NTFS للقراءة والكتابة مع شريط السعة والتفاصيل" width="760">
</p>

<p align="center"><i>إذا كان هذا المشروع مفيدًا أو مثيرًا لاهتمامك، فإن نجمة ⭐ على GitHub تساعد الآخرين على اكتشافه.</i></p>

## لقطات الشاشة

لقطات شاشة حقيقية للتطبيق المبني من هذا المستودع (`make app`). الأقراص الظاهرة هي أقراص العيّنة
في وضع العرض التجريبي المدمج (`make demo`)، بحيث يمكنك تجربة كل الشاشات دون قرص فعلي.

### النافذة الرئيسية

نافذة واحدة لجميع أقراص NTFS لديك. يعرض كل وحدة تخزين حالتها بنظرة واحدة — **قراءة وكتابة** أو **للقراءة فقط** أو **غير محمّل**
— مع شريط السعة ونقطة التحميل والناقل (USB / Thunderbolt) وتفاصيل الجهاز، إضافةً إلى *إظهار في Finder* و*إخراج*.

![النافذة الرئيسية: قرص USB للقراءة والكتابة](../screenshots/overview.png)

| قرص SSD عبر Thunderbolt، قراءة وكتابة | قرص غير محمّل |
| --- | --- |
| ![قرص Thunderbolt](../screenshots/thunderbolt-drive.png) | ![قرص غير محمّل](../screenshots/not-mounted.png) |

### إعداد موجَّه: بلا كلمة مرور وبلا امتداد نواة

NTFSTool هو وحدة نظام ملفات FSKit عادية. الشيء الوحيد الذي يطلبه macOS هو تفعيل مفتاح واحد في
**إعدادات النظام ← عام ← عناصر تسجيل الدخول والإضافات ← إضافات نظام الملفات**. يشرح التطبيق ذلك خطوة بخطوة
ويلاحظ التغيير من تلقاء نفسه.

![فعّل إضافة نظام الملفات](../screenshots/enable-extension.png)

### آمن افتراضيًا

إذا كان Windows قد وضع الجهاز في وضع السبات أو كان *التشغيل السريع* (Fast Startup) مفعّلًا، فإن الكتابة ستُتلف القرص. يحمّل NTFSTool مثل هذا القرص
**للقراءة فقط** ويخبرك بدقة كيف تعالج المشكلة، بدلًا من المخاطرة ببياناتك.

| قرص تركه Windows في وضع السبات | ما يخبرك به التطبيق |
| --- | --- |
| ![قرص في وضع السبات](../screenshots/hibernated-windows.png) | ![أوقف تشغيل Windows بالكامل](../screenshots/dialog-shut-down-windows.png) |

### إعادة توصيل القرص

لا يسمح macOS للنظام بتحميل الأقراص الفعلية إلا عند توصيلها، لذا يُخرج التطبيق القرص بأمان
ويطلب منك إعادة توصيله؛ وعندها يتم تحميله للقراءة والكتابة.

![أعد توصيل القرص](../screenshots/dialog-plug-in-again.png)

### الوضع الداكن

| النافذة الرئيسية | دليل الإضافة |
| --- | --- |
| ![الوضع الداكن، النافذة الرئيسية](../screenshots/dark-overview.png) | ![الوضع الداكن، دليل الإضافة](../screenshots/dark-enable-extension.png) |

### التخطيط المضغوط وحالة الفراغ وحول

| الشريط الجانبي مخفي | بانتظار قرص |
| --- | --- |
| ![الشريط الجانبي مخفي](../screenshots/sidebar-hidden.png) | ![لا يوجد قرص متصل](../screenshots/no-drive.png) |

![حول NTFSTool](../screenshots/about.png)

الواجهة متاحة بـ 20 لغة، بما في ذلك التخطيطات من اليمين إلى اليسار للعربية والفارسية: English،
简体中文، 繁體中文، 日本語، 한국어، Deutsch، Français، Español، Italiano، Português، Русский، العربية، فارسی،
Türkçe، हिन्दी، বাংলা، Bahasa Indonesia، Kiswahili، ไทย و Tiếng Việt.

## ما هذا المستودع

هذه هي **النسخة المفتوحة المصدر للتعلّم والمرجعية** من NTFSTool. وهي مشتقة من NTFSTool المجاني على Mac App Store وتحتفظ بالبنية المعمارية نفسها، لكنها صغيرة ومقروءة عمدًا:

- كل ملف مصدري معلَّق عليه بالإنجليزية، لتتمكن من دراسة كيف تتكامل وحدة نظام ملفات FSKit، وجسر C فوق
  محرك NTFS، وواجهة SwiftUI/AppKit المعزولة؛
- يستخدم المشغّل مسار الإدخال/الإخراج المباشر البسيط (بلا طبقة تخزين مؤقت ولا ضبط أداء خاص بالمنصة)؛
- يؤدي التطبيق الأساسيات فقط (بلا قياس عن بُعد، وبلا رفع تشخيصات، وبلا أي وصول إلى الشبكة إطلاقًا)؛
- لا توجد أي شيفرة للترخيص أو التفعيل أو الفترة التجريبية.

إذا كنت تريد أفضل تجربة كمستخدم فقط، فثبّت الإصدارات الرسمية من [ntfstool.com](https://ntfstool.com).
تضيف NTFSTool Pro (مغلقة المصدر) مشغّلًا ثانيًا لإصدارات macOS الأقدم، وإصلاح الأقراص وأدوات أخرى.

## الميزات

- قراءة **وكتابة** وحدات NTFS؛ يحمّلها macOS تلقائيًا عبر وحدة مشغّل FSKit
- تطبيق معزول (sandbox)؛ ويعمل المشغّل كوحدة FSKit معزولة في فضاء المستخدم
- يعرض حالة كل وحدة NTFS: قراءة وكتابة، قراءة فقط (Windows في السبات / التشغيل السريع)، قراءة فقط
  بواسطة المشغّل المدمج من Apple، غير محمّل
- يرشدك إلى تفعيل إضافة نظام الملفات في إعدادات النظام
- الفتح في Finder، والإخراج، ومعلومات القرص
- 20 لغة للواجهة
- تُخزَّن السمات الموسّعة كتدفقات NTFS مسمّاة (بلا ملفات `._`)؛ وخيار التحميل الاختياري `windows_names`
  يرفض أسماء الملفات التي لا يستطيع Windows استخدامها

## المتطلبات

- macOS 15.6 أو أحدث للتشغيل؛ **دعم الكتابة يتطلب macOS 26 أو أحدث** (في macOS 15 يواصل النظام
  استخدام مشغّل NTFS للقراءة فقط من Apple للأقراص الفعلية)
- للبناء: Xcode 16.3 أو أحدث (FSKit SDK) مع أدوات سطر الأوامر الخاصة به، وSwift 5.9+

## البناء — بأمر واحد

```sh
git clone https://github.com/ntfstool/ntfstool.git
cd ntfstool
make            # → build/NTFSTool.app (ad-hoc signed, native architecture)
```

أهداف أخرى:

| الأمر | ما يفعله |
| --- | --- |
| `make run` | يبني التطبيق ويفتحه |
| `make demo` | يشغّل الواجهة بأقراص تجريبية في الذاكرة — بلا مشغّل وبلا حساب Apple Developer |
| `make test` | يشغّل اختبارات الوحدة |
| `make app UNIVERSAL=1` | يبني arm64 + x86_64 |
| `make scan` | يتحقق من خلوّ الشجرة من مواد التوقيع أو المعرّفات الخاصة |

تنسّق اختبارات المشغّل صور أقراص NTFS حقيقية وتفحصها باستخدام الأمرين الفرعيين `mkntfs` و`ntfsfix` في الملف التنفيذي `ntfstool-driver`. وتُتخطّى
ما لم تُوجد الأداتان معًا في `bin`/`sbin` الخاصة بـ Homebrew أو في المجلد المحدد بـ `NTFSTOOL_NTFS_TOOLS=<dir>`.

## تشغيل المشغّل (يتطلب حساب Apple Developer الخاص بك)

تستخدم وحدة FSKit الاستحقاق المقيَّد `com.apple.developer.fskit.fsmodule`. لا يحمّل macOS إلا الوحدة
التي يستند توقيعها إلى ملف تعريف تجهيز (provisioning profile) يمنحها هذا الاستحقاق. يعرض البناء الموقَّع ad-hoc الواجهة كاملة، لكن
النظام لن يحمّل المشغّل. هذه قاعدة منصة من Apple تنطبق على كل مشاريع FSKit.

1. في حساب Apple Developer لديك أنشئ معرّفَي تطبيق (App ID)، مثل `com.yourname.ntfs` و`com.yourname.ntfs.fskit`.
   فعّل إمكانية **FSKit Module** على الثاني، واختياريًا App Group
   (مثل `TEAMID.com.yourname.ntfs`) على كليهما ليتمكن التطبيق من قراءة سجل المشغّل.
2. أنشئ ملفَّي تعريف تجهيز للتطوير (أو Developer ID) لكليهما ونزّلهما.
3. ابنِ بهويتك:

   ```sh
   TEAM_ID=ABCDE12345 \
   BUNDLE_ID=com.yourname.ntfs \
   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
   APP_PROFILE=~/Downloads/app.provisionprofile \
   FSKIT_PROFILE=~/Downloads/fskit.provisionprofile \
   make app
   ```

4. انسخ `build/NTFSTool.app` إلى `/Applications`، وافتحه مرة واحدة، ثم فعّل المشغّل في
   **إعدادات النظام ← عام ← عناصر تسجيل الدخول والإضافات ← إضافات نظام الملفات**.
5. أعد توصيل قرص NTFS. يجب أن يعرضه الأمر `mount` الآن بالنوع `ntfstooloss`.

جميع خيارات البناء موثّقة في أعلى الملف [`scripts/build-app.sh`](../../scripts/build-app.sh).
ملفات تعريف التجهيز ومفاتيح التوقيع لا مكان لها في هذا المستودع؛ ويتحقق `make scan` من ذلك.

## البنية

| المسار | ما هو |
| --- | --- |
| `Sources/NTFSTool` | نقطة دخول التطبيق (`main.swift`) |
| `Sources/NTFSToolApp` | غلاف AppKit، وواجهات SwiftUI، ونموذج التطبيق، وخدمات النظام، و20 ترجمة |
| `Sources/NTFSToolCore` | منطق خالص بلا AppKit: نموذج القرص، وقائمة اللغات، ومخزن الإعدادات، وسجل الأحداث المحلي |
| `Sources/NTFSToolBridge` | جسر C (`ntfstool_bridge.c`) يوفّر عمليات قائمة على inode فوق `libntfstool-driver` |
| `Sources/NTFSToolDriver` | نواة Swift للمشغّل (بلا اعتماد على FSKit، وقابلة لاختبار الوحدة على أي macOS) |
| `FSKitModule/NTFSToolFS` | وحدة FSKit (`NTFSToolFS.appex`) التي يحمّلها macOS لتحميل وحدات NTFS |
| `Tests/` | اختبارات الوحدة لنموذج التطبيق وللمشغّل (بصور NTFS حقيقية) |
| `scripts/` | `build-app.sh` و`build-fskit.sh` و`scan-secrets.sh` |

راجع [docs/ARCHITECTURE.md](../ARCHITECTURE.md) لمعرفة كيف تنتقل عملية الكتابة من Finder إلى القرص.

## المساهمة

نرحّب بالمشكلات (issues) وطلبات الدمج (pull requests). يُرجى توقيع الإيداعات (`git commit -s`) للإقرار بـ
[شهادة أصل المطوّر](https://developercertificate.org/) — راجع [CONTRIBUTING.md](../../CONTRIBUTING.md).

## الترخيص


## العلامات التجارية

**يرجى عدم نشر هذا التطبيق أو أي نسخة متفرعة منه على Mac App Store** — فهذا ينافس النسخة المجانية الرسمية ويربك المستخدمين.
