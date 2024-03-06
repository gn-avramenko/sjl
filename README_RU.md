# Simple Java Launcher

ВНИМАНИЕ!!! ПРОЕКТ ЕЩЕ НЕ ГОТОВ К ИСПОЛЬЗОВАНИЮ!!!

**Table of contents**
* [О проекте](#о-проекте) 
* [Использование](#Использование)
* [JVM Options](#JVM-Options)
* [Обновление](#Обновление)
* [Примеры использования](#примеры-использования)


## О проекте
Целью проекта Simple Java Launcher (далее SJL) является разработка gradle-плагина, создающего исполняемые exe-файлы, запускающие java-приложения под Windows.
SJL можно рассматривать как аналог [launch4j](http://launch4j.sourceforge.net/), привносящий в оригинал ряд улучшений и предоставляющий gradle-плагин 
для автоматизации процесса сборки. 

Основные возможности SJL:
* Унаследованные от launch4j
    - Создает исполняемые exe-файлы, позволяющие запускать java-программу как стандартное приложение Windows
    - Для запуска Java-приложения используется JNI, поэтому не создается отельный java-процесс. Это позволяет 
закреплять на панели задач соответствующий ярлык, а также снимать задачу в Менеджере процессов (а не искать связанный java-процесс)   
    - Сборка может быть запущена под 64-битным Windows или 64-битным Linux
    - Конфигурируемый на этапе сборки classpath с поддержкой wildcard
    - Конфигурируемая иконка в exe-файле
    - Splash screen в формате BMP, отображаемый до момента полной загрузки java-приложения
    - Возможность использования Java, установленной на компьютере пользователя
    - Возможность использования Java, поставляемой вместе с приложением
    - Конфигурация манифеста exe-файла
    - Конфигурация JVM options на этапе сборки
    - Переопределение JVM options при запуске Java-приложения с помощью специального options-файла
    - Возможность перезапуска приложения при определенном коде выхода
    - Подробный вывод отладочной информации при 
    - Небольшой размер исполняемого файла (110Кб без иконки)
    - В проекте содержатся примеры использования gradle-плагина    
* Добавленные
    - Options файл позволяет не только добавлять новые параметры, но переопределять указанные на этапе сборки. 
Как пример, при создании дистрибутива программы можно указать в ней некий предполагаемый -Xmx. Но в процессе использования
можно дать возможность пользователю самому увеличить объем выделяемой памяти через графический интерфейс. В этом случае можно создать options-файл, и указать в нем новый Xmx.
Так сделано например в Intellij Idea. Более подробно описан в разделе [JVM Options](#JVM-Options).
    - Реализована процедура обновления java-приложения, java-машины (если она поставляется с приложением), а также самого исполняемого exe-файла.
Это основное улучшение по сравнению с launch4j. Более подробно алгоритм обновления описан в разделе [Обновление](#обновление)

Возможности, не перенесенные из launch4j
* Те, что я не вижу смысла реализовывать
    - Запуск в режиме консольного приложения. Перечисленные выше улучшения имеют смысл только для приложения 
с графическим интерфейсом.
    - Встраивание jar-файла в exe-файл. Графические приложения обычно состоят из нескольких файлов.
    - Открытие браузера при отсутствии установленной на машине пользователя JRE
    - Поддержка сборки в Ant
* Те, что можно добавить, если будет интерес к проекту
    - Запуск сборки под Mac OS
    - Динамический classpath с использованием переменных окружения
    - Динамический расчет initial/max heap size в зависимости от размера свободной памяти
    - Установка переменных окружения
    - Возможность смены рабочей директории
    - Подписывание исполняемого exe-файла с помощью sign4j
    - Supports Windows Security Features of the Windows 8 certification kit
    - Поддержка сборки в Maven

## Использование
Пример [build.gradle.kts](http://)
```
plugins {
    id("sjl-gradle-plugin") version "0.0.1" //(1)
}
sjl { //(2)
    winGui { //(3)
        common { //(4)
            general { //(5)
                //...
            }
            java { //(6)
                //...
            }
            version { //(7)
                //...
            } 
            manifest { //(8)
                //...
            }
            messages { //(9)
                //...
            }
        }
        launcher("win-gui-launcher-en-kotlin") { //(10)

        }
        launcher("win-gui-launcher-ru-kotlin") { //(11)
            messages { //(12)
                //...
            }
        }
    }
}
```
В результате в проекте появятся задачи
* sjl/create-win-gui-launcher-en-kotlin. Она создаст файл build/sjl/win-gui-launcher-en-kotlin.exe
* sjl/create-win-gui-launcher-ru-kotlin. Она создаст файл build/sjl/win-gui-launcher-ru-kotlin.exe
* sjl/create-all-launchers. Она выполнит две предыдущие задачи

Пояснения:
1. Подключение плагина к процессу сборки  
2. Главный конфигурационный блок плагина
3. Блок, описывающий конфигурацию для исполняемых файлов для Java-программы с графическим интерфейсом.
Этот блок сделан с прицелом на будущее, если возникнет необходимость еще и консольные программы поддержать, 
либо создавать исполняемые файлы для Linux или Mac OS. В рамках блока можно описать 
несколько exe-файлов с помощью блоков launcher("Launcher name"){} (см (10) и (11)). Можно задать
также общий блок common{} (4), в котором описаны общие для всех файлов свойства
4. Общая часть конфигурации
5. Параметры общего характера для exe-файла
   * tasksGroup. Группа задач в gradle, в которой располагаются задачи создания exe-файлов. По-молчанию: sjl.
   * dependsOnTasks. Список задач, которые должны выполнится до создания exe-файлов. По-умолчанию: список пуст.
   * sjlDirRelativePath. Относительный путь (базовый путь определяется директорией, в которой лежит исполняемый файл) служебной директории (куда например пишутся логи, и где создаются скрипты обновления). По-умолчанию: .sjl.
   * icon. Свойство типа File, в котором содержится иконка приложения. По-умолчанию: не задано. Не обязателен.
   * architecture. Задает архитектуру системы, под которую создается exe-файл (X64|WIN32). По-умолчанию: X64. 
   * allowMultipleInstances. Параметр, позволяющий запускать несколько экземпляров программы. По-умолчанию: false.
   * mutexName. Если возможен запуск только одного экземпляра, то нужно задать mutex, позволяющий это контролировать. По-умолчанию: SJL-MUTEX.
   * instanceAlreadyRunningExitCode. Если возможен запуск только одного экземпляра, то этот параметр определяет код выхода программы, при уже запущенном экземпляре. По-умолчанию: 0. 
   * showSplashScreen. Конфигурационный блок, содержащий параметр relativePath с относительным путем до файла Splash Screen в формате BMP. По-умолчанию: не задан. Не обязателен. 
6. Параметры Java-машины
   * classPathProvider. Функция, возвращающая classpath в том виде, как он будет передан Java-машине. Пути должны быть указаны относительно директории с exe-файлом. Можно использовать wildcard. По-умолчанию: не задан. Обязателен.
   * mainClass. Полное имя класса с main-функцией. По-умолчанию: не задан. Обязателен.
   * vmOptionsFileRelativePath. Относительный путь к options-файлу. По-умолчанию: не задан. Не обязателен.
   * restartExitCode. Код выхода, который предполагает перезапуск приложения. По-умолчанию: 79.
   * vmOptions. Параметры запуска Java-машины, разделенные символом |. Более подробно см раздел [JVM Options](#JVM-Options).
   * useInstalledJava. Конфигурационный блок, предписывающий использование Java-машины, установленной на компьютере пользователя. Ее расположение определяется переменной окружения JAVA_HOME. Блок содержит параметры, которым она должна соответствовать:
     - minJavaVersion. Минимальная версия java в виде натурального числа. Версия 1.8 соответствует числу 8. Минорные версии не поддерживаются. По умолчанию: не задан. Не обязателен.
     - maxJavaVersion. Максимальная версия java. По умолчанию: не задан. Не обязателен.
     - required64JRE. Флаг, предписывающий использование 64 версии java на 64-битной версии Windows. По-умолчанию: true.
   * useEmbeddedJava. Конфигурационный блок, предписывающий использование Java-машины, поставляемой вместе с приложением. Содержит один обязательный параметр: relativePath, который указывает на относительный путь к дистрибутиву JRE или JDK. По-умолчанию: не задан.
7. Описывает блок Version Info ресурсной секции exe-файла. Для ее создания используется библиотека [go-winres](https://github.com/tc-hib/go-winres/blob/main/README.md). Секция содержит следующие элементы:
   * fileVersion
   * productVersion
   * flags
   * timestamp
   * comments
   * companyName
   * fileDescription
   * internalName
   * legalCopyright
   * legalTrademarks
   * originalFilename
   * privateBuild
   * productName
   * specialBuild
8. Описывает блок Manifest ресурсной секции exe-файла. Для ее создания также используется библиотека [go-winres](https://github.com/tc-hib/go-winres/blob/main/README.md). Секция содержит следующие элементы:
   * identity. Блок с параметрами name и version
   * description. По умолчанию: пусто
   * minimum-os". По умолчанию: win7
   * execution-level. По умолчанию: as invoker
   * ui-access. По умолчанию: true
   * auto-elevate. По умолчанию: false
   * dpi-awareness. По умолчанию: system
   * disable-theming. По умолчанию: false
   * disable-window-filtering. По умолчанию:false
   * high-resolution-scrolling-aware. По умолчанию: false
   * ultra-high-resolution-scrolling-aware. По умолчанию: false
   * long-path-aware. По умолчанию: false
   * printer-driver-isolation. По умолчанию: false
   * gdi-scaling. По умолчанию: false
   * segment-heap. По умолчанию: false
   * use-common-controls-v6. По умолчанию: false
9. Описывает блок с формулировками сообщений об ошибках:
   * errorTitle. Заголовок окна с ошибкой. По-умолчанию: Error.
   * standardErrorMessage. Определяет сообщение об ошибке по умолчанию. Т.е. если не задано, например, unableToOpenFileMessage, но задано standardErrorMessage, то выведется именно standardErrorMessage. По-умолчанию: не задано. Не обязательный параметр.
   * unableToOpenFileMessage. По-умолчанию:  Unable to open file %s
   * unableToCopyDirectoryMessage. По-умолчанию: Unable to copy directory from %s to %s
   * unableToCreateDirectoryMessage. По-умолчанию: Unable to create directory %s
   * sourceFileDoesNotExistMessage. По-умолчанию: Source file %s does not exist
   * unableToCopyFileMessage. По-умолчанию: Unable to copy file from %s to %s
   * unableToDeleteDirectoryMessage. По-умолчанию: Unable to delete %s
   * unableToDeleteFileMessage. По-умолчанию: Unable to delete %s
   * unableToRenameFileMessage. По-умолчанию: Unable to rename file %s to %s
   * unableToPerformSelfUpdateMessage. По-умолчанию: Unable to perform self update
   * unableToLoadBitmapMessage. По-умолчанию: Unable to load bitmap from %s
   * unableToLocateJvmDllMessage. По-умолчанию: Unable to locate jvm.dll in %s
   * unableToLoadJvmDllMessage. По-умолчанию: Unable to load JVM from %s
   * unableToFindAddressOfJNI_CreateJavaVMMessage. По-умолчанию: Unable to find address of JNI_CreateJavaVM
   * classPathIsNotDefinedMessage. По-умолчанию: Classpath is not defined
   * unableToCreateJVMMessage. По-умолчанию: Unable to create JVM
   * unableToFindMainClassMessage. По-умолчанию: Unable to find main class %s
   * instanceAlreadyRunningMessage. По-умолчанию: Instance of the application is already running
   * unableToFindMainMethodMessage. По-умолчанию: Unable to find main method
   * errorInvokingMainMethodMessage. По-умолчанию: Exception occurred while invoking main method
   * javaHomeIsNotDefinedMessage. По-умолчанию: JAVA_HOME environment variable is not defined
   * unableToCheckInstalledJavaMessage. По-умолчанию: Unable to check installed java
   * wrongJavaTypeMessage. По-умолчанию: Wrong Java Version: required %s min version=%d max version %d, found %s version = %d
10. Создается  win-gui-launcher-en-kotlin.exe с параметрами из секции common
11. Создается  win-gui-launcher-ru-kotlin.exe с параметрами из секции common и переопределенными сообщениями об ошибках
12. Переопределяются сообщения об ошибках

## JVM Options
Задать опции запуска Java-машины можно двумя способами:
* С помощью переменной java{vmOptions=arrayListOf(...)}. Эти параметры известны на этапе сборки.
* С помощью содержимого options-файла, относительный путь к которому задается переменно java{vmOptionsFileRelativePath = "..."}. 
С помощью этого файла можно добавить новые параметры, либо переопределить стандартные из предыдущего пункта.
Рассмотрим пример, в котором на этапе сборки мы зададим параметры Xms и Xmx, а после переопределим их с помощью options-файла (с названием, например, vm.options).
В этом случае нам нужно указать параметры
```
java{
    vmOptions = arrayListOf("-Xms128m", "-Xmx??256m")
    vmOptionsFileRelativePath="vm.options"
}
```
Здесь в параметре "-Xmx??256m" использованы символы ??, которые указывают, что параметр -Xmx имеет
значение 256m по-умолчанию, но может быть переопределен в файле vm.options. Если такой файл существует
и его содержимое например следующее
```
-Xmx512m
-Dprop=10
```
то при запуске параметры Java-машины будут равны ```-Xms128m -Xmx512m -Dprop=10```

## Обновление
Обновление содержит следующие этапы
1. Само Java приложение понимает, что ему необходимо обновление
2. Java-приложение скачивает те jar файлы, которые необходимо обновить и кладет их в произвольную временную директорию. 
Если необходимо обновить и встроенную JRE, то приложение скачивает и его во временную директорию
3. Java-приложение создает файл sjl/update/update.script, в котором описаны инструкции обновления. Формат файла описан ниже.
4. При необходимости обновить исполняемы exe-файл ava-приложение создает файл sjl/update/self-update.script. Формат файла описан ниже.
5. Java-приложение завершает работу с кодом перезапуска (java{restartExitCode=..})
6. Исполняемый exe-файл проводит обновление согласно инструкциям из update.script и self-update.script
7. Исполняемы exe-файл удаляет директорию sjl/update
8. Исполняемый exe-файл файл запускает обновленное Java-приложение

Содержимое файла update.script
```
show-splash://Инструкция по отображению Splash Screen
c:\program\update\splash.bmp //Абсолютный путь к файлу изображения в формате BMP
file-move://Инструкция по перемещению файла
c:\program\update\app.jar //Абсолютный путь к файлу, который нужно переместить
c:\program\lib\app.jar //Абсолютный путь к файлу, который нужно заменить
file-delete://Инструкция по удалению файла
c:\program\lib\app.jar //Абсолютный путь к файлу, который нужно удалить
sleep://Инструкция по приостановке выполнения
1000//Количество миллисекунд, на которые нужно приостановить выполнение
hide-splash://Инструкция по скрытию Spash Screen
```
Содержимое файла self-update.script
```
c:\program\update\new-executable.exe //Абсолютный путь к обновленному exe-файлу
```
При выполнении инструкции ```file-move:``` исходный файл не сразу удаляется (удаление происходит в пункте 7).
Нужно это для того, чтобы при возникновении временного сбоя при обновлении программу можно было перезапустить
и она бы завершила процесс обновления

## Примеры использования
В директории examples/sample-gui-app можно посмотреть пример java-приложения, задействующего функции обновления.

В директории examples/win-gui-launcher-kotlin можно посмотреть скрипт build.gradle.kts, создающего пару исполняемых exe-файлов.

В директории examples/win-gui-launcher-groovy можно посмотреть скрипт build.gradle, создающего пару исполняемых exe-файлов.

Для целей тестирования необходимо:
1. Выполнить задачу dist/dist. 
2. Выполнить задачу sjl/create-all-launchers
3. Попробовать запустить полученные исполняемые файлы из директорий
   examples/win-gui-launcher-kotlin/build/.sjl/launchers и examples/win-gui-launcher-groovy/build/.sjl/launchers 
## Версия
См [VERSION.md](VERSION.md) со списком версий.