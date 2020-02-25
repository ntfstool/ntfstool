<template>
    <el-container class="al-main">
        <el-col class="al-lblock" :span="8">
            <el-header class="lheader">
                <div>
                    <div>
                        <span class="lheader_span">NTFS Tool</span>
                    </div>
                </div>
            </el-header>

            <el-main class="lmain">
                <div v-for="(list, index)  in diskList">
                    <div class="lm-t" v-if="list.length > 0">
                        <!--系统卷-->
                        <span v-if="index == 'inner'">{{$t('Systemvolume')}}</span>
                        <!--外置卷-->
                        <span v-if="index == 'ext'">{{$t('Externalvolume')}}</span>
                        <!--镜像卷-->
                        <span v-if="index == 'image'">{{$t('ImageVolume')}}</span>
                    </div>

                    <div v-bind:class="{'lm-block-active':(select_disk_key == item.index)}"
                         class="lm-block"
                         v-for="item in list"
                         @click="choseDisk(item)"
                         v-on:dblclick="(select_disk_key == item.index) && openDisk(item)"
                    >
                        <div class="lm-block_1">
                            <div v-if="typeof item.info != 'undefined' && typeof item.info.mounted != 'undefined' && item.info.mounted"
                                 @click="uMountDisk(item)" @click.stop>
                                <i class="iconfont iconpush" style="color: rgb(110, 110, 112);"></i>
                            </div>

                            <div v-else @click="mountDisk(item)" @click.stop>
                                <i class="iconfont iconrepush1" style="color: #6e6e70;    font-size: 18px;"></i>
                            </div>
                        </div>

                        <div class="lm-block_2">
                            <!--系统卷-->
                            <div v-if="index == 'inner'">
                                <img src="../assets/disk04.png">
                            </div>

                            <div v-else-if="index == 'ext'">
                                <img v-if="typeof item.info != 'undefined' && typeof item.info.typebundle != 'undefined' &&  item.info.typebundle == 'ntfs'"
                                     src="../assets/disk01.png">

                                <img v-else src="../assets/disk02.png">
                            </div>

                            <div v-else="index == 'image'">
                                <img src="../assets/disk03.png">
                            </div>
                        </div>


                        <div class="lm-block_3">
                            <div class="lm-block_3n_d">

                                <div class="lm-block_3n_d_1">
                                    <div class="lm-block_3n_d_1_flex">
                                        <div class="block_3n_d_1_sd">
                                            <span v-if="typeof item.info != 'undefined' && typeof item.info.mounted != 'undefined' && item.info.mounted"
                                                  class="block_3n_d_1_sdp mounted_dot"></span>
                                            <span v-else class="block_3n_d_1_sdp unmounted_dot"></span>
                                        </div>

                                        <span class="block_3n_d_1_sp">{{item.name.length > 16 ? (item.name.substring(0,16) + "...") : item.name}}</span>
                                    </div>

                                    <div class="lm-block_3n_d_1_fdf">
                                        {{typeof item.info != 'undefined' && typeof item.info.total_size != "undefined"
                                        ? item.info.total_size : "" }}
                                        {{typeof item.info != 'undefined' && typeof item.info.total_size_wei !=
                                        "undefined" ? item.info.total_size_wei : "" }}
                                    </div>
                                </div>

                                <div class="lm-block_3n_d_2"
                                     v-if="typeof item.info != 'undefined' && typeof item.info.readonly !='undefined' && !item.info.readonly && typeof item.info.percentage !='undefined'">
                                    <span v-if="item.info.percentage >= 90"
                                          v-bind:style="{ width: item.info.percentage + '%', backgroundColor: 'rgb(131, 192, 253)' }"></span>

                                    <span v-else-if="item.info.percentage < 90"
                                          v-bind:style="{ width: item.info.percentage + '%', backgroundColor: 'rgb(131, 192, 253)' }"></span>

                                    <span v-else="item.info.percentage < 50"
                                          v-bind:style="{ width: item.info.percentage + '%', backgroundColor: 'rgb(131, 192, 253)' }"></span>
                                </div>

                                <div v-if="typeof item.info != 'undefined' && typeof item.info.readonly !='undefined' && item.info.readonly">
                                    <span class="readonly">Readonly</span>

                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </el-main>


            <el-footer class="lfooter">
                <div class="lfooter_div" @click="refreshDevice">
                    <div style="display: flex;">
                        <div v-if="loading == -1" class="mint-spinner-snake">
                            <i class="iconfont iconiconsxz i-loading"></i>
                        </div>

                        <i v-else class="iconfont iconshuaxin i-loading2"></i>
                    </div>

                    <span class="version_span">
                        NTFSTool V3.0.2
                    </span>
                </div>
            </el-footer>
        </el-col>


        <el-col class="al-rblock" :span="16">
            <el-header class="rheader">
                <div class="rheader_1">
                    <span> {{select_item.name}}</span>
                </div>
            </el-header>


            <el-main class="ar-main">
                <div>
                    <div class="rmain">
                        <div class="rmain_l">
                            <!--系统卷-->
                            <div v-if="select_item.group == 'inner'">
                                <img src="../assets/disk04.png">
                            </div>


                            <div v-else-if="select_item.group == 'ext'">
                                <img v-if="typeof select_item.info.typebundle != 'undefined' &&  select_item.info.typebundle == 'ntfs'"
                                     src="../assets/disk01.png">

                                <img v-else src="../assets/disk02.png">
                            </div>

                            <div v-else="select_item.group == 'image'">
                                <img src="../assets/disk03.png">
                            </div>
                        </div>

                        <div class="rmain_r">
                            <div class="rmain_r_2">
                                <div style="background: #f9f8f8;">
                                    <!--已安装-->
                                    <span>{{$t('installed')}}</span>

                                    <span v-if="typeof select_item.info.mounted != 'undefined' && select_item.info.mounted">
                                    <i class="el-icon-check" style="color: #67c23a;"></i> Yes
                                </span>

                                    <span v-else>
                                    <i class="el-icon-error" style="color: red;"></i> No
                                </span>
                                </div>
                                <!--路径节点-->
                                <div style="background: rgb(245, 245, 245);">
                                    <span>{{$t('Pathnode')}}</span>
                                    <span>
                                        /dev/{{select_item.index}}</span>
                                </div>
                                <!--挂载节点-->
                                <div style="background: #f9f8f8;"><span>{{$t('Mountnode')}}</span><span> {{typeof select_item.info.mountpoint != 'undefined' && select_item.info.mountpoint? select_item.info.mountpoint :'~'}}</span>
                                </div>
                                <div style="background: rgb(245, 245, 245);"><span>UUID</span><span>
                                {{typeof select_item.info.uuid != 'undefined' && select_item.info.uuid? select_item.info.uuid :'~'}}
                            </span></div>
                                <!--分区类型-->
                                <div style="background: #f9f8f8;"><span>{{$t('Partitiontype')}}</span><span> {{select_item.info.typebundle}}</span>
                                </div>
                                <!--内置  协议-->
                                <div style="background: rgb(245, 245, 245);"><span>{{$t('Mounttype')}}</span><span> {{typeof select_item.info.protocol != 'undefined' && select_item.info.protocol? select_item.info.protocol :'~'}}
                            </span>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="rmain2">
                        <div class="rmain2_1" v-if="typeof select_item.info.percentage !='undefined'">
                            <!--磁盘占用   已接近极限,请即时整理数据,以免丢失-->
                            <span v-if="select_item.info.percentage >= 90">{{$t('Diskuse')}}: {{select_item.percentage}}% ,{{$t('Nearingthelimit1')}}</span>
                            <!--可继续安全使用-->
                            <span v-if="select_item.info.percentage >= 50 && select_item.info.percentage < 90">{{$t('Diskuse')}}: {{select_item.info.percentage}}%,{{$t('Cancontinuetobeusedsafely')}}</span>
                            <!--较少,可以安全存储-->
                            <span v-if="select_item.info.percentage < 50">{{$t('Diskuse')}}: {{select_item.info.percentage}}% {{$t('Lessforsafestorage')}}</span>
                        </div>

                        <div class="rmain2_2">
                            <div class="rmain2_2_bar" v-if="typeof select_item.info.percentage !='undefined'">
                                <!--background-color: #5586db;-->
                                <span v-if="select_item.info.percentage >= 90"
                                      v-bind:style="{ width: select_item.info.percentage + '%', backgroundColor: '#fc615d' }"></span>

                                <span v-if="select_item.info.percentage >= 50 && select_item.info.percentage < 90"
                                      v-bind:style="{ width: select_item.info.percentage + '%', backgroundColor: '#fdbc40' }"></span>

                                <span v-if="select_item.info.percentage < 50"
                                      v-bind:style="{ width: select_item.info.percentage + '%', backgroundColor: '#34c84a' }"></span>
                            </div>
                        </div>

                        <div class="rmain2_3">
                            <div class="rmain2_3_1">
                                <table class="rmain2_3_1_r">
                                    <tr>
                                        <td>
                                            <i class="iconfont icondot mr5" style=" font-size: 10px;color: #5586db"></i>
                                        </td>
                                        <td>
                                            {{$t('total')}}
                                        </td>
                                    </tr>

                                    <tr>
                                        <td>

                                        </td>
                                        <td class="rmain2_3_1_rdtext">
                                            {{select_item.info.total_size}} {{select_item.info.total_size_wei}}
                                        </td>
                                    </tr>
                                </table>

                                <table class="rmain2_3_1_r">
                                    <tr>
                                        <td>
                                            <i class="iconfont icondot mr5" style=" font-size: 10px;color: #d12890"></i>
                                        </td>
                                        <td>
                                            {{$t('used')}}
                                        </td>
                                    </tr>

                                    <tr>
                                        <td></td>
                                        <td class="rmain2_3_1_rdtext">
                                            {{select_item.info.used_size}} {{select_item.info.used_size_wei}}
                                        </td>
                                    </tr>
                                </table>


                                <table class="rmain2_3_1_r">
                                    <tr>
                                        <td>
                                            <i class="iconfont icondot mr5" style=" font-size: 10px;color: #f1e600"></i>
                                        </td>
                                        <td>{{$t('available')}}</td>
                                    </tr>
                                    <tr>
                                        <td></td>
                                        <td class="rmain2_3_1_rdtext">
                                            {{(select_item.info.total_size - select_item.info.used_size).toFixed(2)}}
                                            {{select_item.info.total_size_wei}}
                                        </td>
                                    </tr>
                                </table>
                            </div>
                        </div>
                    </div>

                </div>

                <div class="mright-dir" v-if="typeof select_item.info.mounted != 'undefined' && select_item.info.mounted">
                    <div class="goods">
                        <img @click="openDisk(select_item)" src="../assets/opendisk.svg">
                    </div>
                </div>
            </el-main>

            <el-footer class="rfooter">
                <div class="rfooter_div" @click="altest"></div>

                <div class="rfooter_1">
                    <div>
                        <img @click="openSysSetting" src="../assets/setting2.svg">
                    </div>
                </div>
            </el-footer>
        </el-col>
    </el-container>
