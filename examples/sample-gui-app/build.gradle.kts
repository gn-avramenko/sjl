import org.gradle.kotlin.dsl.support.unzipTo

plugins {
    java
}

repositories{
    mavenCentral()
}
java{
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(8))
    }
}

task("dist") {
    group= "dist"
    dependsOn("jar")
    doLast{
        val distDir = project.file("dist")
        if(!distDir.exists()){
            distDir.mkdirs()
        }
        val jdkDir = File(distDir, "jdk")
        if(!jdkDir.exists()){
            val jdkArchiveFile = File(distDir, "jdk.zip")
            uri("https://corretto.aws/downloads/latest/amazon-corretto-21-x64-windows-jdk.zip").toURL().openStream().use {ins ->
                jdkArchiveFile.outputStream().use { os ->
                    ins.copyTo(os)
                    os.flush()
                }
            }
            unzipTo(distDir, jdkArchiveFile)
            distDir.listFiles()!!.find { it.isDirectory && it.name.startsWith("jdk") }!!.renameTo(jdkDir)
            jdkArchiveFile.delete()
        }
        val libFile = File(distDir, "win-gui.jar")
        if(libFile.exists()) {
            libFile.delete()
        }
        project.file("build/libs/win-gui.jar").copyTo(libFile)
        val launchFile = File(distDir, "run.bat")
        if(!launchFile.exists()){
            launchFile.writeText("jdk\\bin\\javaw.exe -cp win-gui.jar com.gridnine.sjl.example.winGui.WinGui")
        }
    }
}