<template>
    <el-container class="al-main">
        <div class="sudo_box" v-show="show_index == 'sudo_box'">
            <div class="left">
                <img src="../../../static/lock.svg">
            </div>
            <div class="right">
                <div class="title">
                    <span>“NtfsTool” 正在尝试调用系统磁盘操作权限。</span>
                    <span>输入密码以允许此次操作。</span>
                </div>
                <div class="input">
                    <table>
                        <tr>
                            <td class="aleft">用户名：</td>
                            <td>
                                <input v-if="workname" name="username" v-model="workname" disabled>
                                <input v-else name="username" v-model="workname">
                            </td>
                        </tr>
                        <tr>
                            <td class="aleft">密码：</td>
                            <td><input name="workpwd" v-on:keyup.enter="checkSudoPwd"
                                       ref="workpwd" type="password" v-model="workpwd"></td>
                        </tr>
                    </table>
                </div>
                <div class="btn-box">
                    <span :class="[btnDisable ? 'btn-disable' : 'btn-default']" @click="cancel()">取消</span>

                    <button :class="[btnDisable ? 'btn-active-disable' :  'btn-active']" @click="checkSudoPwd()">解锁
                    </button>
                </div>
            </div>
        </div>

        <div class="install_fuse" v-show="show_index == 'install_fuse'">
            <div>
                <div class="iboxh">
                     <img style="height: 80px;user-select: none;"
                          src="../../../static/osxfuse-home.png">
                    <div class="iboxh_text">
                        <div>检测到系统缺少Fuse磁盘内核依赖,请点击确认并正常安装,方可继续使用本软件.</div>
                        <div style="margin-top: 5px;">
                            <i class="iconfont iconjump06" style="font-size: 16px;color: black;"  @click="goFuseWebSite">
                                &#xe648;
                                <span style="    font-size: 12px;font-family: cursive;vertical-align: middle;">
                                    Fuse介绍
                                </span>
                            </i>
                        </div>
                    </div>
                </div>
                <div class="btn-box" style="display: flex;justify-content: flex-end;margin: 0 20px;">
                    <span :class="[btnDisable ? 'btn-disable' : 'btn-default']" @click="cancel_installfuse()">取消</span>

                    <button :class="[btnDisable ? 'btn-active-disable' :  'btn-active']" @click="installfuse()">确定
                    </button>
                </div>
            </div>
        </div>

        <div class="about_box" v-show="show_index == 'about_box'">
            <div class="main_about">
                <div>
                    <img @click="openMainsite()" style="width: 80px;height: 80px;user-select: none;"
                         src="../assets/logo.png">
                </div>
                <div style="display: none;">
                    <img :src="statisticsImg">
                </div>
                <div style="font-weight: bold;">
                    NTFSTool for MAC
                </div>

                <!--<div @click="test()" style="font-weight: bold;">-->
                    <!--Flash Test-->
                <!--</div>-->

                <div style="font-size: 10px;">
                    系统 {{os_version}}
                </div>

                <div style="font-size: 10px;">
                    系列号 {{serial_number}}
                </div>

                <div style="font-size: 10px;">
                    版本 {{version}} (3245) <i @click="openGitHub()" class="iconfont" style="margin-left:5px;font-size: 12px;
    color: black;">&#xe691;</i>
                </div>
                <div style="    font-size: 11px;">
                    Copyright © 2020 <u style="margin: 0 5px;cursor: pointer" @click="openMainsite()">NtfsTool</u> Inc.
                </div>
            </div>
        </div>
    </el-container>
</template>
<script>
    import dialog from '@/renderer/lib/dialog.js'
    export default dialog
</script>

<style scoped src="../theme/dialog.scss" lang="scss"></style>