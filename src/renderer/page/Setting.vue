<template>
    <el-container class="al-main">
        <el-header class="rheader rheaderd">
            <div class="rheaderd_div">
                <span>{{$t('preferences')}}</span>
            </div>
            <div class="rheader_1">
                <div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 1)}" @click="chose_block(1)">
                    <div class="rheader_imgd"><img src="../assets/general.png"></div>
                    <span>{{$t('general')}}</span>
                </div>

                <div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 2)}" @click="chose_block(2)">
                    <div class="rheader_imgd"><img src="../assets/notification.png"></div>
                    <span>{{$t('notification')}}</span>
                </div>

                <!--<div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 3)}" @click="chose_block(3)">-->
                    <!--<div class="rheader_imgd"><img src="../assets/ignoredisk.png"></div>-->
                    <!--<span>{{$t('ignoredisk')}}</span>-->
                <!--</div>-->

                <div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 4)}" @click="chose_block(4)">
                    <div class="rheader_imgd"><img src="../assets/privacy.png"></div>
                    <span>{{$t('privacy')}}</span>
                </div>


                <div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 5)}" @click="chose_block(5)">
                    <div class="rheader_imgd"><img src="../assets/update.png"></div>
                    <span>{{$t('update')}}</span>
                </div>

            </div>
        </el-header>


        <el-main class="lmain">
            <el-form class="main-from main-from_b1" v-if="select_block == 1">
                <div class="main-from_div">
                    <div class="main-from_div_1">
                        <div class="main-from_div_1_1">
                            <div class="mb10">
                                <span>NTFSTool for Mac {{$t('menu')}}</span>
                            </div>

                            <div class="translate-lang" @click="openTransLang()">
                                ⛳️ {{$t('Helpsoftwaretranslation')}}
                            </div>

                            <div class="main-from_div_1_1-div mb10">
                                <!--<div class="main-from_div_1_1-div_1">-->
                                    <!--<div style="display: flex;flex-direction: column;justify-content: center">-->
                                        <!--<i  v-if="show_menu" class="iconfont icondot" style=" font-size: 10px;color: #67c23a">&#xe607;</i>-->
                                        <!--<i  v-if="!show_menu" class="iconfont icondot" style=" font-size: 10px;color: orangered">&#xe607;</i>-->
                                    <!--</div>-->

                                    <!--<div style="display: flex;flex-direction: column;justify-content: center">-->
                                        <!--<span v-if="show_menu" class="main-from_div_1_1-div_1span" style="">{{$t('activated')}}</span>-->
                                        <!--<span v-if="!show_menu" class="main-from_div_1_1-div_1span" style="color: orangered">{{$t('closed')}}</span>-->
                                    <!--</div>-->
                                <!--</div>-->
                                <!--<div>-->
                            </div>

                            <!--<div class="font12 mb20">-->
                                <!--<span v-if="show_menu">{{$t('notice_cannot_do_disk01')}}</span>-->
                                <!--<span v-if="!show_menu">{{$t('notice_cannot_do_disk02')}}</span>-->
                            <!--</div>-->

                            <div style="position: absolute;bottom: 30px;">
                                <el-checkbox v-model="auto_run"  @change="changeAutoRun()"  :label="$t('Followthesystemstartup')" name="type"></el-checkbox>
                            </div>

                            <div style="position: absolute;bottom: 0;">
                                <el-checkbox v-model="auto_mount"  @change="changeAutoMount()"  :label="$t('Automatically_mount_NTFS_disk')" name="type"></el-checkbox>
                            </div>
                        </div>
                        <div class="main-from_div_1_2">
                            <div class="main-from_div_1_2_1">
                                <div class="main-from_div_1_2_1_div block">
                                    <el-carousel
                                            ref="carouselObj"
                                            trigger="click"
                                            :autoplay="false"
                                            indicator-position="none"
                                            arrow="never"
                                            height="200px">
                                        <el-carousel-item class="main-from_div_1_2_1_div_item" v-for="item in 4"
                                                          :key="item">
                                            <img style="height: 100%;" src="../assets/theme2.png">
                                        </el-carousel-item>
                                    </el-carousel>
                                </div>
                            </div>
                            <el-form-item class="main-from_div_1_2_2" :label="$t('theme')">
                                <el-radio-group v-model="theme" @change="changeTheme()">
                                    <el-radio label="0" disabled>{{$t('system')}}</el-radio>
                                    <el-radio label="1" disabled>{{$t('dark')}}</el-radio>
                                    <el-radio label="2">{{$t('light')}}</el-radio>
                                </el-radio-group>
                            </el-form-item>
                        </div>
                    </div>
                </div>
                <div class="main-from_div2">
                    <div class="main-from_div2_1">
                        <table class="block1_table">
                            <tr>
                                <td><span class="mr20">{{$t('lang_select')}}</span></td>
                                <td>
                                    <select class="al_select al_select_lang"  v-model="lang" @change="changeLang()">
                                        <option
                                                v-for="item, index in this.lang_list" :key="item.text"  :value="item.val">{{item.text}}
                                        </option>
                                    </select>
                                </td>
                            </tr>

                            <tr>
                                <td><span class="mr20">{{$t('Howtodealwithmountingbadvolumes')}}</span></td>
                                <td>
                                    <select class="al_select al_select_lang" v-model="install_bug_type" @change="changeInstallBugType()">
                                        <option value="auto_solve">{{$t('Automaticprocessing')}}</option>
                                        <option value="tip_solve">{{$t('Promptbeforeprocessing')}}</option>
                                        <option value="no_solve">{{$t('Donothing')}}</option>
                                    </select>
                                </td>
                            </tr>

                            <tr>
                                <td><span class="mr20">{{$t('Howtodealwithhibernation')}} Windows</span></td>
                                <td>
                                    <select class="al_select al_select_lang" v-model="how_restart_window" @change="changeHowRestartWindow()">
                                        <option value="change_to_bacground">{{$t('Switchtobackground')}}</option>
                                        <option value="tip_solve">{{$t('Askhow')}}</option>
                                        <option value="auto_close">{{$t('Autoclose')}}</option>
                                    </select>
                                </td>
                            </tr>
                        </table>
                    </div>

                </div>
            </el-form>


            <el-form class="main-from" v-if="select_block == 2">

                <el-form-item :label="$t('notice') + ':'" class="mb20">
                    <el-checkbox v-model="mount_show_msg"  @change="changeMountShowMsg()"  class="mb20" :label="$t('Shownotificationswhenmountedandlaunched')" name="type"></el-checkbox>

                    <el-checkbox v-model="update_show_msg"  @change="changeUpdateShowMsg()"  :label="$t('Shownotificationswhenupdatesareavailable')" name="type"></el-checkbox>
                    <span class="sub_form_title">
                        {{$t('Shownotificationswhenanupdatedversionisofficiallyavailable')}}
                    </span>
                    <div class="mb20"></div>
                    <el-checkbox v-model="error_disk_msg"  @change="changeErrorDiskMsg()"  :label="$t('Notifywhendiskvolumeisabnormal')" name="type"></el-checkbox>
                    <span class="sub_form_title">
                        {{$t('Diskvolumemaybeabnormalduetoabnormaldisconnection')}}
                    </span>
                </el-form-item>
            </el-form>

            <el-form class="main-from" v-if="select_block == 3">
                <div style="margin: 20px 0">
                    <span>{{$t('Disksintheignore')}}</span>
                </div>


                <div>
                    <el-transfer
                            v-model="value"
                            :data="data"
                            :titles="['Disk', 'IgnoreList']"
                    ></el-transfer>
                </div>
            </el-form>


            <el-form class="main-from main-from-b4" v-if="select_block == 4">
                <div>
                    <span style="font-size: 14px;"> NtfsTool {{$t('Alldatacollectedcanbeviewedintheupdatedprivacypolicy')}}</span>
                </div>

                <div class="main-from_div_1_1-div_1" @click="openPrivacyUrl">
                    <div style="display: flex;flex-direction: column;justify-content: center">
                        <i class="iconfont iconjump06" style="font-size: 16px;color: black;">
                            &#xe648;
                            <span style="    font-size: 12px;font-family: cursive;
    vertical-align: middle;
    margin-left: 3px;">{{$t('Readtheprivacypolicy')}}</span>
                        </i>
                    </div>
                </div>
            </el-form>

            <el-form class="main-from" v-if="select_block == 5">
                <el-form-item :label="$t('update') + ':'" class="mb20">
                    <el-checkbox  v-model="auto_check"  @change="changeAutoCheck()"  :label="$t('Checkforupdatesautomatically')" name="type"></el-checkbox>
                    <el-checkbox  v-model="auto_beta_update"  @change="changeAutoBetaUpdate()"  :label="$t('DetectBetaversionupdates')" name="type"></el-checkbox>
                    <span class="sub_form_title">
                        {{$t('Pleaseupdatetothebetaversion1')}}
                    </span>
                    <div>
                        <el-button @click="checkSoftUpdate">{{$t('Checkforupdates')}}</el-button>
                        <el-button @click="resetConf">{{$t('Resetallconfiguration')}}</el-button>
                    </div>
                </el-form-item>
            </el-form>
        </el-main>
    </el-container>
</template>
<script>
    import setting from '@/renderer/lib/setting.js'
    export default setting
</script>

<style scoped src="@/renderer/theme/setting.css"></style>

<style scoped>

</style>