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

open class PrepareWorkDirTask() : DefaultTask() {
    private lateinit var extension: SjlExtension

    @Inject
    constructor(extension: SjlExtension) : this() {
        this.extension = extension
        group = "other"
    }

    @TaskAction
    fun execute() {
        val workDirs = project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile
        ensureDirectoryExists(workDirs)
        val launchersDir = project.layout.buildDirectory.dir(".sjl/launchers").get().asFile
        ensureDirectoryExists(launchersDir)
        val winFileNames = extension.winGuiConfig.launchers.map { CreateWinGuiLauncherTask.getFileName(it.first) }
        val nixFileNames = extension.nixShellConfig.launchers.map { CreateNixShellLauncherTask.getFileName(it.first) }
        launchersDir.listFiles()?.forEach {
            if (!winFileNames.contains(it.name) && !nixFileNames.contains(it.name)) {
                delete(it)
            }
        }

        extension.winGuiConfig.launchers.forEach {
            val workDir = File(workDirs, CreateWinGuiLauncherTask.getDirectoryName(it.first))
            ensureDirectoryExists(workDir)
            val os = OperatingSystem.current()
            copyIfDiffers(
                if (it.second.generalConfig.architecture == ARCH.WIN32) "launchers/win-gui-win32.exe" else "launchers/win-gui-x64.exe",
                File(workDir, "win-gui-prototype.exe")
            )
            val arch = System.getProperty("os.arch")
            if (os.isLinux) {
                if ("x86_64" == arch || "amd64" == arch) {
                    copyIfDiffers("buildTools/nix64/go-winres", File(workDir, "go-winres"), true)
                    return@forEach
                }
                if ("x86" == arch) {
                    throw Exception("unsupported operation system arch ${arch}")
                }
                throw Exception("unsupported operation system arch ${arch}")
            }
            if (os.isWindows) {
                if ("i386" == arch) {
                    throw Exception("unsupported operation system arch ${arch}")
                }
                copyIfDiffers("buildTools/win64/go-winres.exe", File(workDir, "go-winres.exe"), true)
                return@forEach
            }
            throw Exception("unsupported operation system ${os}")
        }

        extension.nixShellConfig.launchers.forEach {
            val workDir = File(workDirs, CreateNixShellLauncherTask.getDirectoryName(it.first))
            ensureDirectoryExists(workDir)
            copyIfDiffers("launchers/nix-shell-template.sh", File(workDir, "nix-shell-template.sh"))
        }
    }

    companion object {
        fun getTaskName() = "_sjl-prepare-workdir-task"
    }
}