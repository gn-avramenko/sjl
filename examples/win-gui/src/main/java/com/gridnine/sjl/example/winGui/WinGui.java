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

public class WinGui {
    public static void main(String[] args) {
        //Declare frame object
        JFrame win = new JFrame();
        //Set the title
        win.setTitle("Java Swing Example-2");
        //Set the window size
        win.setSize(400, 200);
        //Create label object
        JLabel lbl = new JLabel("Simple Java Swing application");
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
        //Set window position
        win.setLocationRelativeTo(null);
        //Disable default layout of the frame
        win.setLayout(null);
        //Make the window visible
        btn.addActionListener((e) -> System.exit(0));
        win.setVisible(true);
    }
}
