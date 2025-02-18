import org.gradle.kotlin.dsl.support.unzipTo

plugins {
    java
}

repositories{
    mavenCentral()
}
dependencies {
    implementation("org.slf4j:slf4j-api:2.0.16")
    implementation("ch.qos.logback:logback-classic:1.5.8")
}
java{
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(17))
    }
}


buildscript {
    repositories{
        mavenCentral()
    }
    dependencies{
        classpath("org.jetbrains.kotlin:kotlin-stdlib:1.7.10")
    }
}

configurations.create("sample-gui"){
    extendsFrom(configurations.implementation.get())
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
        configurations.getByName("sample-gui").forEach {
            it.copyTo(File(distDir, it.name), true)
        }
    }
}