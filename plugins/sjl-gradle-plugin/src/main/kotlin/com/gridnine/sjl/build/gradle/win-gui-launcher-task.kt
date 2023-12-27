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
import javax.inject.Inject

open class CreateWinGuiLauncherTask(): DefaultTask(){
    private lateinit var extension: SjlExtension
    private lateinit var launcherId: String


    @Inject
    constructor(launcherId:String, extension: SjlExtension) : this() {
        this.launcherId = launcherId
        this.extension = extension;
        val commonConfig = extension.winGuiConfig.commonConfig
        val taskConfig = extension.winGuiConfig.launchers.find { it.first == launcherId }!!.second
        group = taskConfig.tasksGroup?:commonConfig.tasksGroup?:"sjl"
        val depends = arrayListOf<String>()
        taskConfig.dependsOnTasks?:commonConfig.dependsOnTasks?.let { depends.addAll(it) }
        depends.add(PrepareWinGuiWorkDirTask.getTaskName())
        dependsOn(depends)
    }

    @TaskAction
    fun execute(){
        val commonConfig = extension.winGuiConfig.commonConfig
        val taskConfig = extension.winGuiConfig.launchers.find { it.first == launcherId }!!.second
        val config = SjlWinGuiLauncherConfig()
        config.classPathProvider = taskConfig.classPathProvider?:commonConfig.classPathProvider?:throw Exception("no classpath provider is defined")
        config.icon = taskConfig.icon?:commonConfig.icon
        config.mainClass = taskConfig.mainClass?:commonConfig.mainClass?: throw Exception("no main class is defined")
        config.embeddedJvmRelativePath = taskConfig.embeddedJvmRelativePath?:commonConfig.embeddedJvmRelativePath
        config.allowMultipleInstances = taskConfig.allowMultipleInstances?:commonConfig.allowMultipleInstances?:true
        config.mutexName = taskConfig.mutexName?:commonConfig.mutexName?:"SJL"
        config.restartExitCode = taskConfig.restartExitCode?:commonConfig.restartExitCode?:79
        config.sjlDirRelativePath = taskConfig.sjlDirRelativePath?:commonConfig.sjlDirRelativePath?:".sjl"
        config.vmOptionsFileRelativePath = taskConfig.vmOptionsFileRelativePath?:commonConfig.vmOptionsFileRelativePath
        println("executing task")
    }

    companion object{
        fun getTaskName(launcherId: String):String{
            return "create-${launcherId}"
        }
        fun getFileName(launcherId: String):String{
            return "${launcherId.replace(" ", "-")}.exe"
        }
        fun getDirectoryName(launcherId: String):String{
            return "${launcherId.replace(" ", "-")}"
        }
    }
}

open class CreateAllLaunchersTask(): DefaultTask(){
    @Inject
    constructor(launchersIds:List<String>, extension: SjlExtension) : this() {
        val commonConfig = extension.winGuiConfig.commonConfig
        val taskConfig = extension.winGuiConfig.launchers.first().second
        group = taskConfig.tasksGroup?:commonConfig.tasksGroup?:"sjl"
        dependsOn(launchersIds.map { CreateWinGuiLauncherTask.getTaskName(it) })
    }
}