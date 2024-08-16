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


@file:Suppress("unused")

package com.gridnine.sjl.build.gradle

import groovy.lang.Closure
import org.codehaus.groovy.runtime.ConvertedClosure
import org.gradle.api.DefaultTask
import org.gradle.api.Project
import java.io.File
import java.lang.reflect.Proxy
import javax.inject.Inject

@DslMarker
annotation class SjlConfigMaker

@SjlConfigMaker
open class SjlExtension @Inject constructor(private val project: Project) {

    internal val commonConfig: SjlCommonConfig = SjlCommonConfig(project)

    internal val winGuiConfig: SjlWinGuiConfig = SjlWinGuiConfig(project)

    internal val nixShellConfig: SjlNixShellConfig = SjlNixShellConfig(project)

    fun all(configure: SjlCommonConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = commonConfig
            cl.call()
        } else {
            this.commonConfig.configure()
        }
    }

    fun winGui(configure: SjlWinGuiConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = winGuiConfig
            cl.call()
        } else {
            this.winGuiConfig.configure()
        }
        val tasksNames = arrayListOf<String>()
        winGuiConfig.launchers.forEach {
            val taskName = CreateWinGuiLauncherTask.getTaskName(it.first)
            tasksNames.add(taskName)
            this.project.tasks.create(taskName, CreateWinGuiLauncherTask::class.java, it.first, this)
            this.project.tasks.create(
                CreateWinGuiJsonFileTask.getTaskName(it.first),
                CreateWinGuiJsonFileTask::class.java,
                it.first,
                this
            )
        }
    }

    internal fun createTasks(){
        this.project.tasks.create("create-all-launchers", CreateAllLaunchersTask::class.java, this)
        this.project.tasks.create(PrepareWorkDirTask.getTaskName(), PrepareWorkDirTask::class.java, this)
    }


    fun nixShell(configure: SjlNixShellConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = nixShellConfig
            cl.call()
        } else {
            this.nixShellConfig.configure()
        }
        val tasksNames = arrayListOf<String>()
        nixShellConfig.launchers.forEach {
            val taskName = CreateNixShellLauncherTask.getTaskName(it.first)
            tasksNames.add(taskName)
            this.project.tasks.create(taskName, CreateNixShellLauncherTask::class.java, it.first, this)
        }
    }
}


@SjlConfigMaker
open class SjlWinGuiConfig(private val project: Project) {

    internal val commonConfig = SjlWinGuiLauncherConfig(project)

    internal val launchers = mutableListOf<Pair<String, SjlWinGuiLauncherConfig>>()

    fun common(configure: SjlWinGuiLauncherConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = commonConfig
            cl.call()
        } else {
            this.commonConfig.configure()
        }
    }

    fun launcher(launcherId: String, configure: SjlWinGuiLauncherConfig.() -> Unit) {
        val launcherConfig = SjlWinGuiLauncherConfig(project)
        launchers.add(launcherId to launcherConfig)
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = launcherConfig
            cl.call()
        } else {
            launcherConfig.configure()
        }
    }
}

@SjlConfigMaker
open class SjlNixShellConfig(private val project: Project) {

    internal val commonConfig = SjlNixShellLauncherConfig(project)

    internal val launchers = mutableListOf<Pair<String, SjlNixShellLauncherConfig>>()

    fun common(configure: SjlNixShellLauncherConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = commonConfig
            cl.call()
        } else {
            this.commonConfig.configure()
        }
    }

    fun launcher(launcherId: String, configure: SjlNixShellLauncherConfig.() -> Unit) {
        val launcherConfig = SjlNixShellLauncherConfig(project)
        launchers.add(launcherId to launcherConfig)
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = launcherConfig
            cl.call()
        } else {
            launcherConfig.configure()
        }
    }
}

@SjlConfigMaker
open class SjlCommonConfig(private val project: Project) {
    var tasksGroup: String? = null
}

@SjlConfigMaker
open class SjlWinCommonConfig(private val project: Project)

@SjlConfigMaker
open class SjlNixCommonConfig(private val project: Project)

@SjlConfigMaker
open class EmbeddedJavaConfig {
    var relativePath: String? = null
}

@SjlConfigMaker
open class InstalledJavaConfig {
    var minJavaVersion: Int = 0
    var maxJavaVersion: Int = 0
    var required64JRE = true
}

@SjlConfigMaker
open class SplashScreenConfig {
    lateinit var relativePath: String
}

