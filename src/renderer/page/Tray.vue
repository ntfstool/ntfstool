<template>
    <el-container class="al-main">
        <!--小三角-->
        <div ref="trayref_h" class="topsj">
            <div>
                <i class="iconfont ico_color">&#xe666;</i>
            </div>
        </div>

        <div ref="trayref" class="trayref" @blur="test('aaa')">
            <!--头部-->
            <div class="trayref_h">
                <div class="trayref_h_1">
                    <div class="trayref_h_1_1">
                        <div class="trayref_h_1_1sub_1">
                            <i @click="pushAll" class="iconfont mr10 ico_color" @mouseover="setTitle('全部推出')"
                               @mouseout="setTitle()">&#xe769;</i>
                            <i class="iconfont" style="transform: scaleX(-1);color: transparent;">&#xe601;</i>
                        </div>
                    </div>

                    <div class="trayref_h_1_2">
                        <div class="trayref_h_1_2_1" @click="startDebug">
                            <span>{{title}}</span>
                        </div>
                    </div>


                    <div class="trayref_h_1_3">
                        <div class="trayref_h_1_3_1" style="position: relative">
                            <i class="iconfont mr10 ico_color"  @click="openHomePage" @mouseover="setTitle('打开桌面')" @mouseout="setTitle()">&#xe8e2;</i>
                            <i class="iconfont ico_color"
                               @click="openMenuBox('menu_box1')"

                               @mouseover="setTitle('打开菜单')" @mouseout="setTitle()"
                               style="transform: scaleX(-1);">&#xe601;</i>


                            <div class="menu_box" v-show="menu_box1">
                                <div @click="openAboutPage">关于</div>
                                <span class="line"></span>
                                <div @click="openHomePage">打开桌面</div>
                                <div @click="openSettingPage">偏好设置</div>
                                <div @click="openFeedBackPage">提交反馈</div>
                                <span class="line"></span>
                                <div @click="exitAll">退出</div>

                                <span v-show="showDebugMenu" class="line"></span>
                                <div v-show="showDebugMenu" @click="openDialog">Dialog</div>
                                <div v-show="showDebugMenu" @click="openLog">openLog</div>
                            </div>
                        </div>
                    </div>

                </div>
            </div>

            <!--disk block-->
            <div class="diskb">
                <div class="diskb_1">
                    <div>
                        <i class="iconfont ico_color mr10">&#xe769;</i>
                    </div>
                </div>

                <div class="diskb_2">
                    <div style="display: flex;
    justify-content: center;">
                        <img src="../assets/disk2.png">
                    </div>
                </div>


                <div class="diskb_3">
                    <div class="diskb_3_1">
                        <div>
                            <i class="iconfont ico_color">&#xe607;</i>
                            <span>本地磁盘</span>
                        </div>
                        <div style="font-size: 15px;">
                            Readonly
                        </div>
                    </div>

                    <div class="diskb_3_2">
                        <div class="diskb_3_2_1"></div>
                    </div>

                    <div class="diskb_3_3">
                        <div style="font-size: 15px;">
                            <!--Reboot DIsk-->
                            2133.GB
                        </div>
                        <div style="    font-size: 15px;">2133.GB 可用</div>
                    </div>
                </div>
            </div>


            <!--disk block-->
            <div class="diskb">
                <div class="diskb_1">
                    <div>
                        <i class="iconfont icontuichu mr10">&#xe769;</i>
                    </div>
                </div>

                <div class="diskb_2">
                    <div>
                        <img src="../assets/disk2.png">
                    </div>
                </div>


                <div class="diskb_3">
                    <div class="diskb_3_1">
                        <div>
                            <i class="iconfont ico_color">&#xe607;</i>
                            <span>本地磁盘</span>
                        </div>
                        <div>
                            Readonly
                        </div>
                    </div>

                    <div class="diskb_3_2">
                        <div class="diskb_3_2_1"></div>
                    </div>

                    <div class="diskb_3_3">
                        <div>
                            <!--Reboot DIsk-->
                            2133.GB
                        </div>
                        <div>2133.GB 可用</div>
                    </div>
                </div>
            </div>


            <!--disk block-->
            <div class="diskb">
                <div class="diskb_1">
                    <div>
                        <i class="iconfont ico_color mr10">&#xe769;</i>
                    </div>
                </div>

                <div class="diskb_2">
                    <div>
                        <img src="../assets/disk2.png">
                    </div>
                </div>


                <div class="diskb_3">
                    <div class="diskb_3_1">
                        <div>
                            <i class="iconfont ico_color">&#xe607;</i>
                            <span>本地磁盘</span>
                        </div>
                        <div>
                            Readonly
                        </div>
                    </div>

                    <div class="diskb_3_2">
                        <div class="diskb_3_2_1"></div>
                    </div>

                    <div class="diskb_3_3">
                        <div>
                            <!--Reboot DIsk-->
                            2133.GB
                        </div>
                        <div>2133.GB 可用</div>
                    </div>
                </div>
            </div>

            <!--disk block-->
            <div class="diskb">
                <div class="diskb_1">
                    <div>
                        <i class="iconfont ico_color mr10">&#xe769;</i>
                    </div>
                </div>

                <div class="diskb_2">
                    <div>
                        <img src="../assets/disk2.png">
                    </div>
                </div>


                <div class="diskb_3">
                    <div class="diskb_3_1">
                        <div>
                            <i class="iconfont ico_color">&#xe607;</i>
                            <span>本地磁盘</span>
                        </div>
                        <div>
                            Readonly
                        </div>
                    </div>

                    <div class="diskb_3_2">
                        <div class="diskb_3_2_1"></div>
                    </div>

                    <div class="diskb_3_3">
                        <div>
                            <!--Reboot DIsk-->
                            2133.GB
                        </div>
                        <div>2133.GB 可用</div>
                    </div>
                </div>
            </div>


            <!--disk block-->
            <div class="diskb">
                <div class="diskb_1">
                    <div>
                        <i class="iconfont ico_color mr10">&#xe769;</i>
                    </div>
                </div>

                <div class="diskb_2">
                    <div>
                        <img src="../assets/disk2.png">
                    </div>
                </div>


                <div class="diskb_3">
                    <div class="diskb_3_1">
                        <div>
                            <i class="iconfont ico_color">&#xe607;</i>
                            <span>本地磁盘</span>
                        </div>
                        <div>
                            Readonly
                        </div>
                    </div>

                    <div class="diskb_3_2">
                        <div class="diskb_3_2_1"></div>
                    </div>

                    <div class="diskb_3_3">
                        <div>
                            <!--Reboot DIsk-->
                            2133.GB
                        </div>
                        <div>2133.GB 可用</div>
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