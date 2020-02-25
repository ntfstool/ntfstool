/**
 * @author service@ntfstool.com
 */
import Vue from 'vue'
import axios from 'axios'
import App from './App'
import router from './router'

import '../../static/font_icon_file/iconfont.css'
import 'muse-ui/dist/muse-ui.css'
import MuseUI from 'muse-ui'
import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'

import '@/theme/default.css'
import '@/assets/iconfont/iconfont.css'

import messages from '@/lang/index'
import ElementLocale from 'element-ui/lib/locale'
import VueI18n from 'vue-i18n'
import {ntfsList, mountDevices, checkDevicesIsNtfs} from '@/utils/utils'
import {alEvent} from '@/utils/alevent.js';

const Store = require('electron-store');
const store = new Store();

Vue.use(ElementUI)
Vue.use(MuseUI)
Vue.use(VueI18n)


let langNow = store.get("lang", "en");

// Create VueI18n instance with options
const i18n = new VueI18n({
    locale: langNow, // set locale
    fallbackLocale: "en",
    messages, // set locale messages
});

ElementLocale.i18n((key, value) => i18n.t(key, value))

if (!process.env.IS_WEB) Vue.use(require('vue-electron'))

Vue.http = Vue.prototype.$http = axios
Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
    components: {App},
    router,
    i18n,
    store,
    template: '<App/>'
}).$mount('#app')
