#!/usr/bin/env sh

# enable debug logging
# set -x

allowMultipleInstances=ALLOW_MULTIPLE_INSTANCES_PLACEHOLDER
instanceAlreadyRunningExitCode=INSTANCE_ALREADY_RUNNING_EXIT_CODE_PLACEHOLDER
javaClassPathProvider=JAVA_CLASS_PATH_PROVIDER_PLACEHOLDER
javaVmOptions=JAVA_VM_OPTIONS_PLACEHOLDER
javaVmOptionsFileRelativePath=JAVA_VM_OPTIONS_FILE_RELATIVE_PATH_PLACEHOLDER
javaMainClass=JAVA_MAIN_CLASS_PLACEHOLDER
javaRestartExitCode=JAVA_RESTART_EXIT_CODE_PLACEHOLDER
embeddedJrePath=EMBEDDED_JRE_PATH_PLACEHOLDER

enableDebugLogging=false
if [ "$1" = "-sjl-debug" ]; then
    enableDebugLogging=true
fi
javaPath="java"
if [ -n "$embeddedJrePath" ]; then
    javaPath="$(realpath $embeddedJrePath)/bin/java"
fi
if [ -z "$instanceAlreadyRunningExitCode" ]; then
    instanceAlreadyRunningExitCode="1"
fi
if [ -f $javaVmOptionsFileRelativePath ]; then
    javaVmOptions="$javaVmOptions $(tr "\n" " " <"$javaVmOptionsFileRelativePath")"
fi

runningInstances=$(pgrep -f "$0")
if [ "$(echo "$runningInstances" | wc -l)" != "1" ] && ! $allowMultipleInstances; then
    echo "found running instance of the script, multiple instances are not allowed, exiting:"
    echo $runningInstances
    exit $instanceAlreadyRunningExitCode
fi

commandExitCode=0
javaClassPathProvider=$(ls -1 $javaClassPathProvider | paste -sd ";" -)
sjlUpdateDirectory=".sjl/update"
sjlUpdateScript="$sjlUpdateDirectory/update.script"
while true; do
    if [ -f "$sjlUpdateScript" ]; then
        $enableDebugLogging && echo "found update.script file, starting update..."
        while IFS= read -r line; do
            if echo "$line" | grep -q "^.*file-move:.*$"; then
                IFS= read -r src
                IFS= read -r dst
                $enableDebugLogging && echo "  -> copying $src to $dst"
                cp -f $src $dst
            elif echo "$line" | grep -q "^.*file-delete:.*$"; then
                IFS= read -r src
                $enableDebugLogging && echo "  -> deleting $src"
                rm -f $src
            fi
        done <$sjlUpdateScript
        $enableDebugLogging && echo "  -> deleting $sjlUpdateDirectory"
        rm -rf $sjlUpdateDirectory
        $enableDebugLogging && echo "update finished."
    fi
    $enableDebugLogging && echo "parsed java launch command: $javaPath $javaVmOptions -cp $javaClassPathProvider $javaMainClass"
    $javaPath $javaVmOptions -cp $javaClassPathProvider $javaMainClass
    commandExitCode=$?
    $enableDebugLogging && echo "command exit code: $commandExitCode"
    if [ $commandExitCode = $javaRestartExitCode ]; then
        $enableDebugLogging && echo "restart exit code detected, restarting application..."
    else
        $enableDebugLogging && echo "normal exit code detected, exiting..."
    fi
    [ $commandExitCode = $javaRestartExitCode ] || exit $commandExitCode
done
