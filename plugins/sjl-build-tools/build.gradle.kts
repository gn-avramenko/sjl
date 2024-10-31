plugins {
    java
    `maven-publish`
}

java {
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(8))
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
    publications {
        create<MavenPublication>("local") {
            groupId = "com.gridnine.sjl"
            artifactId = "sjl-build-tools"
            version = "0.0.12"
            from(components["java"])
        }
    }
}