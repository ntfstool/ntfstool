<template>
    <el-container class="al-main">
        <el-header class="rheader rheaderd">
            <div class="rheaderd_div">
                <!--偏好设置-->
                <span>{{$t('preferences')}}</span>
            </div>
            <div class="rheader_1">
                <div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 1)}" @click="chose_block(1)">
                    <div class="rheader_imgd"><img src="../assets/general.png"></div>
                    <!--常规-->
                    <span>{{$t('general')}}</span>
                </div>

                <div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 2)}" @click="chose_block(2)">
                    <div class="rheader_imgd"><img src="../assets/notification.png"></div>
                    <!--消息通知-->
                    <span>{{$t('notification')}}</span>
                </div>

                <!--<div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 3)}" @click="chose_block(3)">-->
                    <!--<div class="rheader_imgd"><img src="../assets/ignoredisk.png"></div>-->
                    <!--忽略磁盘-->
                    <!--<span>{{$t('ignoredisk')}}</span>-->
                <!--</div>-->

                <div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 4)}" @click="chose_block(4)">
                    <div class="rheader_imgd"><img src="../assets/privacy.png"></div>
                    <!--隐私-->
                    <span>{{$t('privacy')}}</span>
                </div>


                <div class="rheader_1_t" v-bind:class="{'tab_active':(select_block == 5)}" @click="chose_block(5)">
                    <div class="rheader_imgd"><img src="../assets/update.png"></div>
                    <!--更新-->
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
                                <span>AlNtfs for Mac {{$t('menu')}}</span>
                            </div>

                            <div class="main-from_div_1_1-div mb10">
                                <div class="main-from_div_1_1-div_1">
                                    <div style="display: flex;flex-direction: column;justify-content: center">
                                        <i  v-if="show_menu" class="iconfont icondot" style=" font-size: 10px;color: #67c23a"></i>
                                        <i  v-if="!show_menu" class="iconfont icondot" style=" font-size: 10px;color: orangered"></i>
                                    </div>

                                    <div style="display: flex;flex-direction: column;justify-content: center">
                                        <!--已启用 已关闭-->
                                        <span v-if="show_menu" class="main-from_div_1_1-div_1span" style="">{{$t('activated')}}</span>
                                        <span v-if="!show_menu" class="main-from_div_1_1-div_1span" style="color: orangered">{{$t('closed')}}</span>
                                    </div>
                                </div>
                                <div>
                                    <div style="display: flex;flex-direction: column;justify-content: center">
                                        <el-button v-if="show_menu" @click="setToggleTrayMenu">{{$t('disable')}}</el-button>
                                        <el-button v-if="!show_menu" @click="setToggleTrayMenu">{{$t('enable')}}</el-button>
                                    </div>
                                </div>
                            </div>

                            <div class="font12 mb20">
                                <!--警告: 禁用后您将无法直接菜单栏操作卷宗-->
                                <span v-if="show_menu">{{$t('notice_cannot_do_disk01')}}</span>
                                <!--提示: 开启后将在右上角显示快捷操作菜单-->
                                <span v-if="!show_menu">{{$t('notice_cannot_do_disk02')}}</span>
                            </div>

                            <div style="position: absolute;bottom: 0;">
                                <!--跟随系统启动-->
                                <el-checkbox v-model="auto_run"  @change="changeAutoRun()"  :label="$t('Followthesystemstartup')" name="type"></el-checkbox>
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
                                <!--如何处理安装坏卷-->
                                <td><span class="mr20">{{$t('Howtodealwithmountingbadvolumes')}}</span></td>
                                <td>
                                    <select class="al_select al_select_lang" v-model="install_bug_type" @change="changeInstallBugType()">
                                        <!--自动处理-->
                                        <option value="auto_solve">{{$t('Automaticprocessing')}}</option>
                                        <option value="tip_solve">{{$t('Promptbeforeprocessing')}}</option>
                                        <option value="no_solve">{{$t('Donothing')}}</option>
                                    </select>
                                </td>
                            </tr>

                            <tr>
                                <!--如何处理休眠-->
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
                    <!--挂载和推出时显示通知-->
                    <el-checkbox v-model="mount_show_msg"  @change="changeMountShowMsg()"  class="mb20" :label="$t('Shownotificationswhenmountedandlaunched')" name="type"></el-checkbox>

                    <!--有更新时显示通知-->
                    <el-checkbox v-model="update_show_msg"  @change="changeUpdateShowMsg()"  :label="$t('Shownotificationswhenupdatesareavailable')" name="type"></el-checkbox>
                    <span class="sub_form_title">
                        <!--官方存在更新版本时显示通知-->
                        {{$t('Shownotificationswhenanupdatedversionisofficiallyavailable')}}
                    </span>
                    <div class="mb20"></div>
                    <!--磁盘卷宗存在异常时通知-->
                    <el-checkbox v-model="error_disk_msg"  @change="changeErrorDiskMsg()"  :label="$t('Notifywhendiskvolumeisabnormal')" name="type"></el-checkbox>
                    <span class="sub_form_title">
                        <!--磁盘卷宗可能因为异常断开造成数据异常-->
                        {{$t('Diskvolumemaybeabnormalduetoabnormaldisconnection')}}
                    </span>
                </el-form-item>
            </el-form>

            <el-form class="main-from" v-if="select_block == 3">
                <div style="margin: 20px 0">
                    <span>忽略列表里的磁盘将不再出现在磁盘列表</span>
                </div>


                <div>
                    <el-transfer
                            v-model="value"
                            :data="data"
                            :titles="['磁盘', '忽略列表']"
                    ></el-transfer>
                </div>
            </el-form>


            <el-form class="main-from main-from-b4" v-if="select_block == 4">
                <div>
                     <!--收集的所有数据均可在更新后的隐私政策中查看-->
                    <span> AlNtfs {{$t('Alldatacollectedcanbeviewedintheupdatedprivacypolicy')}}</span>
                </div>

                <div class="main-from_div_1_1-div_1" @click="openPrivacyUrl">
                    <div style="display: flex;flex-direction: column;justify-content: center">
                        <i class="iconfont iconjump06" style="font-size: 16px;color: black;">
                            <!--阅读隐私政策            -->
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
                        <!--请谨慎更新到测试版本,因为它们包含实验性的功能.这些功能不稳定,也可能造成数据丢失.-->
                        {{$t('Pleaseupdatetothebetaversion1')}}
                    </span>
                    <div>
                        <el-button @click="checkSoftUpdate">{{$t('Checkforupdates')}}</el-button>
                        <!--重置所有配置-->
                        <el-button @click="resetConf">{{$t('Resetallconfiguration')}}</el-button>
                    </div>


                    <div>
                        <!--上次检查时间-->
                        <span style="font-size: 12px;">{{$t('Lastchecktime')}} : 2020.2.12 14:36</span>
                    </div>
                </el-form-item>
            </el-form>
        </el-main>
    </el-container>
