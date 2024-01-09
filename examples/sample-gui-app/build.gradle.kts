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

buildscript {
    repositories{
        mavenCentral()
    }
    dependencies{
        classpath("org.jetbrains.kotlin:kotlin-stdlib:1.9.10")
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
        val libFile = File(distDir, "sample-gui-app.jar")
        if(libFile.exists()) {
            libFile.delete()
        }
        project.file("build/libs/sample-gui-app.jar").copyTo(libFile)
        val launchFile = File(distDir, "run.bat")
        if(!launchFile.exists()){
            launchFile.writeText("jdk\\bin\\javaw.exe -cp sample-gui-app.jar com.gridnine.sjl.example.winGui.WinGui")
        }
    }
}