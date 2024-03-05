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

import com.google.gson.GsonBuilder
import com.google.gson.JsonObject
import org.gradle.api.DefaultTask
import org.gradle.api.tasks.TaskAction
import java.io.File
import java.util.concurrent.atomic.AtomicInteger
import javax.inject.Inject

open class CreateWinGuiJsonFileTask() : DefaultTask() {
    private lateinit var extension: SjlExtension

    private lateinit var launcherId: String

    @Inject
    constructor(launcherId: String, extension: SjlExtension) : this() {
        this.extension = extension
        this.launcherId = launcherId
        val commonConfig = extension.winGuiConfig.commonConfig
        val taskConfig = extension.winGuiConfig.launchers.find { it.first == launcherId }!!.second
        val depends = arrayListOf<String>()
        taskConfig.generalConfig.dependsOnTasks ?: commonConfig.generalConfig.dependsOnTasks?.let { depends.addAll(it) }
        depends.add(PrepareWorkDirTask.getTaskName())
        depends.add(PrepareWorkDirTask.getTaskName())
        dependsOn(depends)
        this.dependsOn(depends)
        group = "other"
    }

    private fun addCommonParameter(rcDataEntry: JsonObject, name: String, value: String, counter: AtomicInteger) {
        val entry = JsonObject()
        rcDataEntry.add(name, entry)
        entry.addProperty("0000", createParameter(counter, value))
    }

    private fun createParameter(counter: AtomicInteger, value: String): String {
        val file = File(
            File(
                project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile,
                CreateWinGuiLauncherTask.getDirectoryName(launcherId)
            ), "param-${counter.incrementAndGet()}.txt"
        )
        updateIfDiffers(file, value)
        return file.name
    }


    private fun addErrorMessage(
        rcDataEntry: JsonObject,
        parameterName: String,
        launcherConfig: SjlWinGuiLauncherConfig,
        commonConfig: SjlWinGuiLauncherConfig,
        extracter: (SjlWinGuiMessagesConfig) -> String?,
        defaultValue: String,
        counter: AtomicInteger
    ) {
        addLocaleEntry(
            rcDataEntry,
            parameterName,
            launcherConfig,
            commonConfig,
            { extracter.invoke(it) ?: it.standardErrorMessage },
            defaultValue,
            counter
        )
    }

    private fun addLocaleEntry(
        rcDataEntry: JsonObject,
        parameterName: String,
        launcherConfig: SjlWinGuiLauncherConfig,
        commonConfig: SjlWinGuiLauncherConfig,
        extracter: (SjlWinGuiMessagesConfig) -> String?,
        defaultValue: String,
        counter: AtomicInteger
    ) {
        val message = createParameter(counter, extracter.invoke(launcherConfig.messagesConfig)?:extracter.invoke(commonConfig.messagesConfig)?:defaultValue)
        val localeEntry = JsonObject();
        rcDataEntry.add(parameterName, localeEntry)
        localeEntry.addProperty("0000", message)
    }

