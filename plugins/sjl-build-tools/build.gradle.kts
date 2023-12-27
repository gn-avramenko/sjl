plugins {
    java
    `maven-publish`
}

java{
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
    }
    publications {
        create<MavenPublication>("local") {
            groupId = "com.gridnine.sjl"
            artifactId = "sjl-build-tools"
            version = "0.0.1"

            from(components["java"])
        }
    }
}