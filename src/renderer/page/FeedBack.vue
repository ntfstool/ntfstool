<template>
    <el-container class="main-box">
        <header class="header">
            <div>NTFS Tool 反馈</div>
        </header>
        <div style="
    display: flex;">
            <div style="     width: 30%;
    BACKGROUND: RED;
    display: flex;
    justify-content: space-between;
    flex-direction: column;">
                <div style="    text-align: center;">
                    <img @click="openMainsite()" style="width: 120px;height: 120px;user-select: none;" src="../assets/logo.png">
                </div>

                <div class="lb-box">
                    <!--<div style="font-weight: bold;">-->
                        <!--NTFSTool for MAC-->
                    <!--</div>-->
                    <!--<div style="font-size: 10px;">-->
                        <!--版本 {{version}} (3245) <i   @click="openGitHub()"  class="iconfont" style="margin-left:5px;font-size: 12px;-->
    <!--color: black;">&#xe691;</i>-->
                    <!--</div>-->
                    <!--<div style="    font-size: 11px;">-->
                        <!--Copyright © 2020 <u style="margin: 0 5px;cursor: pointer" @click="openMainsite()">NtfsTool</u> Inc.-->
                    <!--</div>-->
                </div>


            </div>

            <div class="fd_form">
                <div style="    font-size: 25px;">反馈表单</div>

                <div style="font-size: 14px;">请提供关于您问题的详细描述、建议、漏洞报告或者您的疑问,以便我们对您的诉求有了清晰的认识之后才能给您最有效的答复。</div>
                <div>
                    <select class="al_select al_select_lang"
                            v-model="fb_back"
                            @change="changeFeedBack()">
                        <!--自动处理-->
                        <option value="feedback">反馈</option>
                        <option value="help">支持请求</option>
                        <option value="report">错误报告</option>
                    </select>
                </div>
                <div style="    display: flex;">
                    <el-input v-model="fb_name" placeholder="john appleseed"></el-input>
                    <el-input style="margin-left: 20px;" v-model="fb_email" placeholder="someone@somewhere.com"></el-input>
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

                <div style="    font-size: 12px;">
                    <el-checkbox  v-model="agreement"  @change="changeAutoCheck()" name="type">

                    </el-checkbox>
                    提交运行数据、日志等信息，从而帮助了解您的设备信息并改善我们的应用
                </div>

                <div style="    text-align: right;
    margin-top: 10px;">
                    <button @click="sumbit_feedback" style="    font-size: 12px;
    padding: 4px 10px;">发送反馈</button>
                </div>
            </div>
        </div>
    </el-container>
</template>
<script>
    const {shell,ipcRenderer,remote} = require('electron')

    import {getPackageVersion,disableZoom} from '@/utils/utils'

    export default {
        components: {},
        data() {
            return {
                version:"-",
                fb_back:"feedback",
                fb_name:"",
                fb_email:"",
                fb_content:"",
                agreement:false,

            }
        },
        mounted() {
            disableZoom(require('electron').webFrame);
            this.version = getPackageVersion();
        },
        methods: {
            openMainsite() {
                shell.openExternal("https://www.ntfstool.com")
            },
            openGitHub(){
                shell.openExternal("https://github.com/ntfstool/ntfstool")
            },
            changeFeedBack(){

            },
            sumbit_feedback(){
                if(!this.agreement){
                    var confirm_status = confirm("建议您勾选提交运行数据、日志等信息...")
                    if (confirm_status) {
                        this.agreement = true;
                    }else{
                        confirm_status = confirm("您已放弃提交运行数据、日志等信息,我们可能无法复原您的问题,确定继续提交?")
                        if (!confirm_status) {
                            return;
                        }
                    }
                }

                alert("提交信息成功");
            }
        }
    }

</script>

<style scoped>
    .header{
        display: flex;
        width: 100%;
        height: 45px;
        line-height: 25px;
        justify-content: center;
        font-size: 15px;
        -webkit-app-region: drag;
    }
    .main-box{
        display: flex;
        flex-direction: column;
        height: 100vh;
        background: #ececec !important;
    }

    .fd_form{
        width: calc(70% - 20px);
    }

    .fd_form > div{
        padding: 5px 0;
    }
    .lb-box > div{
        text-align: center;
        padding: 2px;
    }
</style>