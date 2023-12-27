plugins{
    id("sjl-gradle-plugin") version "0.0.1"
}
sjl {
    winGui {
        common {
            allowMultipleInstances = false
            classPathProvider = {
                "win-gui.jar"
            }
            dependsOnTasks = arrayListOf()
            embeddedJvmRelativePath = "jdk"
            icon = project.file("examples/win-gui/src/main/resources/wingui.ico")
            mainClass = "com.gridnine.sjl.example.winGui.WinGui"
        }
        launcher("win-gui-launcher-en-kotlin") {

        }
        launcher("win-gui-launcher-ru-kotlin") {

        }
    }
}