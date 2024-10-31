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

import org.gradle.api.tasks.Exec
import org.gradle.api.tasks.TaskAction
import org.gradle.internal.os.OperatingSystem
import java.io.ByteArrayOutputStream
import java.io.File
import javax.inject.Inject

open class CreateWinGuiLauncherTask() : Exec() {

    private lateinit var launcherId: String

    private var stdout = ByteArrayOutputStream()

    @Inject
    constructor(launcherId: String, extension: SjlExtension) : this() {
        this.launcherId = launcherId
        val commonConfig = extension.winGuiConfig.commonConfig
        val taskConfig = extension.winGuiConfig.launchers.find { it.first == launcherId }!!.second
        group = extension.commonConfig.tasksGroup ?: "sjl"
        val depends = arrayListOf<String>()
        taskConfig.generalConfig.dependsOnTasks ?: commonConfig.generalConfig.dependsOnTasks?.let { depends.addAll(it) }
        depends.add(PrepareWorkDirTask.getTaskName())
        depends.add(CreateWinGuiJsonFileTask.getTaskName(launcherId))
        dependsOn(depends)
        val workDir =
            File(project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile, getDirectoryName(launcherId))

        ensureDirectoryExists(workDir)
        //inputs.files(project.fileTree(workDir).include("win-gui-prototype.exe","win-gui.json","go-winres","go-winres.exe","param-*")).withPropertyName("inputFiles")
        val launcherFile = File(workDir, getFileName(launcherId))
        //outputs.file(launcherFile).withPropertyName("outputFile")
        workingDir = workDir
        errorOutput = stdout
        standardOutput = stdout
        val os = OperatingSystem.current()
        val arguments = arrayListOf("patch", "--in", "win-gui.json", "--delete", "--no-backup", launcherFile.name)
        if (os.isLinux) {
            executable = "./go-winres"
            args = arguments
        } else if (os.isWindows) {
            executable = File(workDir,"go-winres.exe").absolutePath
            args = arguments
        } else {
            throw Exception("unsupported operation system ${os}")
        }
    }

    @TaskAction
    override fun exec() {
        val workDir =
            File(project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile, getDirectoryName(launcherId))
        val launcherFile = File(workDir, getFileName(launcherId))
        File(workDir, "win-gui-prototype.exe").copyTo(launcherFile, true)
        try {
            super.exec()
        } catch (t: Throwable) {
            println("exception occurred, exec output:\n$stdout")
            throw t
        }
        launcherFile.copyTo(
            File(
                project.layout.buildDirectory.dir(".sjl/launchers").get().asFile,
                getFileName(launcherId)
            ), true
        )
    }

    companion object {
        fun getTaskName(launcherId: String): String {
            return "create-${launcherId}"
        }

        fun getFileName(launcherId: String): String {
            return "${launcherId.replace(" ", "-")}.exe"
        }

        fun getDirectoryName(launcherId: String): String {
            return launcherId.replace(" ", "-")
        }
    }
}