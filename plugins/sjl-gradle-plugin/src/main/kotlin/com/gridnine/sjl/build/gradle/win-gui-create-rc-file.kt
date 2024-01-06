/*
 * MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package com.gridnine.sjl.build.gradle

import org.gradle.api.DefaultTask
import org.gradle.api.tasks.TaskAction
import java.io.File
import java.lang.IllegalArgumentException
import javax.inject.Inject

open class CreateRcFileTask(): DefaultTask() {
    private lateinit var extension: SjlExtension

    private lateinit var launcherId:String

    @Inject
    constructor(launcherId: String, extension: SjlExtension) : this() {
        this.extension = extension
        this.launcherId = launcherId
        this.dependsOn(PrepareWinGuiWorkDirTask.getTaskName())
        group = "other"
    }

    private fun getIconString(commonConfig: SjlWinGuiLauncherConfig, launcherConfig: SjlWinGuiLauncherConfig): String{
      if ((launcherConfig.icon ?: commonConfig.icon) == null){
          return ""
      }
        return """IDI_WINLAUNCHER         ICON                    "icon.ico""""
    }

    private fun addStringParameter(key:String, sb:StringBuilder, obligatoryParameterName: String?, commonConfig: SjlWinGuiLauncherConfig, launcherConfig: SjlWinGuiLauncherConfig, extracter: (conf:SjlWinGuiLauncherConfig) -> String?){
        var value = extracter(launcherConfig)
        if(value == null){
            value = extracter(commonConfig)
        }
        if(value == null){
            if(obligatoryParameterName !== null){
                throw IllegalArgumentException("parameter $obligatoryParameterName is not defined for launcher $launcherId")
            }
            return
        }
        if(sb.isNotEmpty()){
            sb.append("\n            ")
        }
        sb.append("""  $key         "$value"""")
    }
    private fun getStringTableContent(commonConfig: SjlWinGuiLauncherConfig, launcherConfig: SjlWinGuiLauncherConfig):String{
        val sb = StringBuilder()
        addStringParameter("IDS_MAIN_CLASS", sb, "mainClass", commonConfig, launcherConfig){
            it.mainClass?.replace(".","/")
        }
        addStringParameter("IDS_CLASS_PATH", sb, "classPathProvider", commonConfig, launcherConfig){
            it.classPathProvider?.let { cp -> getEscapedWindowsAbsolutePath(cp()) }
        }
        addStringParameter("IDS_JVM_PATH", sb, null, commonConfig, launcherConfig){
            it.embeddedJvmRelativePath?.let { jp -> getEscapedWindowsAbsolutePath(jp) }
        }
        addStringParameter("IDS_VM_OPTIONS", sb, null, commonConfig, launcherConfig){
            it.vmOptions.let { o -> if(o.get().isEmpty()) null else o.get().toList().sorted().joinToString { "|" } }
        }
        addStringParameter("IDS_VM_OPTIONS_FILE", sb, null, commonConfig, launcherConfig){
            it.vmOptionsFileRelativePath?.let { o -> getEscapedWindowsAbsolutePath(o) }
        }
        addStringParameter("IDS_RESTART_EXIT_CODE", sb, null, commonConfig, launcherConfig){
            (it.restartExitCode?:79).toString()
        }
        val allowMultipleInstances =
            (launcherConfig.allowMultipleInstances ?: commonConfig.allowMultipleInstances) == true
        addStringParameter("IDS_MUTEX_NAME", sb, if(allowMultipleInstances) null else "mutexName" , commonConfig, launcherConfig){
            if (allowMultipleInstances) null else  it.mutexName?:"SJL"
        }
        addStringParameter("IDS_SJL_DIR", sb, null , commonConfig, launcherConfig){
            getEscapedWindowsAbsolutePath(it.sjlDirRelativePath?:".sjl")
        }
        return sb.toString()
    }
    @TaskAction
    fun execute(){
        val commonConfig = extension.winGuiConfig.commonConfig
        val launcherConfig = extension.winGuiConfig.launchers.find { it.first == launcherId }!!.second
        val workDir = File(project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile, CreateWinGuiLauncherTask.getDirectoryName(launcherId))
        val rcContent = """
            #include "resource.h"
            #pragma code_page(65001)
            ${getIconString(commonConfig, launcherConfig)}
            STRINGTABLE
            BEGIN
            ${getStringTableContent(commonConfig, launcherConfig)} 
            END
        """.trimIndent()
        updateIfDiffers(File(workDir, "wingui.rc"), rcContent)
    }

    companion object{
        fun getTaskName(launcherId: String) = "_sjl-win-gui-rc-file-for-${launcherId}"
    }
}