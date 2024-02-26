package levels;

import com.googlecode.lanterna.TerminalSize;
import com.googlecode.lanterna.TextColor;
import com.googlecode.lanterna.graphics.Theme;
import com.googlecode.lanterna.graphics.ThemeStyle;
import com.googlecode.lanterna.gui2.*;
import com.googlecode.lanterna.gui2.GridLayout;
import com.googlecode.lanterna.gui2.Label;
import com.googlecode.lanterna.gui2.Panel;
import com.googlecode.lanterna.gui2.Window;
import com.googlecode.lanterna.screen.Screen;

import java.awt.*;
import java.util.Collections;

public class MainMenu extends Level {
    private Window window;
    public MainMenu(String lvlName) {
        super(lvlName);
    }
    public void prepare(MultiWindowTextGUI textGUI) {
        this.textGUI = textGUI;
        this.window = new BasicWindow(lvlName);

        TerminalSize screenSize = textGUI.getScreen().getTerminalSize();

        window.setHints(Collections.singleton(Window.Hint.FULL_SCREEN));

        var mainPanel = new Panel(new GridLayout(1));
        mainPanel.setPreferredSize(screenSize);

        GridLayout gridLayout = (GridLayout)mainPanel.getLayoutManager();
        gridLayout.setHorizontalSpacing(10);
        gridLayout.setVerticalSpacing(10);

        var titleLabel = new Label("LAST STAND");
        titleLabel.setBackgroundColor(TextColor.ANSI.BLUE);


        titleLabel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER, // Horizontal alignment in the grid cell if the cell is larger than the component's preferred size
                GridLayout.Alignment.CENTER, // Vertical alignment in the grid cell if the cell is larger than the component's preferred size
                true,       // Give the component extra horizontal space if available
                false,        // Give the component extra vertical space if available
                5,                  // Horizontal span
                3));                  // Vertical span
        mainPanel.addComponent(titleLabel);


        window.setComponent(mainPanel);
    }
    public void render() {
        this.textGUI.addWindow(this.window);
    }
    public void close() {
        this.window.close();
    }
}
