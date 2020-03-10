/**
 * @author service@ntfstool.com
 */
import Vue from 'vue'
import axios from 'axios'
import App from './App'
import router from './router'

import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'
import '@/theme/ntfstool.css'

import messages from '@/lang/index'
import ElementLocale from 'element-ui/lib/locale'
import VueI18n from 'vue-i18n'
import {alEvent} from '@/utils/alevent.js';
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


new Vue({
    components: {App},
    router,
    i18n,
    store,
    template: '<App/>'
}).$mount('#app')
