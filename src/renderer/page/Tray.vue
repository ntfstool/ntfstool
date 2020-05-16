<template>
    <el-container class="al-main">
        <div ref="trayref_h" class="topsj">
            <div>
                <i class="iconfont ico_color">&#xe666;</i>
            </div>
        </div>

        <div ref="trayref" class="trayref" @blur="test('aaa')">
            <div class="trayref_h">
                <div class="trayref_h_1">
                    <div class="trayref_h_1_1">
                        <div class="trayref_h_1_1sub_1">
                            <i @click="openWebsite" class="iconfont mr10 ico_color" @mouseover="setTitle('openWebsite')"
                               @mouseout="setTitle()">&#xe617;</i>
                            <i class="iconfont" style="transform: scaleX(-1);color: transparent;">&#xe600;</i>
                        </div>
                    </div>

                    <div class="trayref_h_1_2">
                        <div class="trayref_h_1_2_1" @click="startDebug">
                            <span>{{title}}</span>
                        </div>
                    </div>


                    <div class="trayref_h_1_3">
                        <div class="trayref_h_1_3_1" style="position: relative" @blur="menu_box1 = false" tabindex="0">
                            <i class="iconfont mr10 ico_color" @click="openHomePage"
                               @mouseover="setTitle('Open Desktop')" @mouseout="setTitle()">&#xe8e2;</i>
                            <i class="iconfont ico_color"
                               @click="openMenuBox('menu_box1')"

                               @mouseover="setTitle('Open Menu')" @mouseout="setTitle()"
                               style="transform: scaleX(-1);">&#xe601;</i>
                            <div class="menu_box" v-show="menu_box1">
                                <div @click="openAboutPage">{{$t('About')}}</div>
                                <span class="line"></span>
                                <div @click="openHomePage">{{$t('Opendesktop')}}</div>
                                <div @click="openSettingPage">{{$t('preferences')}}</div>
                                <div @click="openFeedBackPage">{{$t('Submitfeedback')}}</div>
                                <span class="line"></span>
                                <div @click="exitAll"> {{$t('Quit')}}</div>

                                <span v-show="showDebugMenu" class="line"></span>
                                <div v-show="showDebugMenu" @click="openDialog">Dialog</div>
                                <div v-show="showDebugMenu" @click="openLog">openLog</div>
                            </div>
                        </div>
                    </div>

                </div>
            </div>

            <div v-for="(list,index)  in diskMap">
                <div v-for="(item)  in list">
                    <!--disk block-->
                    <div class="diskb"
                         v-on:dblclick="openDisk(item)"
                    >
                        <div class="diskb_1">
                            <div v-if="item.mounted == true"
                                 @click="uMountDisk(item)" @click.stop>
                                <i v-if="index == 'inner'" class="iconfont ico_color mr10">&#xe61a;</i>
                                <i v-else class="iconfont ico_color mr10" title="点击卸载磁盘">&#xe769;</i>
                            </div>

                            <div v-else @click="mountDisk(item)" @click.stop title="点击挂载磁盘">
                                <i class="iconfont ico_color mr10">&#xe609;</i>
                            </div>
                        </div>

                        <div class="diskb_2">
                            <div style="display: flex;justify-content: center;">
                                <div v-if="index == 'inner'">
                                    <img src="../assets/disk04.png">
                                </div>

                                <div v-else-if="index == 'ext'">
                                    <img v-if="typeof item.file_system == 'string' && item.file_system.toLowerCase().indexOf('ntfs') >= 0"
                                         src="../assets/disk01.png">

                                    <img v-else src="../assets/disk02.png">
                                </div>

                                <div v-else="index == 'image'">
                                    <img src="../assets/disk03.png">
                                </div>
                            </div>
                        </div>


                        <div class="diskb_3" :title="$t('dbclicktoopen')">
                            <div class="diskb_3_1">
                                <div>
                                    <i class="iconfont ico_color"  v-bind:style="{color:item.percentage_color}">&#xe607;</i>

                                    <span v-if="typeof item.status != 'undefined' && (item.status == 0)">
                                        mounting...
                                    </span>

                                    <span v-else>{{item._name}}</span>
                                </div>


                                <div v-if="typeof item.writable == 'string' && item.writable.toLowerCase() == 'no'" class="readonly_txt">
                                    {{$t('Readonly')}}
                                </div>

                                <div  v-else-if="item.mounted == false" class="readonly_txt" style="background-color: #c0c4cc;cursor: pointer" @click="mountDisk(item)" @click.stop>
                                    {{$t('Unmounted')}}
                                </div>
                            </div>

                            <div class="diskb_3_2">
                                <div class="diskb_3_2_1"
                                     v-bind:style="{ width: item.percentage + '%',background:item.percentage_color}">
                                </div>
                            </div>

                            <div class="diskb_3_3">
                                <div style="font-size: 14px;">
                                    {{$t('total')}}: {{item.total_size}}
                                </div>
                                <div style="    font-size: 14px;">
                                    {{$t('used')}}: {{item.used_size}}
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </el-container>
</template>
<script>
    import tray from '@/renderer/lib/tray.js'

    export default tray
</script>


<style scoped src="@/renderer/theme/tray.css"></style>

<style scoped>
    .el-submenu__title {
        height: 100% !important;
        line-height: 100% !important;
    }
</style>