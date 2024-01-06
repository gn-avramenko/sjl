/*
 * MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package com.gridnine.sjl.build.gradle

import java.io.File


fun ensureDirectoryExists(dir: File) {
    if (!dir.exists() && !dir.mkdirs()) {
        throw Exception("Unable to create directory ${dir.absolutePath}")
    }
}

private class Fake {}

fun delete(file: File) {
    if (!file.exists()) {
        return
    }
    if (file.isDirectory && !file.deleteRecursively()) {
        throw Exception("unable to delete directory ${file.absolutePath}")
    }
    if (!file.delete()) {
        throw Exception("unable to delete file ${file.absolutePath}")
    }
}

fun copyIfDiffers(source: String, destination: File, setExecutable: Boolean = false) {
    val res = Fake::class.java.classLoader.getResource(source) ?: throw Exception("unable to load resource ${source}")
    ensureDirectoryExists(destination.parentFile)
    val content = res.readBytes()
    if (destination.exists()) {
        if (destination.readBytes().contentEquals(content)) {
            return
        }
    }
    destination.writeBytes(content)
    if(setExecutable){
        destination.setExecutable(true)
    }
}

fun copyIfDiffers(source: File, destination: File) {
    if(destination.exists() && destination.readBytes().contentEquals(source.readBytes())){
        return
    }
    ensureDirectoryExists(destination.parentFile)
    destination.writeBytes(source.readBytes())
}

fun updateIfDiffers(file: File, content: String) {
    ensureDirectoryExists(file.parentFile)
    if(file.exists() && content == file.readText(Charsets.UTF_8)){
        return
    }
    file.writeText(content, Charsets.UTF_8)
}

fun getEscapedWindowsAbsolutePath(file:File):String{
    return file.absolutePath.replace(File.pathSeparator, "\\\\")
}

fun getEscapedWindowsAbsolutePath(pathLike:String):String{
    return pathLike.replace("\\","\\\\").replace("/", "\\\\")
}
