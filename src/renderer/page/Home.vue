<template>
    <el-container class="nt-main">
        <div class="nt-header">
            <div class="title">
                <span>NTFSTool</span>
            </div>
            <div class="body">
                <div class="l">
                    <div class="bar" style="position: relative;" @blur="menu_top_box = false" tabindex="0">
                        <div class="i" @click="menu_top_box = !menu_top_box">
                            <i class="iconfont iconpush" style="color: rgb(110, 110, 112);">&#xe610;</i>
                            <i class="iconfont iconpush" style="color: rgb(110, 110, 112);">&#xe6e5;</i>
                        </div>
                        <span class="t">Menu</span>

                        <div class="menu_box menu_box_tf" v-show="menu_top_box">
                            <div style="padding: 0 20px;">
                                显示设置
                            </div>
                            <span class="line"></span>
                            <div @click="menu_show_set('inner')">
                                <i class="iconfont iconpush">{{menu_show_conf && typeof menu_show_conf.inner != "undefined" &&
                                    menu_show_conf.inner == true ? "&#xe618;" : "&#xef96;"}}</i>
                                系统卷
                            </div>
                            <div @click="menu_show_set('ext')">
                                <i class="iconfont iconpush">{{menu_show_conf && typeof menu_show_conf.ext != "undefined" &&
                                    menu_show_conf.ext == true ? "&#xe618;" : "&#xef96;"}}</i>
                                扩展卷
                            </div>
                            <div @click="menu_show_set('image')">
                                <i class="iconfont iconpush">{{menu_show_conf && typeof menu_show_conf.image != "undefined" &&
                                    menu_show_conf.image == true ? "&#xe618;" : "&#xef96;"}}</i>
                                镜像卷
                            </div>
                        </div>
                    </div>


                </div>
                <div class="m">

                    <div class="bar">
                        <div class="i" @click="openDisk(select_item)">
                            <i class="iconfont iconpush" style="color: rgb(110, 110, 112);">&#xe7d0;</i>
                        </div>
                        <span class="t">打开</span>
                    </div>

                    <div v-bind:class="['bar',select_item.group == 'ext' && !(typeof select_item.writable == 'string' && select_item.writable.toLowerCase() == 'no') ? '' : 'bar_disable']">
                        <div class="i" @click="select_item.group == 'ext' && !(typeof select_item.writable == 'string' && select_item.writable.toLowerCase() == 'no')
                         && changeVolumeName(select_item)">
                            <i class="iconfont iconpush" style="color: rgb(110, 110, 112);">&#xe6c4;</i>
                        </div>
                        <span class="t">重命名</span>
                    </div>

                    <div v-bind:class="['bar',select_item.group != 'inner' ? '' : 'bar_disable']">
                        <div class="i"
                             @click="select_item.group != 'inner' && (select_item.mounted == true ? uMountDisk(select_item) : mountDisk(select_item))">
                            <i class="iconfont iconpush" style="color: rgb(110, 110, 112);">&#xeb99;</i>
                        </div>

                        <span v-if="select_item.mounted == true" class="t">卸载</span>
                        <span v-else class="t">挂载</span>
                    </div>


                </div>
                <div class="r">
                    <div class="bar">
                        <div class="i" @click="openAboutPage">
                            <i class="iconfont iconpush" style="color: rgb(110, 110, 112);">&#xe63f;</i>
                        </div>
                        <span class="t">About</span>
                    </div>
                </div>
            </div>
        </div>

        <div class="nt-body">
            <el-col class="al-lblock" style="width: 35%;">
                <div class="shadow_nt"></div>

                <el-main :class="['lmain',!scrollShow?'hide_scroll' : '']" @scroll="handleScroll">
                    <div v-for="(list, index)  in diskMap">
                        <div class="lm-t" v-if="index == 'inner' && list.length > 0">
                            <span>{{$t('Systemvolume')}}</span>
                        </div>

                        <div class="lm-t" v-if="index == 'ext' && list.length > 0">
                            <span>{{$t('Externalvolume')}}</span>
                        </div>

                        <div class="lm-t" v-if="index == 'image' && list.length > 0">
                            <span>{{$t('ImageVolume')}}</span>
                        </div>

                        <div v-bind:class="{'lm-block-active':(select_item.bsd_name == item.bsd_name)}"
                             class="lm-block"
                             v-for="item in list"
                             @click="choseDisk(item)"
                             v-on:dblclick="(select_item.bsd_name == item.bsd_name) && openDisk(item)"
                        >
                            <div class="lm-block_1">
                                <div v-if="item.mounted == true"
                                     @click="item.group!= 'inner' &&  uMountDisk(item)" @click.stop>

                                    <i v-if="index == 'inner'" class="iconfont iconpush"
                                       style="color: rgb(110, 110, 112);">&#xe61a;</i>
                                    <i v-else class="iconfont iconpush" style="color: rgb(110, 110, 112);"
                                       title="点击卸载磁盘">&#xe769;</i>
                                </div>

                                <div v-else @click="mountDisk(item)" @click.stop title="点击挂载磁盘">
                                    <i class="iconfont iconrepush1" style="color: #6e6e70;font-size: 18px;">&#xe609;</i>
                                </div>
                            </div>

                            <div class="lm-block_2">
                                <div v-if="index == 'inner'">
                                    <img src="../assets/disk04.png">
                                </div>

                                <div v-if="index == 'ext'">
                                    <img v-if="typeof item.file_system == 'string' && item.file_system.toLowerCase().indexOf('ntfs') >= 0"
                                         src="../assets/disk01.png">

                                    <img v-else src="../assets/disk02.png">
                                </div>

                                <div v-if="index == 'image'">
                                    <img src="../assets/disk03.png">
                                </div>
                            </div>

                            <div class="lm-block_3">
                                <div class="lm-block_3n_d">
                                    <div class="lm-block_3n_d_1">
                                        <div class="lm-block_3n_d_1_flex" :title="item._name">


                                            <!--<div class="block_3n_d_1_sd">-->
                                                <!--<span v-if="item.mounted"-->
                                                      <!--class="block_3n_d_1_sdp mounted_dot"></span>-->
                                                <!--<span v-else class="block_3n_d_1_sdp unmounted_dot"></span>-->
                                            <!--</div>-->


                                            <!--<span v-if="typeof item.status != 'undefined' && (item.status == 0)"-->
                                                  <!--class="block_3n_d_1_sp">-->
                                                <!--{{$t('Mounting')}}...-->
                                            <!--</span>-->

                                            <span class="block_3n_d_1_sp">

                                                <i v-if="item.mount_status"  v-html="item.mount_status" class="iconfont iconrepush1" style="color: #51b7f6;font-size: 12px;">
                                                    {{item.mount_status}}
                                                </i>

                                                <i v-else-if="!item.mount_status && item.mounted" class="iconfont iconrepush1" style="color: #51b7f6;font-size: 12px;">
                                                    &#xe67a;
                                                </i>

                                                 <i v-else-if="!item.mount_status && !item.mounted" class="iconfont iconrepush1" style="color: #c0c4cc;font-size: 12px;">
                                                    &#xe67a;
                                                </i>

                                                <i v-else  class="iconfont iconrepush1" style="color: #51b7f6;font-size: 18px;">
                                                    &#xe67a;
                                                </i>


                                                {{item._name.length > 16 ? (item._name.substring(0,16) + "...") : (item._name ? item._name : "未命名")}}
                                            </span>

                                        </div>

                                        <div class="block_3n_d_1_sd"
                                             title="点击修复只读挂载"
                                             @click="writeable_fix(item)" @click.stop
                                             v-if="item.group == 'ext' && (typeof item.writable == 'string' && item.writable.toLowerCase() == 'no')">
                                            <span class="readonly">
                                                {{$t('Readonly')}}
                                            </span>
                                        </div>

                                        <div v-else class="lm-block_3n_d_1_fdf">
                                            {{item.total_size}}
                                        </div>
                                    </div>

                                    <div v-if="item.mounted == true" class="lm-block_3n_d_2">
                                        <div class="lm-block_3n_d_2_bar"
                                             v-bind:style="{backgroundColor: select_item.bsd_name == item.bsd_name ? '#e4e4e4' : 'white'}">
                                            <span v-bind:style="{ width: item.percentage + '%', backgroundColor: '#66b1ff' }"></span>
                                        </div>
                                    </div>

                                    <div v-else class="lm-block_3n_d_2">
                                        <span title="点击挂载" class="umounted" @click="mountDisk(item)" @click.stop>
                                            {{$t('Unmounted')}}
                                        </span>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </el-main>


                <el-footer class="lfooter">
                    <div class="lfooter_div" @click="refreshDevice">
                        <div style="display: flex;">
                            <!--<div v-if="loading == -1" class="mint-spinner-snake">-->
                            <!--<i class="iconfont iconiconsxz i-loading">&#xe623;</i>-->
                            <!--</div>-->

                            <div v-if="loading == -1" class="mint-spinner-snake">
                                <i class="iconfont iconiconsxz" style="color: #585858">&#xe623;</i>
                            </div>

                            <div v-else>
                                <i class="iconfont iconiconsxz" style="color:transparent;">&#xe623;</i>
                            </div>


                            <span class="version_span">
                                NTFSTool V{{version}}
                            </span>

                            <!--<i v-else class="iconfont iconshuaxin i-loading2">&#xe650;</i>-->
                        </div>


                    </div>
                </el-footer>
            </el-col>

            <el-col class="al-rblock" style="width: 65%;">
                <el-main class="ar-main">
                    <div>
                        <div class="rmain">
                            <div class="rmain_l">
                                <div class="rmain_l_box" v-if="select_item.group == 'inner'">
                                    <img src="../assets/disk04.png">
                                </div>

                                <div class="rmain_l_box" v-else-if="select_item.group == 'ext'">
                                    <img v-if="select_item.file_system.toLowerCase().indexOf('ntfs') >= 0"
                                         src="../assets/disk01.png">

                                    <img v-else src="../assets/disk02.png">
                                </div>

                                <div class="rmain_l_box" v-else="select_item.group == 'image'">
                                    <img src="../assets/disk03.png">
                                </div>
                            </div>

                            <div class="rmain_r">
                                <div class="r-bt">
                                    <input v-if="select_item.group == 'ext' && select_item.mounted == true && select_item.writable == 'yes'"
                                           @keyup.enter="submit_change_name"
                                           ref="name_input"
                                           :pre_value="select_item._name"
                                           class="l-input" type="text" v-model="select_item._name">

                                    <div v-else class="l">
                                        {{select_item._name ? select_item._name : "未命名"}}
                                    </div>
                                </div>

                                <div class="rmain_r_2">
                                    <div>
                                        <span>
                                            Status
                                        </span>

                                        <span v-if="select_item.mounted == true && select_item.writable == 'yes'"
                                              style="color: #67c23a">
                                             write_mounted
                                        </span>

                                        <span v-else-if="select_item.mounted == true && select_item.writable != 'yes'"
                                              style="color: #f56c6c;">
                                            只读挂载
                                            <span @click="writeable_fix"
                                                  v-if="typeof select_item.file_system == 'string' && select_item.file_system.toLowerCase().indexOf('ntfs') >= 0"
                                                  class="writeable_fix">
                                                 修复
                                             </span>
                                        </span>

                                        <span v-else style="color:#e6a23c"> unmount </span>
                                    </div>

                                    <div>
                                        <span>bsd_name</span>
                                        <span>{{select_item.bsd_name}}</span>
                                    </div>


                                    <div>
                                        <span>file_system</span>
                                        <span>{{select_item.file_system}}</span>
                                    </div>

                                    <div style="border-bottom:none;">
                                        <span>mount_point</span>
                                        <span>{{select_item.mount_point ? select_item.mount_point : "-"}}</span>
                                    </div>


                                </div>
                            </div>
                        </div>

                        <div class="rmain2">
                            <div class="rmain2_2">
                                <div class="rmain2_2_bar" v-if="typeof select_item.percentage !='undefined'">
                                    <span v-bind:style="{ width: select_item.percentage + '%', backgroundColor: select_item.percentage_color }"></span>
                                </div>
                            </div>

                            <div class="rmain2_3">
                                <div class="rmain2_3_1">
                                    <table class="rmain2_3_1_r">
                                        <tr>
                                            <td>
                                                <i class="iconfont icondot mr5" style=" font-size: 10px;color: #5586db">&#xe607;</i>
                                            </td>
                                            <td>{{$t('total')}}</td>
                                        </tr>

                                        <tr>
                                            <td></td>
                                            <td class="rmain2_3_1_rdtext">{{select_item.total_size}}</td>
                                        </tr>
                                    </table>

                                    <table class="rmain2_3_1_r">
                                        <tr>
                                            <td>
                                                <i class="iconfont icondot mr5" style=" font-size: 10px;color: #d12890">&#xe607;</i>
                                            </td>
                                            <td>{{$t('used')}}</td>
                                        </tr>

                                        <tr>
                                            <td></td>
                                            <td class="rmain2_3_1_rdtext">
                                                {{select_item.used_size}}
                                            </td>
                                        </tr>
                                    </table>


                                    <table class="rmain2_3_1_r">
                                        <tr>
                                            <td>
                                                <i class="iconfont mr5"
                                                   style=" font-size: 10px;color: #f1e600">&#xe607;</i>
                                            </td>
                                            <td>{{$t('available')}}</td>
                                        </tr>
                                        <tr>
                                            <td></td>
                                            <td class="rmain2_3_1_rdtext">
                                                {{select_item.free_size}}
                                            </td>
                                        </tr>
                                    </table>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="mright-dir" v-if="select_item.mounted">
                        <div class="goods">
                            <img @click="openDisk(select_item)" src="../assets/opendisk.svg"
                                 :title="$t('Clicktoopenthedisk')">
                        </div>
                    </div>
                </el-main>

                <el-footer class="rfooter">
                    <div class="rfooter_1" @blur="menu_box = false" tabindex="0">
                        <div>
                            <img @click="menu_box = !menu_box" src="../assets/setting2.svg">
                        </div>

                        <div class="menu_box" v-show="menu_box">
                            <div @click="exitAll">{{$t('Quit')}}</div>
                            <span class="line"></span>
                            <div @click="openFeedBackPage">{{$t('Submitfeedback')}}</div>
                            <div @click="openSettingPage"> {{$t('preferences')}}</div>
                            <span class="line"></span>
                            <div @click="openAboutPage"> {{$t('About')}}</div>
                            <span class="line"></span>
                            <div v-show="showDebugMenu" @click="clearPwd">{{$t('Clearpassword')}}</div>
                        </div>
                    </div>
                </el-footer>
            </el-col>
        </div>
    </el-container>
</template>
<script>
    import home from '@/renderer/lib/home.js'

    export default home
</script>
<style scoped src="@/renderer/theme/home.scss" lang="scss"></style>