    @TaskAction
    fun execute() {
        val counter = AtomicInteger(0);
        val commonConfig = extension.winGuiConfig.commonConfig
        val launcherConfig = extension.winGuiConfig.launchers.find { it.first == launcherId }!!.second
        val workDir = File(
            project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile,
            CreateWinGuiLauncherTask.getDirectoryName(launcherId)
        )
        val root = JsonObject();
        val iconFile = launcherConfig.generalConfig.icon ?: commonConfig.generalConfig.icon
        if (iconFile != null) {
            val iconEntry = JsonObject();
            root.add("RT_GROUP_ICON", iconEntry);
            val appEntry = JsonObject();
            iconEntry.add("APP", appEntry);
            appEntry.addProperty("0000", iconFile.toRelativeString(workDir));
        }
        launcherConfig.manifestConfig ?: commonConfig.manifestConfig?.let { config ->
            val manifestEntry = JsonObject();
            root.add("RT_MANIFEST", manifestEntry)
            val subManifestEntry = JsonObject();
            manifestEntry.add("#1", subManifestEntry);
            val languageEntry = JsonObject();
            subManifestEntry.add("0000", languageEntry);
            if (!config.identityName.isNullOrBlank() || !config.identityVersion.isNullOrBlank()) {
                val identityEntry = JsonObject()
                config.identityName?.let { identityEntry.addProperty("name", it) }
                config.identityVersion?.let { identityEntry.addProperty("version", it) }
            }
            config.description?.let { languageEntry.addProperty("description", it) }
            config.minimumOs?.let { languageEntry.addProperty("minimum-os", it.getCode()) }
            config.executionLevel?.let { languageEntry.addProperty("execution-level", it.getCode()) }
            config.dpiAwareness?.let { languageEntry.addProperty("dpi-awareness", it.getCode()) }
            config.autoElevate?.let { languageEntry.addProperty("auto-elevate", it) }
            config.uiAccess?.let { languageEntry.addProperty("ui-access", it) }
            config.disableTheming?.let { languageEntry.addProperty("disable-theming", it) }
            config.disableWindowFiltering?.let { languageEntry.addProperty("disable-window-filtering", it) }
            config.highResolutionScrollingAware?.let {
                languageEntry.addProperty(
                    "high-resolution-scrolling-aware",
                    it
                )
            }
            config.ultraHighResolutionScrollingAware?.let {
                languageEntry.addProperty(
                    "ultra-high-resolution-scrolling-aware",
                    it
                )
            }
            config.longPathAware?.let { languageEntry.addProperty("long-path-aware", it) }
            config.printerDriverIsolation?.let { languageEntry.addProperty("printer-driver-isolation", it) }
            config.gdiScaling?.let { languageEntry.addProperty("gdi-scaling", it) }
            config.segmentHeap?.let { languageEntry.addProperty("segment-heap", it) }
            config.useCommonControlsV6?.let { languageEntry.addProperty("use-common-controls-v6", it) }
        }
        launcherConfig.versionConfig ?: commonConfig.versionConfig?.let { config ->
            val versionEntry = JsonObject();
            root.add("RT_VERSION", versionEntry);
            val subVersionEntry = JsonObject();
            versionEntry.add("#1", subVersionEntry);
            val subSubVersionEntry = JsonObject();
            subVersionEntry.add("0000", subSubVersionEntry);
            if (config.fileVersion != null || config.productVersion != null || config.flags != null || config.timestamp != null) {
                val fixedEntry = JsonObject();
                subSubVersionEntry.add("fixed", fixedEntry);
                config.fileVersion?.let { fixedEntry.addProperty("file_version", it) }
                config.productVersion?.let { fixedEntry.addProperty("product_version", it) }
                config.flags?.let { fixedEntry.addProperty("flags", it) }
                config.timestamp?.let { fixedEntry.addProperty("timestamp", it) }
            }
            val infoEntry = JsonObject();
            subSubVersionEntry.add("info", infoEntry);
            val languageEntry = JsonObject();
            infoEntry.add("0000", languageEntry);
            config.comments?.let { languageEntry.addProperty("Comments", it) }
            config.companyName?.let { languageEntry.addProperty("CompanyName", it) }
            config.fileDescription?.let { languageEntry.addProperty("FileDescription", it) }
            config.fileVersion?.let { languageEntry.addProperty("FileVersion", it) }
            config.internalName?.let { languageEntry.addProperty("InternalName", it) }
            config.legalCopyright?.let { languageEntry.addProperty("LegalCopyright", it) }
            config.legalTrademarks?.let { languageEntry.addProperty("LegalTrademarks", it) }
            config.originalFilename?.let { languageEntry.addProperty("OriginalFilename", it) }
            config.privateBuild?.let { languageEntry.addProperty("PrivateBuild", it) }
            config.productName?.let { languageEntry.addProperty("ProductName", it) }
            config.productVersion?.let { languageEntry.addProperty("ProductVersion", it) }
            config.specialBuild?.let { languageEntry.addProperty("SpecialBuild", it) }
        }
        val rcDataEntry = JsonObject()
        root.add("RT_RCDATA", rcDataEntry)
        addCommonParameter(
            rcDataEntry,
            "SJL_PATH",
            launcherConfig.generalConfig.sjlDirRelativePath ?: commonConfig.generalConfig.sjlDirRelativePath ?: ".sjl",
            counter
        )
        if ((launcherConfig.generalConfig.allowMultipleInstances ?: commonConfig.generalConfig.allowMultipleInstances) != true) {
            addCommonParameter(
                rcDataEntry,
                "MUTEX_NAME",
                launcherConfig.generalConfig.mutexName ?: commonConfig.generalConfig.mutexName ?: "SJL-MUTEX",
                counter
            )
        }
        addCommonParameter(
            rcDataEntry, "INSTANCE_ALREADY_RUNNING_EXIT_CODE",
            (launcherConfig.generalConfig.instanceAlreadyRunningExitCode ?: commonConfig.generalConfig.instanceAlreadyRunningExitCode
            ?: 0).toString(), counter
        )
        if ((launcherConfig.generalConfig.showSplashScreen ?: commonConfig.generalConfig.showSplashScreen) == true) {
            addCommonParameter(
                rcDataEntry,
                "SPLASH_SCREEN_FILE",
                launcherConfig.generalConfig.splashScreenConfig?.relativePath ?: commonConfig.generalConfig.splashScreenConfig?.relativePath
                ?: throw IllegalArgumentException("splashScreenConfig.relativePath is not defined"),
                counter
            )
        }
        val vmOptionsFile = launcherConfig.javaConfig.vmOptionsFileRelativePath ?: commonConfig.javaConfig.vmOptionsFileRelativePath
        if (!vmOptionsFile.isNullOrBlank()) {
            addCommonParameter(rcDataEntry, "VM_OPTIONS_FILE", vmOptionsFile, counter)
        }
        val vmOptions = launcherConfig.javaConfig.vmOptions ?: commonConfig.javaConfig.vmOptions
        vmOptions?.let { addCommonParameter(rcDataEntry, "VM_OPTIONS", it.joinToString("|"), counter) }
        val useInstalledJava = (launcherConfig.javaConfig.useInstalledJava ?: commonConfig.javaConfig.useInstalledJava) == true
        addCommonParameter(rcDataEntry, "USE_INSTALLED_JAVA", useInstalledJava.toString(), counter)
        if (useInstalledJava) {
            addCommonParameter(
                rcDataEntry,
                "MIN_JAVA_VERSION",
                ((launcherConfig.javaConfig.installedJavaConfig ?: commonConfig.javaConfig.installedJavaConfig)!!.minJavaVersion).toString(),
                counter
            )
            addCommonParameter(
                rcDataEntry,
                "MAX_JAVA_VERSION",
                ((launcherConfig.javaConfig.installedJavaConfig ?: commonConfig.javaConfig.installedJavaConfig)!!.maxJavaVersion).toString(),
                counter
            )
            val require64Bit = (launcherConfig.javaConfig.installedJavaConfig ?: commonConfig.javaConfig.installedJavaConfig)!!.required64JRE
            addCommonParameter(rcDataEntry, "REQUIRES_64_BIT", require64Bit.toString(), counter)
        } else {
            addCommonParameter(
                rcDataEntry,
                "EMBEDDED_JAVA_HOME",
                (launcherConfig.javaConfig.embeddedJavaConfig ?: commonConfig.javaConfig.embeddedJavaConfig)!!.relativePath
                    ?: throw IllegalArgumentException("embeddedJavaConfig.relativePath is not defined"),
                counter
            )
        }
        addCommonParameter(
            rcDataEntry,
            "MAIN_CLASS",
            (launcherConfig.javaConfig.mainClass ?: commonConfig.javaConfig.mainClass)?.replace(".", "/")
            ?: throw IllegalArgumentException("main class is not defined"),
            counter
        )
        addCommonParameter(
            rcDataEntry,
            "RESTART_EXIT_CODE",
            (launcherConfig.javaConfig.restartExitCode ?: commonConfig.javaConfig.restartExitCode ?: 79).toString(),
            counter
        )
        addCommonParameter(
            rcDataEntry,
            "CLASS_PATH",
            (launcherConfig.javaConfig.classPathProvider ?: commonConfig.javaConfig.classPathProvider
            ?: throw IllegalArgumentException("class path provider is not defined")).invoke(),
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_OPEN_FILE_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToOpenFileMessage },
            "Unable to open file %s",
            counter
        );
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_COPY_DIRECTORY_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToCopyDirectoryMessage },
            "Unable to copy directory from %s to %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "SOURCE_FILE_DOES_NOT_EXIST_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.sourceFileDoesNotExistMessage },
            "Source file %s does not exist",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_COPY_FILE_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToCopyFileMessage },
            "Unable to copy file from %s to %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_DELETE_DIRECTORY_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToDeleteDirectoryMessage },
            "Unable to delete %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_DELETE_FILE_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToDeleteFileMessage },
            "Unable to delete %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_RENAME_FILE_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToRenameFileMessage },
            "Unable to rename file %s to %s",
            counter
        );
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_PERFORM_SELF_UPDATE_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToPerformSelfUpdateMessage },
            "Unable to perform self update",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_LOAD_BITMAP_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToLoadBitmapMessage },
            "Unable to load bitmap from %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_LOCATE_JVM_DLL_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToLocateJvmDllMessage },
            "Unable to locate jvm.dll in %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_LOAD_JVM_DLL_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToLoadJvmDllMessage },
            "Unable to load JVM from %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_FIND_ADDRESS_OF_CREATE_JVM_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToFindAdressOfJNI_CreateJavaVMMessage },
            "Unable to find address of JNI_CreateJavaVM",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "CLASS_PATH_IS_NOT_DEFINED_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.classPathIsNotDefinedMessage },
            "Classpath is not defined",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_CREATE_DIRECTORY_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToCreateDirectoryMessage },
            "Unable to create directory %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_CREATE_JVM_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToCreateJVMMessage },
            "Unable to create JVM",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_FIND_MAIN_CLASS_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToFindMainClassMessage },
            "Unable to find main class %s",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "INSTANCE_ALREADY_RUNNING_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.instanceAlreadyRunningMessage },
            "Instance of the application is already running",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_FIND_MAIN_METHOD_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToFindMainMethodMessage },
            "Unable to find main method",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "ERROR_INVOKING_MAIN_METHOD_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.errorInvokingMainMethodMessage },
            "Exception occurred while invoking main method",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "JAVA_HOME_IS_NOT_DEFINED_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.javaHomeIsNotDefinedMessage },
            "JAVA_HOME environment variable is not defined",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "UNABLE_TO_CHECK_INSTALLED_JAVA_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.unableToCheckInstalledJavaMessage },
            "Unable to check installed java",
            counter
        )
        addErrorMessage(
            rcDataEntry,
            "WRONG_JAVA_TYPE_MESSAGE",
            launcherConfig,
            commonConfig,
            { it.wrongJavaTypeMessage },
            "Wrong Java Version: required %s min version=%d max version %d, found %s version = %d",
            counter
        )
        addLocaleEntry(rcDataEntry, "ERROR_TITLE", launcherConfig, commonConfig, { it.errorTitle }, "Error", counter)
        val jsonContent = GsonBuilder().setPrettyPrinting().create().toJson(root)
        updateIfDiffers(File(workDir, "win-gui.json"), jsonContent)
    }

    companion object {
        fun getTaskName(launcherId: String) = "_sjl-win-gui-json-file-for-${launcherId}"
    }
}