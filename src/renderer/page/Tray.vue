<template>
    <el-container class="al-main">
            <!--小三角-->
            <div   ref="trayref_h" class="topsj">
                <div>
                    <i class="iconfont iconsjx02 ico_color"></i>
                </div>
            </div>

            <div   ref="trayref" class="trayref">
                <!--头部-->
                <div class="trayref_h">
                    <div class="trayref_h_1">
                        <div class="trayref_h_1_1">
                            <div class="trayref_h_1_1sub_1">
                                <i @click="pushAll" class="iconfont iconpush ico_color"></i>
                            </div>
                        </div>

                        <div class="trayref_h_1_2">
                            <div class="trayref_h_1_2_1">
                                <span class="ico_color">AlNtfs</span>
                            </div>
                        </div>


                        <div class="trayref_h_1_3">
                            <div class="trayref_h_1_3_1">
                                <i class="iconfont icondesktop mr10 ico_color"></i>
                                <i class="iconfont iconmenu05  ico_color" style="transform: scaleX(-1);"></i>
                            </div>
                        </div>

                    </div>
                </div>

                <!--disk block-->
                <div class="diskb">
                    <div class="diskb_1">
                        <div>
                            <i class="iconfont iconpush ico_color mr10"></i>
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
                                <i class="iconfont icondot ico_color"></i>
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
                            <i class="iconfont icontuichu iconpush mr10"></i>
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
                                <i class="iconfont icondot ico_color"></i>
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
                            <i class="iconfont iconpush ico_color mr10"></i>
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
                                <i class="iconfont icondot ico_color"></i>
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
                            <i class="iconfont iconpush ico_color mr10"></i>
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
                                <i class="iconfont icondot ico_color"></i>
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
                            <i class="iconfont iconpush ico_color mr10"></i>
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
                                <i class="iconfont icondot ico_color"></i>
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

    const path = require('path');
    const {shell} = require('electron')

    const {remote} = require('electron')

    export default {
        components: {},
        data() {
            return {}
        },
        mounted() {
            console.warn(this.$refs, "this.$refsa")
            this.resetSize();
        },
        methods: {
            resetSize: function () {
                console.warn(this.$refs, "this.$refs")
                let height0 = this.$refs.trayref_h.offsetHeight;  //100
                let height = this.$refs.trayref.offsetHeight;  //100
                console.warn(height, "traywin height");

                remote.getCurrentWindow().setSize(380, height0 + height + 30)
            },
            pushAll: () => {
                alert("ok");

                let option = {
                    title: "title",
                    body: "body",
                    icon: "../static/hhw.ico",
                    href: 'https://www.iqiyi.com/v_19rqz6uit0.html'
                };

                // 创建通知并保存
                let hhwNotication = new window.Notification(option.title, option);

                // 当通知被点击时, 用默认浏览器打开链接
                hhwNotication.onclick = function () {
                    shell.openExternal(option.href)
                }
            }
        }
    }
</script>

<style>
    html, body, #app {
        background: transparent;
        /*background: #f1f1f1 !important;*/
        /*background: rgba(245, 247, 250, 0.63) !important;*/
    }

    .al-main {
        display: flex;
        flex-direction: column;
        margin-top: -10px;
        background: transparent;
    }

    .topsj {
        height: 20px;
        text-align: center;
    }

    .topsj div {
        margin-top: -5px;
    }

    .topsj div i {
        font-size: 30px;
        height: 30px;
        color: #ebebeb;
    }

    .ico_color {
        color: #797979;
    }

    .trayref {
        background: rgb(243, 243, 243);
        border-radius: 8px;
        padding-bottom: 15px;
        overflow: hidden;
    }

    .trayref_h {
        background: #ebebeb;
        width: 100%;
        border-bottom: 1px solid #d8d5d5;
    }

    .trayref_h_1 {
        height: 40px;
        display: flex;
        justify-content: space-between;
        margin: 0 20px;
        /*background: red;*/
    }

    .trayref_h_1_1 {
        display: flex;
        flex-direction: column;
        justify-content: center;
    }

    .trayref_h_1_1sub_1 {
        display: flex;
        justify-content: center;
    }

    .trayref_h_1_2 {
        display: flex;
        flex-direction: column;
        justify-content: center;
    }

    .trayref_h_1_2_1 {
        display: flex;
        justify-content: center;
    }

    .trayref_h_1_2_1 span {
        font-family: serif;
    }

    .trayref_h_1_3 {
        display: flex;
        flex-direction: column;
        justify-content: center;
    }

    .trayref_h_1_3_1 {
        display: flex;
        justify-content: center;
    }


    .diskb {
        width: 100%;
        display: flex;
        padding: 0 20px;
        margin-top: 20px;
        /*background: #f1f1f1;*/
        /*background: red;*/
    }

    .diskb_1 {
        display: flex;
        justify-content: center;
        flex-direction: column;
        /*background: pink;*/
    }

    .diskb_2 {
        display: flex;
        justify-content: center;
        flex-direction: column;
        /*background: grey;*/
    }

    .diskb_2 img {
        width: 44px;
        margin-right: 10px;
    }

    .diskb_3 {
        width: 100%;
        display: flex;
        flex-direction: column;
        justify-content: space-between;
        /*background: white;*/
    }

    .diskb_3_1 {
        display: flex;
        justify-content: space-between;
        color: #4c4d4f;
        font-family: serif;
        /*background: green*/
    }

    .diskb_3_1 i {
        color: #67c23a;
        font-size: 10px;
    }

    .diskb_3_1 span {
        color: #444446;
    }

    .diskb_3_2 {
        width: 100%;

        border-radius: 10px;
        overflow: hidden;
        background: #c7c4c5;
    }

    .diskb_3_2_1 {
        height: 4px;
        border-radius: 4px;
        width: 80%;
        background: #409eff;
    }

    .diskb_3_3 {
        display: flex;
        justify-content: space-between;
        color: #4c4d4f;
        font-family: serif;
        /*background: green*/
    }
</style>