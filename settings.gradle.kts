include(":examples:sample-gui-app")
include(":plugins:sjl-gradle-plugin")
include(":plugins:sjl-build-tools")
include(":examples:win-gui-launcher-kotlin")
include(":examples:win-gui-launcher-groovy")
rootProject.name ="sjl"
pluginManagement {
    repositories {
        maven{
            name="local-project"
            url = uri("local-maven-repository")
        }
        gradlePluginPortal()
    }
}