@SjlConfigMaker
open class SjlWinGuiMessagesConfig {
    var standardErrorMessage: String? = null
    var unableToOpenFileMessage: String? = null
    var unableToCopyDirectoryMessage: String? = null
    var unableToCreateDirectoryMessage: String? = null
    var sourceFileDoesNotExistMessage: String? = null
    var unableToCopyFileMessage: String? = null
    var unableToDeleteDirectoryMessage: String? = null
    var unableToDeleteFileMessage: String? = null
    var unableToRenameFileMessage: String? = null
    var unableToPerformSelfUpdateMessage: String? = null
    var unableToLoadBitmapMessage: String? = null
    var unableToLocateJvmDllMessage: String? = null
    var unableToLoadJvmDllMessage: String? = null
    var unableToFindAddressOfJNI_CreateJavaVMMessage: String? = null
    var classPathIsNotDefinedMessage: String? = null
    var errorTitle: String? = null
    var unableToCreateJVMMessage: String? = null
    var unableToFindMainClassMessage: String? = null
    var instanceAlreadyRunningMessage: String? = null
    var unableToFindMainMethodMessage: String? = null
    var errorInvokingMainMethodMessage: String? = null
    var javaHomeIsNotDefinedMessage: String? = null
    var unableToCheckInstalledJavaMessage: String? = null
    var wrongJavaTypeMessage: String? = null
}

enum class EXECUTION_LEVEL {
    DEFAULT {
        override fun getCode() = ""
    },
    HIGHEST {
        override fun getCode() = "highest"
    },
    ADMINISTRATOR {
        override fun getCode() = "administrator"
    };

    abstract fun getCode(): String
}

enum class ARCH {
    X64,
    WIN32
}

enum class OS {
    VISTA {
        override fun getCode() = "vista"
    },
    WIN7 {
        override fun getCode() = "win7"
    },
    WIN8 {
        override fun getCode() = "win8"
    },
    WIN8_1 {
        override fun getCode() = "win8.1"
    },
    WIN10 {
        override fun getCode() = "win10"
    };

    abstract fun getCode(): String
}

enum class DPI_AWARENESS {
    UNAWARE {
        override fun getCode() = "unaware"
    },
    SYSTEM {
        override fun getCode() = "system"
    },
    PER_MONITOR {
        override fun getCode() = "per monitor"
    },
    PER_MONITOR_V2 {
        override fun getCode() = "per monitor v2"
    };

    abstract fun getCode(): String
}


@SjlConfigMaker
open class SjlWinVersionInfoConfig {
    var fileVersion: String? = null
    var productVersion: String? = null
    var flags: String? = null
    var timestamp: String? = null
    var comments: String? = null
    var companyName: String? = null
    var fileDescription: String? = null
    var internalName: String? = null
    var legalCopyright: String? = null
    var legalTrademarks: String? = null
    var originalFilename: String? = null
    var privateBuild: String? = null
    var productName: String? = null
    var specialBuild: String? = null
}

@SjlConfigMaker
open class SjlWinManifestConfig {
    internal var identityName: String? = null
    internal var identityVersion: String? = null

    fun identity(name: String, version: String) {
        identityName = name
        identityVersion = version
    }

    var description: String? = null
    var minimumOs: OS? = null
    var executionLevel: EXECUTION_LEVEL? = null
    var uiAccess: Boolean = true
    var autoElevate: Boolean? = null
    var dpiAwareness: DPI_AWARENESS? = null
    var disableTheming: Boolean? = null
    var disableWindowFiltering: Boolean? = null
    var highResolutionScrollingAware: Boolean? = null
    var ultraHighResolutionScrollingAware: Boolean? = null
    var longPathAware: Boolean? = null
    var printerDriverIsolation: Boolean? = null
    var gdiScaling: Boolean? = null
    var segmentHeap: Boolean? = null
    var useCommonControlsV6: Boolean? = null
}

@SjlConfigMaker
open class SjlWinGuiGeneralConfig {
    var dependsOnTasks: List<String>? = null
    var icon: File? = null
    var architecture: ARCH = ARCH.X64
    var allowMultipleInstances: Boolean? = null
    var mutexName: String? = null
    var sjlDirRelativePath: String? = null
    var instanceAlreadyRunningExitCode: Int? = null
    internal var showSplashScreen: Boolean? = null
    internal var splashScreenConfig: SplashScreenConfig? = null

    fun showSplashScreen(configure: SplashScreenConfig.() -> Unit) {
        showSplashScreen = true
        splashScreenConfig = SplashScreenConfig()
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = splashScreenConfig
            cl.call()
        } else {
            splashScreenConfig!!.configure()
        }
    }
}

@SjlConfigMaker
open class SjlNixShellGeneralConfig {
    var dependsOnTasks: List<String>? = null
    var allowMultipleInstances: Boolean? = null
    var mutexName: String? = null
    var sjlDirRelativePath: String? = null
    var instanceAlreadyRunningExitCode: Int? = null
}

