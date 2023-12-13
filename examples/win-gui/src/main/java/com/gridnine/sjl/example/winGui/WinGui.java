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
    public static void main(String[] args) throws IOException {
        System.out.println("Hello world");
        //Declare frame object
        JFrame win = new JFrame();
        //Set the title
        win.setTitle("Java Swing Example-2");
        //Set the window size
        win.setSize(400, 500);
        //Create label object
        File versionFile = new File("version.txt");
        List<String> lines = versionFile.exists()? Files.readAllLines(versionFile.toPath()): Arrays.asList("0");
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
        btn.setBounds(150, 100, 80, 30);
        //Add button to frame
        win.add(btn);
        JButton btn2 = new JButton("Restart");
        btn2.addActionListener((e) -> System.exit(79));
        btn2.setBounds(150, 150, 80, 30);
        win.add(btn2);

        JButton btn3 = new JButton("Update");
        btn3.addActionListener((e) -> {
            try {
                if(versionFile.exists()){
                    versionFile.delete();
                }
                Files.write(versionFile.toPath(), Arrays.asList(""+(version+1)), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
                File updateDir = new File(".sjl/update");
                if(updateDir.exists()){
                    deleteDirectory(updateDir);
                }
                updateDir.mkdirs();
                Files.copy(new File("../../../examples/win-gui/dist/win-gui.jar").toPath(), new File(".sjl/update/win-gui.jar").toPath());
                Files.write(new File(".sjl/update/version.txt").toPath(), Arrays.asList(""+(version+1)), StandardOpenOption.CREATE_NEW, StandardOpenOption.WRITE);
                StringBuilder sb = new StringBuilder();
                sb.append("fmove:\n");
                sb.append(".sjl\\update\\win-gui.jar\n");
                sb.append("..\\..\\..\\examples\\win-gui\\dist\\win-gui.jar\n");
                sb.append("fmove:\n");
                sb.append(".sjl\\update\\version.txt\n");
                sb.append("version.txt");
                Files.write(new File(".sjl/update/update.script").toPath(), sb.toString().getBytes(StandardCharsets.UTF_8));
            } catch (IOException ex) {
                throw new RuntimeException(ex);
            }
            System.exit(79);
        });
        btn3.setBounds(150, 200, 80, 30);
        win.add(btn3);

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
