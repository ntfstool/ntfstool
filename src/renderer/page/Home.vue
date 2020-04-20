<template>
    <el-container class="al-main">
        <el-col class="al-lblock" style="width: 35%;">
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
                        <span v-if="index == 'inner'">{{$t('Systemvolume')}}</span>
                        <span v-if="index == 'ext'">{{$t('Externalvolume')}}</span>
                        <span v-if="index == 'image'">{{$t('ImageVolume')}}</span>
                    </div>

                    <div v-bind:class="{'lm-block-active':(select_disk_key == item.index)}"
                         class="lm-block"
                         v-for="item in list"
                         @click="choseDisk(item)"
                         v-on:dblclick="(select_disk_key == item.index) && openDisk(item)"
                    >
                        <div class="lm-block_1">
                            <div v-if="typeof item.info != 'undefined' && typeof item.info.mounted != 'undefined' && item.info.mounted == true"
                                 @click="uMountDisk(item)" @click.stop>
                                <i class="iconfont iconpush" style="color: rgb(110, 110, 112);">&#xe769;</i>
                            </div>

                            <div v-else @click="mountDisk(item)" @click.stop>
                                <i class="iconfont iconrepush1" style="color: #6e6e70;font-size: 18px;">&#xe609;</i>
                            </div>
                        </div>

                        <div class="lm-block_2">
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


                                        <span v-if="typeof item.status != 'undefined' && (item.status == 0)" class="block_3n_d_1_sp">
                                            {{$t('Mounting')}}...
                                        </span>

                                        <span v-else class="block_3n_d_1_sp">{{item.name.length > 16 ? (item.name.substring(0,16) + "...") : item.name}}</span>


                                    </div>

                                    <div class="lm-block_3n_d_1_fdf">
                                        {{typeof item.info != 'undefined' && typeof item.info.total_size != "undefined"
                                        ? item.info.total_size : "" }}
                                        {{typeof item.info != 'undefined' && typeof item.info.total_size_wei !=
                                        "undefined" ? item.info.total_size_wei : "" }}
                                    </div>
                                </div>


                                <div v-if="typeof item.info != 'undefined' && typeof item.info.mounted != 'undefined' && item.info.mounted == true"
                                >
                                    <div v-if="typeof item.info != 'undefined' && typeof item.info.readonly !='undefined' && item.info.readonly">
                                        <span class="readonly">
                                            {{$t('Readonly')}}
                                        </span>
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
                                </div>

                                <div v-else>
                                    <span class="readonly">{{$t('Unmounted')}}</span>
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
                            <i class="iconfont iconiconsxz i-loading">&#xe623;</i>
                        </div>

                        <i v-else class="iconfont iconshuaxin i-loading2">&#xe650;</i>
                    </div>

                    <span class="version_span">
                        NTFSTool V{{version}}
                    </span>
                </div>
            </el-footer>
        </el-col>


        <el-col class="al-rblock" style="width: 65%;">
            <el-header class="rheader" style="position: relative;">
                <div class="rheader_1">
                    <span> {{select_item.name}}</span>
                </div>
            </el-header>


            <el-main class="ar-main">
                <div>
                    <div class="rmain">
                        <div class="rmain_l">
                            <div  class="rmain_l_box" v-if="select_item.group == 'inner'">
                                <img src="../assets/disk04.png">
                            </div>


                            <div class="rmain_l_box"  v-else-if="select_item.group == 'ext'">
                                <img v-if="typeof select_item.info.typebundle != 'undefined' &&  select_item.info.typebundle == 'ntfs'"
                                     src="../assets/disk01.png">

                                <img v-else src="../assets/disk02.png">
                            </div>

                            <div class="rmain_l_box"  v-else="select_item.group == 'image'">
                                <img src="../assets/disk03.png">
                            </div>
                        </div>

                        <div class="rmain_r">
                            <div class="rmain_r_2">
                                <div style="background: #f9f8f8;">
                                    <span>{{$t('installed')}}</span>

                                    <span v-if="typeof select_item.info.mounted != 'undefined' && select_item.info.mounted">
                                    <i class="el-icon-check" style="color: #67c23a;"></i> Yes
                                </span>

                                    <span v-else>
                                    <i class="el-icon-error" style="color: red;"></i> No
                                </span>
                                </div>
                                <div style="background: rgb(245, 245, 245);">
                                    <span>{{$t('Pathnode')}}</span>
                                    <span>
                                        /dev/{{select_item.index}}</span>
                                </div>
                                <div style="background: #f9f8f8;"><span>{{$t('Mountnode')}}</span><span> {{typeof select_item.info.mountpoint != 'undefined' && select_item.info.mountpoint? select_item.info.mountpoint :'~'}}</span>
                                </div>
                                <div style="background: rgb(245, 245, 245);"><span>UUID</span>
                                    <span>{{typeof select_item.info.uuid != 'undefined' && select_item.info.uuid? select_item.info.uuid :'~'}}</span>
                                </div>
                                <div style="background: #f9f8f8;"><span>{{$t('Partitiontype')}}</span><span> {{select_item.info.typebundle}}</span>
                                </div>
                                <div style="background: rgb(245, 245, 245);"><span>{{$t('Mounttype')}}</span><span> {{typeof select_item.info.protocol != 'undefined' && select_item.info.protocol? select_item.info.protocol :'~'}}
                            </span>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="rmain2">
                        <div class="rmain2_1" v-if="typeof select_item.info.percentage !='undefined'">
                            <!--Disk usage is approaching the limit, please organize data immediately to avoid loss-->
                            <span v-if="select_item.info.percentage >= 90">{{$t('Diskuse')}}: {{select_item.percentage}}% ,{{$t('Nearingthelimit1')}}</span>
                            <!--Can continue to be used safely-->
                            <span v-if="select_item.info.percentage >= 50 && select_item.info.percentage < 90">{{$t('Diskuse')}}: {{select_item.info.percentage}}%,{{$t('Cancontinuetobeusedsafely')}}</span>
                            <!--Less for safe storage-->
                            <span v-if="select_item.info.percentage < 50">{{$t('Diskuse')}}: {{select_item.info.percentage}}% {{$t('Lessforsafestorage')}}</span>
                        </div>

                        <div class="rmain2_2">
                            <div class="rmain2_2_bar" v-if="typeof select_item.info.percentage !='undefined'">
                                <span v-if="select_item.info.percentage >= 90"
                                      v-bind:style="{ width: select_item.info.percentage + '%', backgroundColor: '#fc615d' }"></span>

                                <span v-if="select_item.info.percentage >= 50 && select_item.info.percentage < 90"
                                      v-bind:style="{ width: select_item.info.percentage + '%', backgroundColor: '#fdbc40' }"></span>

                                <span v-if="select_item.info.percentage < 50"
                                      v-bind:style="{ width: (select_item.info.percentage ? select_item.info.percentage : '0') + '%', backgroundColor: '#34c84a' }">
                                </span>
                            </div>
                        </div>

                        <div class="rmain2_3">
                            <div class="rmain2_3_1">
                                <table class="rmain2_3_1_r">
                                    <tr>
                                        <td>
                                            <i class="iconfont icondot mr5" style=" font-size: 10px;color: #5586db">&#xe607;</i>
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
                                            <i class="iconfont icondot mr5" style=" font-size: 10px;color: #d12890">&#xe607;</i>
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
                                            <i class="iconfont mr5" style=" font-size: 10px;color: #f1e600">&#xe607;</i>
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

                <div class="mright-dir"
                     v-if="typeof select_item.info.mounted != 'undefined' && select_item.info.mounted">
                    <div class="goods">
                        <img @click="openDisk(select_item)" src="../assets/opendisk.svg" :title="$t('Clicktoopenthedisk')">
                    </div>
                </div>
            </el-main>

            <el-footer class="rfooter">
                <div class="rfooter_div" @click="altest"></div>

                <div class="rfooter_1">
                    <div>
                        <img  @click="openMenuBox('menu_box1')" src="../assets/setting2.svg">
                    </div>

                    <div class="menu_box" v-show="menu_box1">
                        <div @click="exitAll">{{$t('Quit')}}</div>
                        <span class="line"></span>
                        <div @click="openFeedBackPage">{{$t('Submitfeedback')}}</div>
                        <div @click="openSettingPage"> {{$t('preferences')}}</div>
                        <span class="line"></span>
                        <!--<div @click="clearPwd">{{$t('Clearpassword')}}</div>-->
                        <div @click="openAboutPage"> {{$t('About')}}</div>
                    </div>
                </div>
            </el-footer>
        </el-col>
    </el-container>
</template>
<script>
    import home from '@/renderer/lib/home.js'
    export default home
</script>
<style scoped src="@/renderer/theme/home.css"></style>
<style scoped></style>