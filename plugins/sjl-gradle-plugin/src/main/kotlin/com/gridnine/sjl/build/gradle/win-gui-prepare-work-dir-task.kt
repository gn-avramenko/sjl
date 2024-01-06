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
import org.gradle.internal.os.OperatingSystem
import java.io.File
import javax.inject.Inject

open class PrepareWinGuiWorkDirTask(): DefaultTask() {
    private lateinit var extension: SjlExtension

    @Inject
    constructor(extension: SjlExtension) : this() {
        this.extension = extension
        group = "other"
    }

    @TaskAction
    fun execute(){
        val workDirs = project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile
        ensureDirectoryExists(workDirs)
        val launchersDir = project.layout.buildDirectory.dir(".sjl/launchers").get().asFile
        ensureDirectoryExists(launchersDir)
        val fileNames = extension.winGuiConfig.launchers.map {  CreateWinGuiLauncherTask.getFileName(it.first)}
        launchersDir.listFiles()?.forEach {
            if(!fileNames.contains(it.name)){
                delete(it)
            }
        }
        extension.winGuiConfig.launchers.forEach {
            val workDir = File(workDirs, CreateWinGuiLauncherTask.getDirectoryName(it.first))
            ensureDirectoryExists(workDir)
            val os = OperatingSystem.current()
            copyIfDiffers("sjl/wingui.o", File(workDir, "wingui.o"))
            copyIfDiffers("sjl/resource.h", File(workDir, "resource.h"))
            val icon = it.second.icon?:extension.winGuiConfig.commonConfig.icon
            if(icon != null){
                copyIfDiffers(icon, File(workDir, "icon.ico"))
            }
            if(os.isLinux){
                copyIfDiffers("sjl/buildTools/unix64/ld", File(workDir, "ld"), true)
                copyIfDiffers("sjl/buildTools/unix64/windres", File(workDir, "windres"), true)
                return@forEach
            }
            throw Exception("unsupported operation system ${os}")
        }
    }

    companion object{
        fun getTaskName() = "_sjl-win-gui-prepare-workdir-task"
    }
}