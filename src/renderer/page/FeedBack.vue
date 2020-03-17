<template>
    <el-container class="main-box">
        <header class="header">
            <div>NTFS Tool 反馈</div>
        </header>

        <div class="contain">
            <div class="box">
                <div class="left">
                    <img @click="openMainsite()" src="../assets/logo.png">
                    <div class="sn">
                        <div class="spans">
                            <span>系列号</span>
                            <span>{{serial_number}}</span>
                        </div>
                    </div>
                </div>
            </div>

            <div class="fd_form">
                <div style="font-size: 25px;">反馈表单</div>

                <div style="font-size: 14px;">请提供关于您问题的详细描述、建议、漏洞报告或者您的疑问,以便我们对您的诉求有了清晰的认识之后才能给您最有效的答复。</div>
                <div>
                    <select class="al_select al_select_lang"
                            v-model="fb_back_type">
                        <!--自动处理-->
                        <option value="feedback">反馈</option>
                        <option value="help">支持请求</option>
                        <option value="report">错误报告</option>
                    </select>
                </div>
                <div style="    display: flex;">
                    <el-input v-model="fb_name" placeholder="john appleseed"></el-input>
                    <el-input style="margin-left: 20px;" v-model="fb_email"
                              placeholder="someone@somewhere.com"></el-input>
                </div>

                <div>
                    <el-input
                            type="textarea"
                            :rows="8"
                            resize="none"
                            placeholder="Please fill in your content"
                            v-model="fb_content">
                    </el-input>
                </div>

                <div style="font-size: 12px;">
                    <el-checkbox v-model="agreement" name="type"></el-checkbox>
                    提交运行数据、日志等信息，从而帮助了解您的设备信息并改善我们的应用
                </div>

                <div style="text-align: right;margin-top: 10px;">
                    <button style="margin-right: 0;" :class="[btnDisable ? 'btn-active-disable' :  'btn-active']"
                            @click="sumbit_feedback">发送反馈
                    </button>
                </div>
            </div>
        </div>
    </el-container>
</template>
<script>
    const {shell, ipcRenderer, remote} = require('electron')
    const axios = require('axios')
    const fs = require("fs")
    const saveLog = require('electron-log');
    import {getPackageVersion, disableZoom, getSystemInfo, noticeTheSystemError, POST_LOG_URL} from '@/utils/utils'

    export default {
        components: {},
        data() {
            return {
                version: "-",
                serial_number: "--",
                os_version: "",
                fb_back_type: "feedback",
                fb_name: "",
                fb_email: "",
                fb_content: "",
                agreement: false,
                btnDisable: false
            }
        },
        mounted() {
            disableZoom(require('electron').webFrame);
            this.version = getPackageVersion();

            getSystemInfo().then(json => {
                var pasysteminfo_parmrams = Object.keys(json).map(function (key) {
                    return encodeURIComponent(key) + "=" + encodeURIComponent(json[key]);
                }).join("&");
                this.serial_number = json.serial_number;
                this.os_version = json.os_version;
            });
        },
        methods: {
            openMainsite() {
                shell.openExternal("https://www.ntfstool.com")
            },
            reDefault() {
                this.fb_name = "";
                this.fb_email = "";
                this.fb_content = "";
            },
            sumbit_feedback() {
                if (this.btnDisable) {
                    return;
                }
                var _this = this;
                this.btnDisable = true;
                setTimeout(function () {
                    _this.btnDisable = false;
                }, 10000);

                if (!this.agreement) {
                    var confirm_status = confirm("建议您勾选提交运行数据、日志等信息...")
                    if (confirm_status) {
                        this.agreement = true;
                    } else {
                        confirm_status = confirm("您已放弃提交运行数据、日志等信息,我们可能无法复原您的问题,确定继续提交?")
                        if (!confirm_status) {
                            return;
                        }
                    }
                }


                //get the log data
                var logObj = saveLog.transports.file.getFile();
                var otherData = "";
                console.warn(logObj, "log getFile");
                if (typeof logObj.path != "undefined") {
                    try {
                        var data = fs.readFileSync(logObj.path);
                        otherData = data.toString();
                    } catch (e) {
                        otherData = e.message;
                        console.warn(e.message,"error")
                    }
                }

                var postData = {
                    "__topic__": "FeedBack",
                    "__source__": "NtfsTool",
                    "__logs__": [{
                        atype: "FeedBack",
                        asn: this.serial_number,
                        os_version: this.os_version,
                        fb_back_type: this.fb_back_type,
                        fb_name: this.fb_name,
                        fb_email: this.fb_email,
                        fb_content: this.fb_content,
                        otherData: Buffer.from(otherData).toString('base64')
                    }]
                };

                console.warn(postData, "postData");

                var postDataStr = JSON.stringify(postData);

                console.warn(postDataStr, "postDataStr");

                axios({
                    method: 'post',
                    url: POST_LOG_URL,
                    headers: {
                        "x-log-apiversion": "0.6.0",
                        "x-log-bodyrawsize": "1234",
                    },
                    data: postDataStr
                }).then(function (response) {
                    _this.btnDisable = false;
                    console.log(response);
                    _this.reDefault();
                    alert("提交信息成功");
                    remote.getCurrentWindow().hide();
                }).catch(function (error) {
                    _this.btnDisable = false;
                    saveLog.error(error.message, "FEEDBACK_ERROR_CONTENT");
                    noticeTheSystemError("FEEDBACK_ERROR");
                    alert("提交信息失败");
                });
            }
        }
    }

</script>

<style lang="scss" scoped>
    .header {
        display: flex;
        width: 100%;
        height: 45px;
        line-height: 25px;
        justify-content: center;
        font-size: 15px;
        -webkit-app-region: drag;
    }

    .main-box {
        display: flex;
        flex-direction: column;
        height: 100vh;
        background: #ececec !important;
    }

    .fd_form {
        width: calc(70% - 20px);
    }

    .fd_form > div {
        padding: 5px 0;
    }

    .lb-box > div {
        text-align: center;
        padding: 2px;
    }

    .contain {
        display: flex;

        .box {
            width: 30%;
            display: flex;
            justify-content: space-between;
            flex-direction: column;

            .left {
                text-align: center;

                img {
                    width: 120px;
                    height: 120px;
                    user-select: none;
                }

                .sn {
                    margin: 20px;
                    text-align: center;
                    flex-direction: column;
                    display: flex;

                    .spans {
                        font-size: 12px;
                        font-weight: bold;
                        line-height: 20px;
                        padding-bottom: 10px;

                        span {
                            user-select: text
                        }
                    }
                }
            }
        }
    }
</style>