@SjlConfigMaker
open class SjlWinGuiJavaConfig {
    internal var useInstalledJava: Boolean? = null
    internal var embeddedJavaConfig: EmbeddedJavaConfig? = null
    internal var installedJavaConfig: InstalledJavaConfig? = null
    var classPathProvider: (() -> String)? = null
    var mainClass: String? = null
    var vmOptionsFileRelativePath: String? = null
    var restartExitCode: Int? = null
    var vmOptions: List<String>? = null

    fun useInstalledJava(configure: InstalledJavaConfig.() -> Unit) {
        useInstalledJava = true
        installedJavaConfig = InstalledJavaConfig()
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = installedJavaConfig
            cl.call()
        } else {
            installedJavaConfig!!.configure()
        }
    }

    fun useEmbeddedJava(configure: EmbeddedJavaConfig.() -> Unit) {
        useInstalledJava = false
        embeddedJavaConfig = EmbeddedJavaConfig()
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = embeddedJavaConfig
            cl.call()
        } else {
            embeddedJavaConfig!!.configure()
        }
    }
}

@SjlConfigMaker
open class SjlNixShellJavaConfig {
    internal var useInstalledJava: Boolean? = null
    internal var embeddedJavaConfig: EmbeddedJavaConfig? = null
    internal var installedJavaConfig: InstalledJavaConfig? = null
    var classPathProvider: (() -> String)? = null
    var mainClass: String? = null
    var vmOptionsFileRelativePath: String? = null
    var restartExitCode: Int? = null
    var vmOptions: List<String>? = null

    fun useInstalledJava(configure: InstalledJavaConfig.() -> Unit) {
        useInstalledJava = true
        installedJavaConfig = InstalledJavaConfig()
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = installedJavaConfig
            cl.call()
        } else {
            installedJavaConfig!!.configure()
        }
    }

    fun useEmbeddedJava(configure: EmbeddedJavaConfig.() -> Unit) {
        useInstalledJava = false
        embeddedJavaConfig = EmbeddedJavaConfig()
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = embeddedJavaConfig
            cl.call()
        } else {
            embeddedJavaConfig!!.configure()
        }
    }
}

@SjlConfigMaker
open class SjlWinGuiLauncherConfig(private val project: Project) : SjlWinCommonConfig(project) {
    internal val javaConfig = SjlWinGuiJavaConfig()
    internal val messagesConfig = SjlWinGuiMessagesConfig()
    internal var versionConfig: SjlWinVersionInfoConfig? = null
    internal var manifestConfig: SjlWinManifestConfig? = null
    internal val generalConfig = SjlWinGuiGeneralConfig()

    fun general(configure: SjlWinGuiGeneralConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = generalConfig
            cl.call()
        } else {
            generalConfig.configure()
        }
    }

    fun java(configure: SjlWinGuiJavaConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = javaConfig
            cl.call()
        } else {
            javaConfig.configure()
        }
    }

    fun messages(configure: SjlWinGuiMessagesConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = messagesConfig
            cl.call()
        } else {
            messagesConfig.configure()
        }
    }

    fun version(configure: SjlWinVersionInfoConfig.() -> Unit) {
        versionConfig = SjlWinVersionInfoConfig()
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = versionConfig
            cl.call()
        } else {
            versionConfig!!.configure()
        }
    }

    fun manifest(configure: SjlWinManifestConfig.() -> Unit) {
        manifestConfig = SjlWinManifestConfig()
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = manifestConfig
            cl.call()
        } else {
            manifestConfig!!.configure()
        }
    }

}

@SjlConfigMaker
open class SjlNixShellLauncherConfig(private val project: Project) : SjlNixCommonConfig(project) {
    internal val generalConfig = SjlNixShellGeneralConfig()
    internal val javaConfig = SjlNixShellJavaConfig()

    fun general(configure: SjlNixShellGeneralConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = generalConfig
            cl.call()
        } else {
            generalConfig.configure()
        }
    }

    fun java(configure: SjlNixShellJavaConfig.() -> Unit) {
        val cl = getClosure(configure)
        if (cl != null) {
            cl.delegate = javaConfig
            cl.call()
        } else {
            javaConfig.configure()
        }
    }
}

private fun getClosure(obj: Any): Closure<*>? {
    if (obj is Proxy) {
        val ih = Proxy.getInvocationHandler(obj)
        if (ih is ConvertedClosure) {
            return ih.delegate as Closure<*>
        }
    }
    return null
}

open class CreateAllLaunchersTask() : DefaultTask() {
    @Inject
    constructor(extension: SjlExtension) : this() {
        group = extension.commonConfig.tasksGroup ?: "sjl"
        val taskNames = arrayListOf<String>()
        extension.winGuiConfig.launchers.forEach {
            taskNames.add(CreateWinGuiLauncherTask.getTaskName(it.first))
        }
        extension.nixShellConfig.launchers.forEach {
            taskNames.add(CreateNixShellLauncherTask.getTaskName(it.first))
        }
        dependsOn(taskNames)
    }
}
