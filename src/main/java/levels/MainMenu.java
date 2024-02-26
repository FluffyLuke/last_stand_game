package levels;

import com.googlecode.lanterna.TerminalSize;
import com.googlecode.lanterna.TextColor;
import com.googlecode.lanterna.gui2.*;
import com.googlecode.lanterna.gui2.GridLayout;
import com.googlecode.lanterna.gui2.Label;
import com.googlecode.lanterna.gui2.Panel;
import com.googlecode.lanterna.gui2.Window;
import text.Texter;

import java.util.Collections;
import java.util.HashMap;

public class MainMenu extends Level {
    private Window window;
    private final Texter texter = Texter.getTexter();
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
        gridLayout.setHorizontalSpacing(3);
        gridLayout.setVerticalSpacing(3);


        // Title text
        String titleText;
        try {
            StringBuilder builder = new StringBuilder();
            HashMap<String, String> text = texter.getText("title");
            for(String line : text.values()) {
                builder.insert(0, (line+"\n"));
            }
            titleText = builder.toString();
        } catch (Exception e) {
            System.out.println("Exception caught in while getting title text: " + e);
            this.close();
            return;
        }

        System.out.println(titleText);
        var titleLabel = new Label(titleText);
        titleLabel.setBackgroundColor(TextColor.ANSI.BLUE);

        titleLabel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER, // Horizontal alignment in the grid cell if the cell is larger than the component's preferred size
                GridLayout.Alignment.CENTER, // Vertical alignment in the grid cell if the cell is larger than the component's preferred size
                true,       // Give the component extra horizontal space if available
                false,        // Give the component extra vertical space if available
                1,                  // Horizontal span
                3));                  // Vertical span
        mainPanel.addComponent(titleLabel);


        Panel buttonPanel = new Panel(new LinearLayout(Direction.VERTICAL));
        buttonPanel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER, // Horizontal alignment in the grid cell if the cell is larger than the component's preferred size
                GridLayout.Alignment.CENTER, // Vertical alignment in the grid cell if the cell is larger than the component's preferred size
                true,       // Give the component extra horizontal space if available
                false,        // Give the component extra vertical space if available
                1,                  // Horizontal span
                3));

        HashMap<String, String> mainMenuText;
        try {
            mainMenuText = texter.getText("main_menu");
        } catch (Exception e) {
            System.out.println("Exception caught in while getting main menu text: " + e);
            this.close();
            return;
        }

        var buttonLayout = LinearLayout.createLayoutData(LinearLayout.Alignment.Center);

        Button button1 = new Button(mainMenuText.get("play_button"));
        Button button2 = new Button(mainMenuText.get("options_button"));
        Button button3 = new Button(mainMenuText.get("quit_button"), new Exit());

        button1.setLayoutData(buttonLayout);
        button2.setLayoutData(buttonLayout);
        button3.setLayoutData(buttonLayout);

        buttonPanel.addComponent(button1);
        buttonPanel.addComponent(button2);
        buttonPanel.addComponent(button3);


        mainPanel.addComponent(buttonPanel);
        window.setComponent(mainPanel);
    }
    public void render() {
        this.textGUI.addWindow(this.window);
    }
    public void close() {
        this.window.close();
    }
}

class Exit implements Runnable {
    @Override
    public void run() {
        System.exit(0);
    }
}
