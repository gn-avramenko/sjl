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

package com.gridnine.sjl.example.winGui;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.Collections;
import java.util.List;
import java.util.stream.Stream;

public class WinGui {

    private final static Logger logger = LoggerFactory.getLogger(WinGui.class);

    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println("Hello world");
        //Declare frame object
        Thread.sleep(2000);
        JFrame win = new JFrame();
        win.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        //Set the title
        win.setTitle("Java Swing Example-2");
        //Set the window size
        win.setSize(400, 350);
        //Create label object
        Path versionFile = Paths.get("version.txt");
        if (!Files.exists(versionFile)) {
            Files.write(versionFile, "0".getBytes(StandardCharsets.UTF_8), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
        }
        List<String> lines = Files.readAllLines(versionFile);
        int version = Integer.parseInt(lines.get(0));
        JLabel lbl = new JLabel("Simple Java Swing application: version " + version);
        //Set label font color
        lbl.setForeground(Color.blue);
        //Set the label position
        lbl.setBounds(100, 20, 250, 100);
        //Add label to frame
        win.add(lbl);
        //Create a button
        JButton btn = new JButton("Close");
        //Set the button position
        btn.setBounds(120, 100, 160, 30);
        //Add button to frame
        win.add(btn);
        JButton btn2 = new JButton("Restart");
        btn2.addActionListener((e) -> System.exit(79));
        btn2.setBounds(120, 150, 160, 30);
        win.add(btn2);

        JButton btn3 = new JButton("Update");
        btn3.addActionListener((e) -> {
            try {
                Path updateDir = Paths.get(".sjl", "update");
                if (Files.exists(updateDir)) {
                    deleteRecursive(updateDir);
                }
                Files.createDirectories(updateDir);
                Path updateVersionFile = Paths.get(".sjl", "update", "version.txt");
                Files.write(updateVersionFile, Collections.singletonList("" + (version + 1)), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
                Path updateWinGuiJar = Paths.get(".sjl", "update", "sample-gui-app.jar");
                Path currentWinGuiJar = Paths.get("..",  "..", "..", "..", "examples", "sample-gui-app", "dist", "sample-gui-app.jar");
                Files.write(updateWinGuiJar, Files.readAllBytes(currentWinGuiJar), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);

                StringBuilder sb = new StringBuilder();
                sb.append("sleep:\n2000\nhide-splash:\n");
                sb.append("file-move:\n");
                sb.append(updateWinGuiJar.toAbsolutePath()).append("\n");
                sb.append(currentWinGuiJar.toAbsolutePath()).append("\n");
                sb.append("file-move:\n");
                sb.append(updateVersionFile.toAbsolutePath()).append("\n");
                sb.append(versionFile.toAbsolutePath()).append("\n");
                sb.append("sleep:\n2000\nhide-splash:");
                Files.write(Paths.get(".sjl", "update", "update.script"), sb.toString().getBytes(StandardCharsets.UTF_8));
            } catch (Throwable ex) {
                logger.error("unable to update", ex);
                throw new RuntimeException(ex);
            }
            System.exit(79);
        });
        btn3.setBounds(120, 200, 160, 30);
        win.add(btn3);
        JButton btn4 = new JButton("Self Update");
        btn4.addActionListener((e) -> {
            try {
                Path updateDir = Paths.get(".sjl", "update");
                if (Files.exists(updateDir)) {
                    deleteRecursive(updateDir);
                }
                Files.createDirectories(updateDir);
                Path updatedLauncher = Paths.get(".sjl", "update", "launcher.exe");
                Path currentLauncher = Paths.get("win-gui.exe");
//                Path currentLauncher = Paths.get("sjl.exe");
                Files.write(updatedLauncher, Files.readAllBytes(currentLauncher), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
                Files.write(Paths.get(".sjl", "update", "self-update.script"), updatedLauncher.toAbsolutePath().toString().getBytes(StandardCharsets.UTF_8));
//                Files.write(Paths.get(".sjl", "update", "self-update.script"), String.format("%s\n%s", currentLauncher.toAbsolutePath().toString(), updatedLauncher.toAbsolutePath().toString()).getBytes(StandardCharsets.UTF_8));
            } catch (Throwable ex) {
                logger.error("unable to self update", ex);
                throw new RuntimeException(ex);
            }
            System.exit(79);
        });
        btn4.setBounds(120, 250, 160, 30);
        win.add(btn4);
        //Set window position
        win.setLocationRelativeTo(null);
        //Disable default layout of the frame
        win.setLayout(null);
        //Make the window visible
        btn.addActionListener((e) -> System.exit(0));

        win.setVisible(true);
    }

    @SuppressWarnings("UnusedReturnValue")
    static boolean deleteRecursive(Path directoryToBeDeleted) {

        try (Stream<Path> fstream = Files.isDirectory(directoryToBeDeleted)
                ? Files.list(directoryToBeDeleted) : Stream.empty()) {
            fstream.forEach(WinGui::deleteRecursive);
            return Files.deleteIfExists(directoryToBeDeleted);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
