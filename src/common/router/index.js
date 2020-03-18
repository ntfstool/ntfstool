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
import Router from 'vue-router'
import Home from '@/renderer/page/Home'
import Setting from '@/renderer/page/Setting'
import Tray from '@/renderer/page/Tray'
import Dialog from '@/renderer/page/Dialog'
import FeedBack from '@/renderer/page/FeedBack'
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
      path: '/dialog',
      name: 'Dialog',
      component:Dialog
    },
    {
      path: '/feedBack',
      name: 'FeedBack',
      component:FeedBack
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
