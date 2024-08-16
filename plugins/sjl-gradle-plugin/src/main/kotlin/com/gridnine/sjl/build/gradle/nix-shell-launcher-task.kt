package com.gridnine.sjl.build.gradle

import org.gradle.api.tasks.Exec
import org.gradle.api.tasks.TaskAction
import java.io.ByteArrayOutputStream
import java.io.File
import javax.inject.Inject

open class CreateNixShellLauncherTask() : Exec() {

    private lateinit var launcherId: String

    private lateinit var nixShellConfig: SjlNixShellConfig

    private var stdout = ByteArrayOutputStream()

    @Inject
    constructor(launcherId: String, extension: SjlExtension) : this() {
        this.launcherId = launcherId
        this.nixShellConfig = extension.nixShellConfig
        val commonConfig = nixShellConfig.commonConfig
        val taskConfig = nixShellConfig.launchers.find { it.first == launcherId }!!.second
        group = extension.commonConfig.tasksGroup ?: "sjl"
        val depends = arrayListOf<String>()
        taskConfig.generalConfig.dependsOnTasks ?: commonConfig.generalConfig.dependsOnTasks?.let { depends.addAll(it) }
        depends.add(PrepareWorkDirTask.getTaskName())
        dependsOn(depends)
        val workDir =
            File(project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile, getDirectoryName(launcherId))
        ensureDirectoryExists(workDir)
        workingDir = workDir
        errorOutput = stdout
        standardOutput = stdout
    }

    @TaskAction
    override fun exec() {
        val workDir =
            File(project.layout.buildDirectory.dir(".sjl/workdirs").get().asFile, getDirectoryName(launcherId))
        val launcherFile = File(workDir, getFileName(launcherId))
        File(workDir, "nix-shell-template.sh").copyTo(launcherFile, true)
        var content = launcherFile.readText()
        content = content.replace(
            "ALLOW_MULTIPLE_INSTANCES_PLACEHOLDER",
            nixShellConfig.commonConfig.generalConfig.allowMultipleInstances?.toString() ?: "false"
        )
        content = content.replace(
            "INSTANCE_ALREADY_RUNNING_EXIT_CODE_PLACEHOLDER",
            nixShellConfig.commonConfig.generalConfig.instanceAlreadyRunningExitCode?.toString() ?: "\"1\""
        )
        content = content.replace(
            "JAVA_CLASS_PATH_PROVIDER_PLACEHOLDER",
            "\"${nixShellConfig.commonConfig.javaConfig.classPathProvider!!.invoke()}\""
        )
        content = content.replace(
            "JAVA_VM_OPTIONS_PLACEHOLDER",
            "\"${nixShellConfig.commonConfig.javaConfig.vmOptions?.joinToString(" ")}\""
        )
        content = content.replace(
            "JAVA_VM_OPTIONS_FILE_RELATIVE_PATH_PLACEHOLDER",
            "\"${nixShellConfig.commonConfig.javaConfig.vmOptionsFileRelativePath}\""
        )
        content = content.replace(
            "JAVA_MAIN_CLASS_PLACEHOLDER",
            "\"${nixShellConfig.commonConfig.javaConfig.mainClass.toString()}\""
        )
        content = content.replace(
            "JAVA_RESTART_EXIT_CODE_PLACEHOLDER",
            "\"${nixShellConfig.commonConfig.javaConfig.restartExitCode?.toString()}\""
        )
        content = content.replace(
            "EMBEDDED_JRE_PATH_PLACEHOLDER",
            "\"${nixShellConfig.commonConfig.javaConfig.embeddedJavaConfig?.relativePath}\""
        )
        launcherFile.writeText(content)
        launcherFile.copyTo(
            File(
                project.layout.buildDirectory.dir(".sjl/launchers").get().asFile,
                getFileName(launcherId)
            ), true
        ).setExecutable(true)
    }

    companion object {
        fun getTaskName(launcherId: String): String {
            return "create-${launcherId}"
        }

        fun getFileName(launcherId: String): String {
            return "${launcherId.replace(" ", "-")}.sh"
        }

        fun getDirectoryName(launcherId: String): String {
            return launcherId.replace(" ", "-")
        }
    }
}
