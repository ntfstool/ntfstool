<template>
    <el-container class="al-main">
        <div class="sudo-box">
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
                            <td><input name="workpwd"  v-on:keyup.enter="checkSudoPwd"
                                       ref="workpwd" type="password" v-model="workpwd"></td>
                        </tr>
                    </table>
                </div>
                <div class="btn-box">
                    <span :class="[btnDisable ? 'btn-disable' : 'btn-default']" @click="cancel()">取消</span>

                    <button :class="[btnDisable ? 'btn-active-disable' :  'btn-active']" @click="checkSudoPwd()">解锁</button>
                </div>
            </div>
        </div>
    </el-container>
</template>
<script>
    const {shell,ipcRenderer,remote} = require('electron')


    import {getPackageVersion, disableZoom, getSystemInfo,noticeTheSystemError} from '@/common/utils/AlfwCommon.js'

    import {
        getDiskList,
        getDiskFullInfo,
        uMountDisk,
        mountDisk,
        openInFinder} from '@/common/utils/AlfwDisk.js'
    import {alEvent} from '@/common/utils/alevent.js'

    import {savePassword} from '@/common/utils/AlfwShell.js'



    // import {disableZoom,execShell,checkSudoPassword,noticeTheSystemError,savePassword} from '@/utils/utils'

    export default {
        components: {},
        data() {
            return {
                workname:"root",
                workpwd:"",
                btnDisable:false,
            }
        },
        mounted() {
            //get the current work name
            //Todo Whoami

            this.workname = "todo list";
            
            // execShell("whoami").then(res => {
            //     if(res){
            //         this.workname = res;
            //     }
            // });

            disableZoom(require('electron').webFrame);

            //check the sudo pwd
            ipcRenderer.on("SudoPwdEvent", (e) => {
                checkSudoPassword().then(res => {
                    if(!res){
                        remote.getCurrentWindow().show();
                    }
                })
            });

        },
        methods: {
            checkSudoPwd(){
                if(this.btnDisable){
                    return;
                }

                if(!this.workpwd){
                    shell.beep()
                    this.$refs.workpwd.focus()
                    this.sharkWin();
                    return;
                }

                this.btnDisable = true;

                setTimeout(function () {
                    this.btnDisable = false;
                },10000);

                checkSudoPassword(this.workpwd).then(res => {
                    this.btnDisable = false;
                    if(!res){
                        shell.beep()
                        this.$refs.workpwd.focus()
                        this.sharkWin();
                    }else{
                        console.warn("pwd ok");
                        if(savePassword(this.workpwd)){
                            remote.getCurrentWindow().hide();
                        }else{
                            shell.beep()
                            this.sharkWin();
                            noticeTheSystemError("dialog_save_err");
                        }
                    }
                }).catch(err =>{
                    shell.beep()
                    this.btnDisable = false;
                    this.sharkWin();
                    noticeTheSystemError("dialog");
                })
            },
            cancel(){
                remote.getCurrentWindow().hide();
            },
            sharkWin(){
                var curWin =  remote.getCurrentWindow();
                var winPos = curWin.getBounds();
                if(typeof winPos.x != "undefined"){
                    const startX = winPos.x;
                    const startY = winPos.y;
                    const moveArrry = [-10, 10 , -7, 7,-3,3,0]
                    const routeArray = moveArrry.map(item => ({ x: item + startX}))
                    let _index = 0
                    function animate () {
                        if (_index === routeArray.length) return
                        setTimeout(() => {
                            curWin.setPosition(routeArray[_index].x,startY)
                            _index++
                            animate()
                        }, 60)
                    }
                    animate()
                }
            }
        }
    }

</script>

<style scoped src="../theme/dialog.scss" lang="scss"></style>