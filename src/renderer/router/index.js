/**
 * @author service@ntfstool.com
 */
import Vue from 'vue'
import Router from 'vue-router'
import Home from '@/page/Home'
import Setting from '@/page/Setting'
import Tray from '@/page/Tray'
Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'Home',
      component:Home
    },
    {
      path: '/setting',
      name: 'Setting',
      component:Setting
    },
    {
      path: '/tray',
      name: 'Tray',
      component:Tray
    },
    {
      path: '*',
      redirect: '/'
    }
  ]
})
