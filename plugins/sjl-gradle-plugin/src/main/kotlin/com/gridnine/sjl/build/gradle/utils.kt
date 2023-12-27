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
import java.net.URL
import java.util.Arrays

fun ensureDirectoryExists(dir: File) {
    if (!dir.exists() && !dir.mkdirs()) {
        throw Exception("Unable to create directory ${dir.absolutePath}")
    }
}

private class Fake {}

fun delete(file: File) {
    if (!file.exists()) {
        return;
    }
    if (file.isDirectory && !file.deleteRecursively()) {
        throw Exception("unable to delete directory ${file.absolutePath}")
    }
    if (!file.delete()) {
        throw Exception("unable to delete file ${file.absolutePath}")
    }
}

fun copyIfDiffers(source: String, destination: File) {
    val res = Fake::class.java.classLoader.getResource(source) ?: throw Exception("unable to load resource ${source}")
    ensureDirectoryExists(destination.parentFile)
    val content = res.readBytes()
    if (destination.exists()) {
        if (destination.readBytes().contentEquals(content)) {
            return
        }
    }
    destination.writeBytes(content)
}