</template>
<script>
    import {ipcRenderer, remote} from 'electron'
    import {getDiskList, listenSudoPwd, getDiskFullInfo, uMountDisk, mountDisk, openInFinder} from '@/utils/utils'
    import {alEvent} from '@/utils/alevent.js';
    const log = require('electron-log');
    const fs = require('fs');
    export default {
        components: {},
        data() {
            return {
                activeTab: 'tab1',
                devices: [],
                diskList: [],//磁盘列表
                loading: 0,//加载中


                select_item: {
                    disk_mount: "",
                    canPush: false,
                    type: "",
                    name: "",
                    size: "",
                    size_wei: "",
                    index: "",
                    info: [],
                    _test: ""
                },
                select_disk_key: "",
                atest_lasttime: 0,
                atest_times: 0,
                sudoDialog: false,
            }
        },
        mounted() {
            var _this = this;
            var webFrame = require('electron').webFrame;
            webFrame.setVisualZoomLevelLimits(1, 1);
            webFrame.setLayoutZoomLevelLimits(0, 0);
            listenSudoPwd();
            _this.refreshDevice();
            //管理员密码
            alEvent.$on('SudoPWDEvent', args => {
                if (_this.sudoDialog) {
                    return;
                }
                _this.sudoDialog = true;

                var title = '磁盘挂载需要管理员权限,请输入密码';
                if (args == "invalid password") {
                    title = '密码验证错误,请重新输入';
                }
                this.$prompt(title, '', {
                    showClose: false,
                    inputType: "password",
                    confirmButtonText: '确定',
                    cancelButtonText: '取消',
                }).then(({value}) => {
                    _this.sudoDialog = false;
                    alEvent.$emit('setPWDEvent', value);//发送刷新事件
                    _this.refreshDevice();
                }).catch(err => {
                    _this.sudoDialog = false;
                })
            });

            //监听语言切换
            ipcRenderer.on("ChangeLangEvent", (e, lang) => {
                console.warn("main wind ChangeLangEvent", lang);
                this.$i18n.locale = lang;
            });

            //监听挂载事件
            ipcRenderer.on("MountEvent", (e, filename) => {
                console.warn("main wind acive MountEvent", filename);
                _this.refreshDevice();
            });

            try {
                var path = '/Volumes/';
                fs.watch(path, function (event, filename) {
                    console.warn(filename, "home watch Volumes")
                    _this.refreshDevice();
                });
            } catch (e) {
                log.warn(e, "watch Volumes");
            }

            alEvent.$on('doRefreshEvent', filename => {
                //console.warn(`Listened doRefreshEvent+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++`)
                _this.refreshDevice();
            });
        },
        methods: {
            refreshDevice() {
                try {
                    this.loading = -1;
                    //更新列表
                    getDiskList().then((diskList) => {
                        console.log(diskList, "getDiskList");
                        this.diskList = diskList;
                        if (!this.select_disk_key) {
                            try {
                                this.choseDisk(diskList["inner"][0]);
                            } catch (e) {
                                log.warn(e, "refreshDevice choseDisk");
                            }
                        }
                        this.loading = 0;
                    });
                } catch (e) {
                    log.warn(e, "refreshDevice");
                }
            },
            changeVolumeName(select_item) {
                this.$prompt('请输入新名称', '', {
                    showClose: false,
                    inputValue: select_item.volume_name,
                    confirmButtonText: '确定',
                    cancelButtonText: '取消',
                }).then(({value}) => {
                    this.$alert("ok " + value);
                })
            },
            clearDisk(item) {
                console.warn("clearDisk click", this.$i18n.t('Erasingthediskwilldelete'))
                // 抹掉磁盘将删除 的所有资料
                var confirm_status = confirm(this.$i18n.t('Erasingthediskwilldelete') + " (" + item.name + ")")
                console.warn(confirm_status, "confirm confirm_status")
                if (confirm_status) {
                    if (confirm(this.$i18n.t('Detectsystemdisktoolisabouttojump'))) {
                        openSysDiskUtils().then(res => {
                            console.warn("openSysDiskUtils res", res)
                        })
                    }
                }
            },
            uMountDisk(item) {
                var _this = this;
                console.warn(item, "select_item");
                if (item.group == 'inner') {
                    alert(this.$i18n.t('Internaldiskcannotbeunmounted') + ":" + item.name);
                    return;
                }
                var confirm_status = confirm(this.$i18n.t('OKtounmountthedisk') + ":" + item.name)
                console.warn(confirm_status, "confirm confirm_status")
                if (confirm_status) {
                    uMountDisk(item).then(res => {
                        console.warn("uMountDisk res", res);
                        let option = {
                            title: "NTFSTool",
                            body: item.name + " 磁盘推出成功",
                        };
                        new window.Notification(option.title, option);
                        _this.refreshDevice();
                    })
                }
            },
            mountDisk(item) {
                var _this = this;
                mountDisk(item).then(res => {
                    console.warn("mountDisk res", res)
                    let option = {
                        title: "NTFSTool",
                        body: item.name + " 磁盘挂载成功",
                    };
                    new window.Notification(option.title, option);
                    _this.refreshDevice();
                })
            },
            choseDisk(item) {
                if (typeof item.index != "undefined" && typeof item.info != "undefined" && typeof item.info.protocol != "undefined") {
                    this.select_item = item;
                    this.select_disk_key = item.index;
                } else {
                    log.warn(item, "choseDisk Error");
                }
            },
            openDisk(item) {
                console.warn("dbclick ", item);
                if (!item.info.mountpoint) {
                    alert("该磁盘没有挂载");
                    return;
                }
                openInFinder(item.info.mountpoint).catch(() => {
                    alert("openDisk fail!");
                });
            },
            openSysSetting() {
                console.warn(" openSysSetting click");
                ipcRenderer.send('MainMsgFromRender', 'openSysSeeting')
            },
            altest() {
                console.warn("altest");
                var cur_time = new Date().getTime();


                if (cur_time - this.atest_lasttime > 1000) {
                    this.atest_times = 0;

                } else {
                    this.atest_times++;
                    // console.warn(this.atest_times, "this.atest_times");

                    if (this.atest_times > 5) {
                        this.atest_times = 0;
                        remote.getCurrentWindow().webContents.openDevTools();
                        console.warn("open debug!....");
                    }
                }
                this.atest_lasttime = cur_time;
            }
        }
    }
