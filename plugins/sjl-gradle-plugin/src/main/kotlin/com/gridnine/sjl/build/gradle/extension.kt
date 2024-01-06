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

import groovy.lang.Closure
import org.codehaus.groovy.runtime.ConvertedClosure
import org.gradle.api.Project
import org.gradle.api.provider.SetProperty
import java.io.File
import java.lang.reflect.Proxy
import javax.inject.Inject

@DslMarker
annotation class SjlConfigMaker

@SjlConfigMaker
open class SjlExtension {
    private val project:Project

    internal val winGuiConfig: SjlWinGuiConfig
    @Inject
    constructor(project:Project){
        this.project = project
        this.winGuiConfig =  SjlWinGuiConfig(project)
    }

    fun winGui(configure: SjlWinGuiConfig.() -> Unit) {
        val cl = getClosure(configure)
        if(cl != null){
            cl.delegate = winGuiConfig
            cl.call()
        } else {
            this.winGuiConfig.configure()
        }
        val tasksNames = arrayListOf<String>()
        winGuiConfig.launchers.forEach {
            val taskName = CreateWinGuiLauncherTask.getTaskName(it.first)
            tasksNames.add(taskName)
            this.project.tasks.create(taskName, CreateWinGuiLauncherTask::class.java, it.first,this)
            this.project.tasks.create(CreateRcFileTask.getTaskName(it.first), CreateRcFileTask::class.java, it.first,this)
            this.project.tasks.create(CreateWinGuiResFileTask.getTaskName(it.first), CreateWinGuiResFileTask::class.java, it.first)
        }
        if(tasksNames.isNotEmpty()){
            this.project.tasks.create("create-all-launchers", CreateAllLaunchersTask::class.java, tasksNames, this)
            this.project.tasks.create(PrepareWinGuiWorkDirTask.getTaskName(), PrepareWinGuiWorkDirTask::class.java, this)
        }
    }

}


@SjlConfigMaker
open class SjlWinGuiConfig(private val project: Project) {

    internal val commonConfig = SjlWinGuiLauncherConfig(project)

    internal val launchers = mutableListOf<Pair<String, SjlWinGuiLauncherConfig>>()

    fun common(configure: SjlWinGuiLauncherConfig.() -> Unit) {
        val cl = getClosure(configure)
        if(cl != null){
            cl.delegate = commonConfig
            cl.call()
        } else {
            this.commonConfig.configure()
        }
    }

    fun launcher(launcherId:String, configure: SjlWinGuiLauncherConfig.() -> Unit) {
        val launcherConfig = SjlWinGuiLauncherConfig(project)
        launchers.add(launcherId to launcherConfig)
        val cl = getClosure(configure)
        if(cl != null){
            cl.delegate = launcherConfig
            cl.call()
        } else {
            launcherConfig.configure()
        }
    }
}

@SjlConfigMaker
open class SjlWinCommonConfig(private val project: Project) {

}

@SjlConfigMaker
open class SjlWinGuiLauncherConfig(private val project: Project) :SjlWinCommonConfig(project) {
    var tasksGroup:String? = null
    var dependsOnTasks:List<String>? = null
    var icon: File?    = null
    var mainClass: String? = null
    var classPathProvider : (() -> String)? = null
    var embeddedJvmRelativePath: String? = null
    var allowMultipleInstances: Boolean? = null
    var vmOptionsFileRelativePath: String? = null
    var restartExitCode:Int? = null
    var mutexName:String? = null
    var sjlDirRelativePath:String? = null
    val vmOptions: SetProperty<String> = project.objects.setProperty(String::class.java)
}



private fun getClosure(obj:Any): Closure<*>? {
    if(obj is Proxy){
        val ih = Proxy.getInvocationHandler(obj)
        if(ih is ConvertedClosure){
            return ih.delegate as Closure<*>
        }
    }
    return null
}