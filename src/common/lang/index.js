/**
 * @author   service@ntfstool.com
 * Copyright (c) 2020 ntfstool.com
 * Copyright (c) 2020 alfw.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the MIT General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * MIT General Public License for more details.
 *
 * You should have received a copy of the MIT General Public License
 * along with this program (in the main directory of the NTFS Tool
 * distribution in the file COPYING); if not, write to the service@ntfstool.com
 */

import en from './en'
import zhCN from './zhCN'
import zhTW from './zhTW'
import ja from './ja'
import ko from './ko'
import es from './es'

import enLocale from 'element-ui/lib/locale/lang/en'
import zhcnLocale from 'element-ui/lib/locale/lang/zh-CN'
import zhtwLocale from 'element-ui/lib/locale/lang/zh-TW'
import koLocale from 'element-ui/lib/locale/lang/ko'
import jaLocale from 'element-ui/lib/locale/lang/ja'
import esLocale from 'element-ui/lib/locale/lang/es'

const languages_select = {
    languages: [
        {
            text: "English",
            val: 'en'
        }, {
            text: "中文 (简体)",
            val: 'zhCN'
        }
        , {
            text: "中文 (繁體)",
            val: 'zhTW'
        }, {
            text: "日本語",
            val: 'ja'
        }, {
            text: "Korea",//韩语
            val: 'ko'
        }, {
            text: "Espanol",//西班牙语
            val: 'es'
        }

    ]
};

export default {
    en: {...en, ...languages_select, ...enLocale},
    zhCN: {...zhCN, ...languages_select, ...zhcnLocale},
    zhTW: {...zhTW, ...languages_select, ...zhtwLocale},
    ja: {...ja, ...languages_select, ...jaLocale},
    ko: {...ko, ...languages_select, ...koLocale},
    es: {...es, ...languages_select, ...esLocale},
}