</template>
<script>
    import {ipcRenderer,shell} from 'electron'
    import { alEvent } from '@/utils/alevent.js';
    const Store = require('electron-store');
    const store = new Store();
    const {remote} = require('electron')

    export default {
        components: {},
        data() {
            const generateData = _ => {
                const data = [];
                for (let i = 1; i <= 15; i++) {
                    data.push({
                        key: i,
                        label: `磁盘 ${i}`,
                        disabled: i % 4 === 0
                    });
                }
                return data;
            };

            return {
                auto_run:store.get("auto_run") == false ? false : true,
                theme: store.get("theme") != "undefined" ? store.get("theme") : 1,
                lang: store.get("lang") != "undefined" ? store.get("lang") : "english",
                show_menu:store.get("show_menu") == false ? false : true,
                install_bug_type: store.get("common.install_bug_type") != "undefined" ? store.get("common.install_bug_type") : "auto_solve",
                how_restart_window: store.get("common.how_restart_window") != "undefined" ? store.get("common.how_restart_window") : "change_to_bacground",
                //message
                mount_show_msg: store.get("message.mount_show_msg") != "undefined" ? store.get("message.mount_show_msg") : "",
                update_show_msg: store.get("message.update_show_msg") != "undefined" ? store.get("message.update_show_msg") : "",
                error_disk_msg: store.get("message.error_disk_msg") != "undefined" ? store.get("message.error_disk_msg") : "",

                //disk_list
                history_list: store.get("disk_list.history_list") != "undefined" ? store.get("disk_list.history_list") : [],
                ignore_list: store.get("disk_list.ignore_list") != "undefined" ? store.get("disk_list.ignore_list") : [],

                //update
                auto_check: store.get("update.auto_check") != "undefined" ? store.get("update.auto_check") : [],
                auto_beta_update: store.get("update.auto_beta_update") != "undefined" ? store.get("update.auto_beta_update") : [],

                update_url: store.get("update.update_url") != "undefined" ? store.get("update.update_url") : "",
                privacy_url: store.get("privacy_url") != "undefined" ? store.get("privacy_url") : "",
                update_beta_url: store.get("update.update_beta_url") != "undefined" ? store.get("update.update_beta_url") : "",

                lang_list:this.$t('languages'),//多语言列表


                remote_size: [],

                data: generateData(),
                value: [1, 4],


                select_block: 1,
                form: {
                    name: '',
                    region: '',
                    date1: '',
                    date2: '',
                    delivery: false,
                    type: [],
                    resource: '',
                    desc: ''
                }
            }
        },
        mounted() {
            console.warn(this.lang_list,"lang_list")
            // this.remote_size = remote.getCurrentWindow().getSize();
            // console.warn(this.remote_size,"remote_size");
            //
            // remote.getCurrentWindow().on('resize', () => {
            //     this.remote_size = remote.getCurrentWindow().getSize();
            //     console.warn(this.remote_size,"remote_size");
            //
            // })
        },
        methods: {
            chose_block(select_block_id) {
                switch (select_block_id) {
                    case 1:
                        this.remote_size = [750, 500];
                        break;
                    case 2:
                        this.remote_size = [675, 285];
                        break;
                    case 3:
                        this.remote_size = [625, 495];
                        break;
                    case 4:
                        this.remote_size = [650, 185];
                        break;
                    case 5:
                        this.remote_size = [530, 330];
                        break;
                }

                remote.getCurrentWindow().setSize(this.remote_size[0], this.remote_size[1])
                this.select_block = select_block_id;
            },
            onSubmit() {
                console.log('submit!');
            },
            setToggleTrayMenu(){
                this.show_menu = ipcRenderer.sendSync('toggleTrayMenu') == "destroy" ? false : true;
                store.set("show_menu",this.show_menu);
            },
            changeTheme() {
                console.warn("set theme",this.theme);
                store.set("theme",this.theme);
                this.$refs.carouselObj.setActiveItem(this.theme);//轮播图切换
            },
            changeLang() {
                store.set("lang",this.lang);
                this.$i18n.locale = this.lang;
                ipcRenderer.send('ChangeLangEvent',this.lang);

            },
            changeInstallBugType() {
                console.warn(this.install_bug_type,"install_bug_type");
                store.set('common.install_bug_type',this.install_bug_type);
            },
            changeHowRestartWindow() {
                store.set("common.how_restart_window",this.how_restart_window);
            },
            changeAutoRun(){
                store.set("auto_run",this.auto_run);
            },


            changeMountShowMsg() {
                store.set("message.mount_show_msg",this.mount_show_msg);
            },
            changeUpdateShowMsg() {
                store.set("message.update_show_msg",this.update_show_msg);
            },
            changeErrorDiskMsg() {
                store.set("message.error_disk_msg",this.error_disk_msg);
            },

            changeAutoCheck() {
                store.set("update.auto_check",this.auto_check);
            },
            changeAutoBetaUpdate() {
                store.set("update.auto_beta_update",this.auto_beta_update);
            },
            openPrivacyUrl(){
                shell.openExternal(this.privacy_url);
            },
            checkSoftUpdate(){
                alert("检查更新");
            },
            resetConf(){
                var confirm_status = confirm("确认重置所有配置为默认?")
                if (confirm_status) {
                    if(ipcRenderer.sendSync('MainMsgFromRender',"resetConf") == "succ" ? true : false){
                        alert("重置成功,请重新开启配置页面");
                        remote.getCurrentWindow();
                        remote.getCurrentWindow().close();
                    }else{
                        alert("重置失败");
                    }
                }
            }
        }
    }
