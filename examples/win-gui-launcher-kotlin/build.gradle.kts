plugins {
    id("sjl-gradle-plugin") version "0.0.21"
}
sjl {
    winGui {
        common {
            general {
                allowMultipleInstances = true // for opening .xdsk files in agent
                sjlDebug = true
                architecture = com.gridnine.sjl.build.gradle.ARCH.X64
                icon = project.file("../sample-gui-app/src/main/resources/projectavatar.ico")
                showSplashScreen {
                    relativePath = ""
                }
            }
            java {
                classPathProvider = {
                    "lib\\*"
                }
                vmOptions = arrayListOf(
                    "-Xms128m", "-Xmx??256m",
                    "--module-path=jre\\fx\\lib", "--add-modules=javafx.controls",
                    "-Dlogback.configurationFile=config\\logback.xml", "-Dsun.jnu.encoding=UTF-8", "-Dfile.encoding=UTF-8"
                )
                mainClass = "com.gridnine.xdisk.agent.app.XdiskAgentApplication"
                restartExitCode = 3
                useEmbeddedJava {
                    relativePath = "jre"
                }
            }
            version {
                fileVersion = "1.0"
                productVersion = "1.0"
            }
            manifest {
                description = "Xdisk Agent"
            }
            messages {
                standardErrorMessage = "Error occurred, consult with developers"
                errorTitle = "Error"
            }
        }
        launcher("win-gui-launcher") {
        }
    }
}