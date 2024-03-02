package ui;

import com.googlecode.lanterna.gui2.MultiWindowTextGUI;
import com.googlecode.lanterna.gui2.Window;
import com.googlecode.lanterna.screen.Screen;



public interface UIComponent {
    public void prepare(MultiWindowTextGUI textGUI);
    public void render();
    public void close();

    public void refresh();
}
