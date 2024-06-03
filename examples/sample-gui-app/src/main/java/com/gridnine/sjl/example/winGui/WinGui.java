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

import javax.swing.*;
import java.awt.*;
import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.OpenOption;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class WinGui {
    public static void main(String[] args) throws IOException, InterruptedException {
        System.out.println("Hello world");
        //Declare frame object
        Thread.sleep(2000);
        JFrame win = new JFrame();
        win.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        //Set the title
        win.setTitle("Java Swing Example-2");
        //Set the window size
        win.setSize(400, 350);
        //Create label object
        File versionFile = new File("version.txt");
        if(!versionFile.exists()){
            Files.write(versionFile.toPath(), "0".getBytes(StandardCharsets.UTF_8), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
        }
        List<String> lines = Files.readAllLines(versionFile.toPath());
        int version =Integer.parseInt( lines.get(0));
        JLabel lbl = new JLabel("Simple Java Swing application: version " +version);
        //Set label font color
        lbl.setForeground(Color.blue);
        //Set the label position
        lbl.setBounds(100,20,250,100);
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
                File updateDir = new File(".sjl/update");
                if(updateDir.exists()){
                    deleteDirectory(updateDir);
                }
                updateDir.mkdirs();
                File updateVersionFile = new File(".sjl/update/version.txt");
                Files.write(updateVersionFile.toPath(), Arrays.asList(""+(version+1)), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
                File updateWinGuiJar = new File(".sjl/update/sample-gui-app.jar");
                File currentWinGuiJar = new File("..\\..\\..\\..\\..\\examples\\sample-gui-app\\dist\\sample-gui-app.jar");
//                File currentWinGuiJar = new File("..\\..\\win-gui\\dist\\sample-gui-app.jar");
                Files.write(updateWinGuiJar.toPath(), Files.readAllBytes(currentWinGuiJar.toPath()), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
                File splashFile = new File("..\\..\\..\\..\\..\\examples\\sample-gui-app\\src\\main\\resources\\splash.bmp");
//                File splashFile = new File("sample.bmp");
                StringBuilder sb = new StringBuilder();
                sb.append("show-splash:\n");
                sb.append(splashFile.getAbsolutePath()+"\n");
                sb.append("sleep:\n2000\nhide-splash:\n");
                sb.append("show-splash:\n");
                sb.append(splashFile.getAbsolutePath()+"\n");
                sb.append("file-move:\n");
                sb.append(updateWinGuiJar.getAbsolutePath()+"\n");
                sb.append(currentWinGuiJar.getAbsolutePath()+"\n");
                sb.append("file-move:\n");
                sb.append(updateVersionFile.getAbsolutePath()+"\n");
                sb.append(versionFile.getAbsolutePath()+"\n");
                sb.append("sleep:\n2000\nhide-splash:");
                Files.write(new File(".sjl/update/update.script").toPath(), sb.toString().getBytes(StandardCharsets.UTF_8));
            } catch (IOException ex) {
                throw new RuntimeException(ex);
            }
            System.exit(79);
        });
        btn3.setBounds(120, 200, 160, 30);
        win.add(btn3);
        JButton btn4 = new JButton("Self Update");
        btn4.addActionListener((e) -> {
            try {
                File updateDir = new File(".sjl/update");
                if(updateDir.exists()){
                    deleteDirectory(updateDir);
                }
                updateDir.mkdirs();
                File updatedLauncher = new File(".sjl/update/launcher.exe");
                File currentLauncher = new File("win-gui.exe");
//                File currentLauncher = new File("sjl.exe");
                Files.write(updatedLauncher.toPath(), Files.readAllBytes(currentLauncher.toPath()), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
                Files.write(new File(".sjl/update/self-update.script").toPath(), updatedLauncher.getAbsolutePath().getBytes(StandardCharsets.UTF_8));
//                Files.write(new File(".sjl/update/self-update.script").toPath(), String.format("%s\n%s", currentLauncher.getAbsolutePath(), updatedLauncher.getAbsolutePath()).getBytes(StandardCharsets.UTF_8));
            } catch (IOException ex) {
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

    static boolean   deleteDirectory(File directoryToBeDeleted) {
        File[] allContents = directoryToBeDeleted.listFiles();
        if (allContents != null) {
            for (File file : allContents) {
                deleteDirectory(file);
            }
        }
        return directoryToBeDeleted.delete();
    }
}
