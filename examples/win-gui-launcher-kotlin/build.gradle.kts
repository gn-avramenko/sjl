plugins {
    id("sjl-gradle-plugin") version "0.0.17"
}
sjl {
    all {
        tasksGroup = "sjl-custom"
    }
    winGui {
        common {
            general {
                allowMultipleInstances = false
                architecture = com.gridnine.sjl.build.gradle.ARCH.X64
                icon = project.file("../sample-gui-app/src/main/resources/wingui.ico")
                showSplashScreen {
                    relativePath = "..\\..\\..\\..\\sample-gui-app\\src\\main\\resources\\splash.bmp"
//                    relativePath = "..\\..\\..\\..\\examples\\sample-gui-app\\src\\main\\resources\\splash.bmp"
                }
            }
            java {
                classPathProvider = {
                    "..\\..\\..\\..\\sample-gui-app\\dist\\sample-gui-app.jar"
//                    "..\\..\\..\\..\\examples\\sample-gui-app\\dist\\sample-gui-app.jar"
                }
                vmOptions = arrayListOf("-Xms128m", "-Xmx??256m")
                mainClass = "com.gridnine.sjl.example.winGui.WinGui"
                vmOptionsFileRelativePath = "win-gui.options"
                restartExitCode = 79
                useEmbeddedJava {
                    relativePath = "..\\..\\..\\..\\sample-gui-app\\dist\\jdk"
//                    relativePath = "..\\..\\..\\..\\examples\\sample-gui-app\\dist\\jdk"
                }
            }
            version {
                fileVersion = "0.0.1"
                productVersion = "0.0.2"
            }
            manifest {
                description = "Sample application"
            }
            messages {
                standardErrorMessage = "Error occurred, consult with developers"
                errorTitle = "Error"
            }
        }
        launcher("win-gui-launcher-en-kotlin") {

        }
        launcher("win-gui-launcher-ru-kotlin") {
            messages {
                standardErrorMessage = "Произошла ошибка, обратитесь к разработчикам"
                errorTitle = "Ошибка"
            }

        }
    }
    nixShell {
        common {
            general {
                allowMultipleInstances = false
            }
            java {
                classPathProvider = {
                    "../../../../sample-gui-app/dist/sample-gui-app.jar"
                }
                vmOptions = arrayListOf("-Xms128m", "-Xmx256m")
                mainClass = "com.gridnine.sjl.example.winGui.WinGui"
                vmOptionsFileRelativePath = "win-gui.options"
                restartExitCode = 79
                useEmbeddedJava {
                    relativePath = ""
                }
            }
        }
        launcher("nix-shell-launcher-en-kotlin") {
        }
    }
}