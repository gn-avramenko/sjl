plugins {
    kotlin("jvm") version "1.7.10"
    id("com.gradle.plugin-publish") version "1.1.0"
}

kotlin {
    jvmToolchain {
        languageVersion.set(JavaLanguageVersion.of(8))
    }
}

repositories {
    mavenCentral()
    maven(project.file("../../local-maven-repository"))
}

buildscript {
    repositories {
        mavenCentral()
    }
    dependencies {
        classpath("org.jetbrains.kotlin:kotlin-stdlib:1.7.10")
    }
}

dependencies {
    implementation(gradleApi())
    implementation("org.jetbrains.kotlin:kotlin-stdlib:1.7.10")
    implementation("com.gridnine.sjl:sjl-build-tools:0.0.13")
    implementation("com.google.code.gson:gson:2.10.1")
}

group = "com.gridnine.sjl"
version = "0.0.13"

gradlePlugin {
    website.set("http://gridnine.com")
    vcsUrl.set("https://github.com/gn-avramenko/sjl")
    plugins {
        create("sjl") {
            id = "sjl-gradle-plugin"
            displayName = "SJL plugin"
            version = "0.0.13"
            description = "Create native wrappers for launching java applications"
            tags.set(listOf("java", "native", "wrappers"))
            implementationClass = "com.gridnine.sjl.build.gradle.SjlPlugin"
        }
    }
}

publishing {
    repositories {
        maven {
            name = "projectLocal"
            url = uri("../../local-maven-repository")
        }
        maven {
            name = "gridnineNexus"
            val gridnineNexusUrlRelease: String? by project
            val gridnineNexusUrlSnapshot: String? by project
            url = uri(gridnineNexusUrlRelease ?: "")
            isAllowInsecureProtocol = true
            credentials(PasswordCredentials::class)
        }
    }
}