</script>

<style scoped>
    .el-carousel__item h3 {
        color: #475669;
        font-size: 14px;
        opacity: 0.75;
        line-height: 150px;
        margin: 0;
    }

    .el-carousel__item:nth-child(2n) {
        background-color: #99a9bf;
    }

    .el-carousel__item:nth-child(2n+1) {
        background-color: #d3dce6;
    }


    /*.rheader_1 div{*/
    /*padding: 3px 5px 5px 5px;*/
    /*margin: 0;*/
    /*border-top-left-radius: 5px;*/
    /*border-top-right-radius: 5px;*/
    /*!*background-color: red;*!*/
    /*}*/
    .block1 {

        display: flex;
        justify-content: center;
        flex-direction: column;
        margin: 20px 30px;
    }

    .el-checkbox__inner:hover {
        border-color: #d8dce5 !important;
    }


    .el-checkbox + .el-checkbox {
        margin-left: 0px !important;
    }

    .main-from {
        display: inline-flex;
        flex-direction: column;
        margin: 10px 30px;
        text-align: left;

    }


    .el-checkbox {
        color: #0a0a0a;
        font-size: 12px;
        cursor: pointer;
        user-select: none;
        line-height: 26px;
    }

    .tab_active {
        /*padding-top: 3px;*/
        /*margin: 0 10px;*/
        background-color: #e8e8e8;
        /*border-top-left-radius: 5px;*/
        /*border-top-right-radius: 5px;*/
    }


    .al-main {
        height: 100%;
        background-color: #ececec;
        text-align: center;
    }


    .al-lblock {
        height: 100%;
        background-color: #232640;
        display: flex;
        justify-content: space-around;
        flex-direction: column;
    }

    .al-rblock {
        display: flex;
        justify-content: space-around;
        flex-direction: column;
        background-color: #f0f0f0
    }

    .lheader {
        -webkit-app-region: drag;
        height: 80px;
        background-color: #262e53;
        display: flex;
        align-items: flex-end
    }

    .lheader div {
        height: 30px;
        width: 100%;
    }

    .lmain {
        padding: 0;
        overflow: hidden;
    }

    .lm-t {
        padding: 5px;
    }

    .lm-t span {
        color: #b4bccc;
    }

    .lm-block {
        width: 100%;
        display: flex;
        height: 60px;
        cursor: pointer;
        background: #323948;
        margin: 5px 0;
    }

    .lm-block-active {
        background-color: #3a8ee6;
    }

    .lm-block_1 {
        display: flex;
        justify-content: center;
        flex-direction: column;
        width: 10%;
        text-align: right;
        /*background: green;*/
    }

    .lm-block_1 div {
        display: flex;
        justify-content: flex-end;
    }

    .lm-block_1 div span {
        width: 10px;
        background-color: #13e735;
        border-radius: 10px;
        height: 10px;
    }

    .lm-block_2 {
        display: flex;
        justify-content: center;
        flex-direction: column;
        text-align: center;
        width: 20%;
        /*background: yellow;*/
    }

    .lm-block_2 img {
        height: 60%;
    }

    .lm-block_3 {
        display: flex;
        justify-content: center;
        flex-direction: column;
        width: 70%;
        color: white;
        /*background: pink;*/
    }

    .lfooter {
        display: flex;
        flex-direction: column;
        justify-content: center;
        /*background-color: #7a7f8a;*/
    }

    .lfooter div {
        display: flex;
        justify-content: center;
        flex-direction: column;
        text-align: center;
        width: 100%;
        color: #e6ebf5;
        /*background: yellow;*/
    }


    .rheader {
        -webkit-app-region: drag;
        height: 80px;
        display: flex;
        justify-content: flex-end;
        flex-direction: column;
        background-color: white;
        padding: 0;
    }

    .rheader_1 {
        text-align: center;
        display: flex;
        justify-content: flex-start;
        /*background: red;*/
        margin-left: 10px;
    }

    .rheader_imgd {
        /*background: red;*/
        justify-content: center;
        flex-direction: row;
    }

    .rheader_1_t {
        display: flex;
        justify-content: center;
        flex-direction: column;
        text-align: center;
        /*width: 100%;*/
        cursor: pointer;
        padding: 2px 6px 2px 6px;
        /*background: green;*/
        margin-right: 10px;
        border-top-left-radius: 3px;
        border-top-right-radius: 3px;
    }


    .rheader_1 img {
        width: 33px;
        height: 33px;
    }

    .rheader_1 span {
        font-size: 12px;
        color: rgba(0, 0, 0, 1);
        margin-top: -5px;
    }

    .rmain {
        display: flex;
        height: 150px;
        /*background-color: red;*/
    }

    .rmain_l {
        width: 30%;
        display: flex;
        flex-direction: column;
        justify-content: center;
        /*background-color: #3a8ee6*/
    }

    .rmain_l img {
        height: 80%;
        width: 80%;
    }

    .rmain_r {
        width: 70%;
        display: flex;
        flex-direction: column;
        justify-content: center;
        /*background-color: #7e57c2*/
    }

    .rmain_r_1 {
        font-size: 20px;
    }

    .rmain_r_1 i {
        color: #a8a8a8
    }

    .rmain_r_2 {

        border-radius: 3px;
        padding: 10px;
        background-color: #e2e2e2;

    }

    .rmain_r_2 div {
        display: flex;
        justify-content: space-between;
    }

    .rmain_r_2 span {
        font-size: 12px;
    }

    .rfooter {
        display: flex;
        flex-direction: column;
        justify-content: center;
        /*background-color: #7a7f8a;*/
    }

    .rfooter_1 {
        display: flex;
        justify-content: center;
        flex-direction: column;
        text-align: right;
        width: 100%;
        color: #e6ebf5;
        /*background: yellow;*/
    }

    .rfooter_1 img {
        height: 25px;
        cursor: pointer
    }


    .rmain2 {
        /*background-color: #e8e8e8;*/
    }

    .rmain2_1 {
        display: flex;
        justify-content: space-between;
    }

    .rmain2_1 span {
        color: #9f9f9f;
        font-size: 12px;
    }

    .rmain2_3_1 {
        display: flex;
        justify-content: space-between;
    }

    .rmain2_3_1_r table, tr, td {
        /*border: 1px solid pink;*/
        font-size: 12px;
        font-size: 12px;
        line-height: 12px;
    }

    .rmain2_3_1_rdtext {
        color: #6f7180;
    }


    .rmain2_3_1_dot {
        display: flex;
        flex-direction: column;
        justify-content: center;
        margin-right: 5px;
    }

    .rmain2_3_1_dot span {
        width: 10px;
        height: 10px;
        display: flex;
        border-radius: 10px;
        background: red;
    }


    .rmain2_2 {
        /*background-color: #6f7180;*/
        height: 12px;
        margin: 5px 0;
        border-radius: 3px;
        overflow: hidden;
    }

    .rmain2_2_bar {
        width: 100%;
        background-color: #cbd0dc;
        height: 100%;
        display: flex;
    }

    .rmain2_2_bar span {
        height: 100%;
    }

    .sub_form_title {
        padding-left: 20px;
        font-size: 12px;
        /* background: red; */
        margin: 0;
        line-height: 16px;
        color: #383838;
    }

    .al_select {

    }

    .al_select:focus {
        outline: none;
    }

    .block1_table tr td:first-child {
        text-align: right;
    }

    .block1_table tr {
        height: 30px;
    }

    .rheaderd {
        background-color: #f6f6f6;
        height: auto !important;
        border-bottom: 1px solid #dedbdb;
        cursor: pointer;
    }

    .rheaderd_div {
        text-align: center;
        margin-bottom: 5px;
        /*background-color: red*/
    }

    .main-from_b1 {
        background: white;
        width: 100%;
        margin: 0;
        padding-bottom: 20px;
    }

    .main-from_div {
        width: 90%;
        margin: 0 auto;
        padding-top: 20px;
    }

    .main-from_div_1 {
        width: 100%;
        display: flex
    }

    .main-from_div_1_1 {
        width: 50%;
        position: relative;
        /*background: green*/
    }

    .main-from_div_1_1-div {
        display: flex;
        justify-content: space-between;
        width: 70%;
    }

    .main-from_div_1_1-div_1 {
        display: flex
    }

    .main-from_div_1_1-div_1span {
        font-size: 15px;
        margin-left: 10px;
    }

    .main-from_btn {
        background-color: transparent;
        border: none;
        font-size: 11px;
        color: #4c4b4b;
        font-weight: 100;
        font-family: cursive;
        padding-left: 0;
    }

    .main-from_div_1_2 {
        width: 50%;
        /*background: red*/
    }

    .main-from_div_1_2_1 {

        display: flex;
        justify-content: flex-end;
        /*background: pink*/
    }

    .main-from_div_1_2_1_div {
        width: 80%
    }

    .main-from_div_1_2_1_div_item {
        display: flex;
        justify-content: center;
        background: transparent !important;
    }

    .main-from_div_1_2_2 {
        display: flex;
        margin-top: 20px;
        justify-content: flex-end;
    }

    .main-from_div2 {

        display: flex;
        margin-top: 20px;
        justify-content: center;
        padding-bottom: 20px;

        background: #efefef
    }

    .main-from_div2_1 {
        margin-top: 20px;
    }

    .al_select_lang {
        width: 150px;
    }

    .main-from-b4 {
        width: 100%;
        margin-top: 30px;
    }

    .main-from-b4_btn {
        background-color: transparent;
        border: none;
        font-size: 11px;
        color: #4c4b4b;
        font-weight: 100;
        font-family: cursive;
        padding-left: 0;
    }
</style>