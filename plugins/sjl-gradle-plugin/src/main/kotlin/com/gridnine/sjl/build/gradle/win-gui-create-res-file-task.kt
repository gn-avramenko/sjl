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
import org.gradle.internal.os.OperatingSystem
import java.io.File
import javax.inject.Inject

open class CreateWinGuiResFileTask(): Exec(){

    @Inject
    constructor(launcherId:String) : this() {
        val workDir = File(project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile, CreateWinGuiLauncherTask.getDirectoryName(launcherId))
        ensureDirectoryExists(workDir)
        workingDir = workDir
        val os = OperatingSystem.current()
        if(os.isLinux){
            executable = "./windres"
        } else {
            throw Exception("unsupported operation system ${os}")
        }
        args =  arrayListOf("-O", "coff", "wingui.rc", "wingui.res")
        errorOutput = System.err
        standardOutput = System.out
        inputs.files(project.fileTree(workDir).exclude("*.res")).withPropertyName("sourceFiles")
        outputs.file(File(workDir, "wingui.res")).withPropertyName("outputFile")
        dependsOn(CreateRcFileTask.getTaskName(launcherId))
    }

    companion object{
        fun getTaskName(launcherId: String):String{
            return "_create-res-file-for-${launcherId}"
        }
    }
}