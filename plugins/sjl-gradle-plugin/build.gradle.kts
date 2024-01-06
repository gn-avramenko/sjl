
plugins {
    kotlin("jvm")  version "1.9.10"
    id("com.gradle.plugin-publish") version "1.1.0"
}

kotlin{
    jvmToolchain(8)
}

repositories{
    mavenCentral()
    maven(project.file("../../local-maven-repository"))
}

buildscript{
    repositories{
        mavenCentral()
    }
    dependencies{
        classpath("org.jetbrains.kotlin:kotlin-stdlib:1.9.10")
    }
}

dependencies{
    implementation(gradleApi())
    implementation("org.jetbrains.kotlin:kotlin-stdlib:1.9.21")
    implementation("com.gridnine.sjl:sjl-build-tools:0.0.1")
}

group = "com.gridnine.sjl"
version = "0.0.1"

gradlePlugin {
    website.set("http://gridnine.com")
    vcsUrl.set("https://github.com/gn-avramenko/sjl")
    plugins {
        create("sjl") {
            id = "sjl-gradle-plugin"
            displayName = "SJL plugin"
            version = "0.0.1"
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
    }
}