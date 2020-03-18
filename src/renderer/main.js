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

import Vue from 'vue'
import axios from 'axios'
import App from './App'
import router from '@/common/router/index'

import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'
import '@/renderer/theme/ntfstool.css'

import messages from '@/common/lang/index'
import ElementLocale from 'element-ui/lib/locale'
import VueI18n from 'vue-i18n'
const Store = require('electron-store');
const store = new Store();

Vue.use(ElementUI)
Vue.use(VueI18n)
let langNow = store.get("lang", "en");


const i18n = new VueI18n({locale: langNow,fallbackLocale: "en",messages});

ElementLocale.i18n((key, value) => i18n.t(key, value))

if (!process.env.IS_WEB) Vue.use(require('vue-electron'))

Vue.http = Vue.prototype.$http = axios

Vue.config.productionTip = false
Vue.config.devtools = false;

new Vue({
    components: {App},
    router,
    i18n,
    store,
    template: '<App/>'
}).$mount('#app')