</script>
<style scoped>
    .al-main {
        height: 100%;
        background-color: #8c939d
    }

    .al-lblock {
        height: 100%;
        background-color: #f4f4f4;
        display: flex;
        justify-content: space-around;
        flex-direction: column;
    }

    .al-rblock {
        display: flex;
        justify-content: space-around;
        flex-direction: column;
        background-color: white;
    }

    .lheader {
        -webkit-app-region: drag;
        display: flex;
        justify-content: flex-end;
        flex-direction: column;
        background-color: #f1f1f1;
        border-bottom: 1px solid #d8d5d5;
    }

    .lheader div {
        height: 45px;
        width: 100%;
        text-align: center;
        font-size: 22px;
    }

    .lmain {
        padding: 0;
    }

    .lm-t {
        padding: 5px;
    }

    .lm-t span {
        color: #6e6e70;
    }

    .lm-block {
        width: 100%;
        display: flex;
        height: 60px;
        cursor: pointer;
        /* background: #ffffff; */
        margin: 5px 0;
        /* border-left: 3px solid #3a8ee6; */
        /* border-bottom: 01px solid #9e9e9e; */
        /* box-shadow: 1px 1px 1px 1px #c5c5c5; */
    }

    .lm-block-active {
        background: #ffffff;
    }

    .lm-block_1 {
        margin-right: 5px;
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
        width: 60%;
        color: white;
        /* background: pink; */
        /* border-bottom: 1px solid #e0e0e0; */
    }


    .lfooter {
        display: flex;
        flex-direction: column;
        justify-content: center;
        /*background-color: #7a7f8a;*/
    }

    .lfooter_div {
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
        border-bottom: 1px solid #f1f1f1;
    }

    .rheader_1 {
        text-align: center;
        display: flex;
        justify-content: center;
        /*background: red;*/
    }

    .rheader_1 div {
        display: flex;
        justify-content: center;
        flex-direction: column;
        text-align: center;
        width: 50px;
        cursor: pointer;
        /*background: green;*/
    }

    .rheader_1 img {
        height: 25px;
    }

    .rheader_1 span {
        font-size: 20px;
        color: #000000;
        line-height: 60px;
        font-weight: 100;
        font-family: -webkit-body;
    }

    .rmain {
        display: flex;
        height: 200px;
        /*background-color: red;*/
        -webkit-user-select: text;
        -moz-user-select: text;
        -ms-user-select: text;
        user-select: text;
    }

    .rmain_l {
        width: 30%;
        display: flex;
        flex-direction: column;
        justify-content: center;
        /*background: yellow;*/
        display: flex;
        flex-direction: column;
        justify-content: center;
    }

    .rmain_l img {
        width: 90%;
        /*background: red;*/
        padding-top: 20px;
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
        padding: 10px 0;
        background-color: #f7f7f7;

    }

    .rmain_r_2 div {
        display: flex;
        padding: 0 10px;
        justify-content: space-between;
    }

    .rmain_r_2 span {
        font-size: 14px;
        color: #b1b1b1;
        line-height: 20px;
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
        font-size: 14px;
        line-height: 30px;
    }

    .rmain2_3_1 {
        display: flex;
        justify-content: space-between;
    }

    .rmain2_3_1_r table, tr, td {
        font-size: 15px;
        line-height: 15px;
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
        height: 8px;
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

    .lm-block_3n_d {
        display: flex;
        justify-content: space-between;
        flex-direction: column;
        /*background-color: rosybrown;*/
    }

    .lm-block_3n_d_1 {
        display: flex;
        justify-content: space-between;
    }

    .lm-block_3n_d_1_flex {
        display: flex
    }

    .block_3n_d_1_sd {
        display: flex;
        margin-right: 5px;
        justify-content: center;
        flex-direction: column;
    }

    .block_3n_d_1_sdp {
        width: 10px;
        border-radius: 10px;
        height: 10px;
    }

    .mounted_dot {
        background-color: #83c0fd;
        /*background-color: #13e735;*/
    }

    .unmounted_dot {
        background-color: #b1a7a7;
    }


    .block_3n_d_1_sp {
        font-size: 15px;
        color: #6e6e70
    }

    .lm-block_3n_d_1_fdf {
        font-size: 15px;
        color: #6e6e70
    }

    .lm-block_3n_d_2 {
        background-color: rgb(224, 224, 224);
        width: 100%;
        height: 3px;
        border-radius: 3px;
        overflow: hidden;
        display: flex;
        margin-top: 10px;
    }

    .lm-block_2 img {
        width: 45px;
        height: 45px;
        vertical-align: bottom;
    }


    .mint-spinner-snake {
        animation: circle 1s infinite linear;
        -webkit-animation: circle 1s infinite linear;
    }

    @-webkit-keyframes circle {
        0% {
            transform: rotate(0deg);
        }
        100% {
            transform: rotate(-360deg)
        }
    }


    .box {
        height: 50px;
        width: 50px;
        position: relative;
    }

    .box::before {
        content: '';
        height: 8px;
        width: 50px;
        background: #000;
        opacity: .2;
        border-radius: 50%;
        position: absolute;
        top: 67px;
        left: 0;
        animation: shadow 1s linear infinite;
    }

    .box::after {
        border-radius: 5px;
        background: red;
        animation: rotate 1s linear infinite;
        content: '';
        position: absolute;
        left: 0;
        top: 0;
        width: 50px;
        height: 50px;
    }

    @keyframes shadow {
        0%, 100% {
            transform: scaleX(1);
        }
        50% {
            transform: scaleX(1.2);
        }
    }

    @keyframes rotate {
        0% {
            transform: translateY(0);
        }
        25% {
            transform: translateY(10px);
        }
        50% {
            transform: translateY(20px) scale(1.1, 0.9);

        }
        75% {
            ransform: translateY(10px);
        }
        100% {
            transform: translateY(0);
        }
    }

    .goods {
        text-align: center;
    }

    .goods img {
        animation: myfirst 2s infinite;
    }

    @keyframes myfirst {
        0% {
            transform: translate(0px, 0px);
        }
        50% {
            transform: translate(0px, -10px);
        }
        100% {
            transform: translate(0px, 0px);
        }
    }

    .lheader_span {
        color: black;
        line-height: 30px;
        font-size: 20px;
    }

    .readonly {
        font-size: 12px;
        color: #f56c6c;
    }

    .version_span {
        flex-direction: column;
        justify-content: center;
        color: #585858;
        margin-left: 10px;
        display: flex;
    }

    .i-loading {
        color: #34c84a;
        font-size: 18px;
    }

    .i-loading2 {
        color: #13e735;
        font-size: 18px;
    }

    .lfooter_div {
        flex-direction: row;
        cursor: pointer
    }

    .mright-dir{
        height: 100%;
        display: flex;
        justify-content: center;
        flex-direction: column;
    }
    .goods{
        margin-top: 50px;
    }
    .goods img{
        height: 100px;
    }
    .rfooter_div{

        width: 100px;
        height: 50px;
        position: absolute;
        bottom: 0;
    }

    .ar-main{
        display: flex;
        flex-direction: column;
        justify-content: space-between;
    }
</style>
