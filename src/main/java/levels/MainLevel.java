package levels;

import com.googlecode.lanterna.TerminalSize;
import com.googlecode.lanterna.gui2.*;

import java.util.Collections;

public class MainLevel extends Level{
    private Window window;
    private Panel optionsPanel;
    private Panel mainPanel;
    private Panel radioPanel;
    private Panel mapPanel;
    public MainLevel(String lvlName) {
        super(lvlName);
    }

    @Override
    public void prepare(MultiWindowTextGUI textGUI) {
        this.textGUI = textGUI;
        this.window = new BasicWindow(lvlName);

        TerminalSize screenSize = textGUI.getScreen().getTerminalSize();
        window.setHints(Collections.singleton(Window.Hint.FULL_SCREEN));

        optionsPanel = new Panel();
        optionsPanel.setPreferredSize(new TerminalSize(3, screenSize.getRows()));

        this.mainPanel = new Panel();
        this.mainPanel.setPreferredSize(new TerminalSize(screenSize.getColumns()-3, screenSize.getRows()));

    }

    @Override
    public void render() {

    }

    @Override
    public void close() {

    }

    @Override
    public void refresh() {

    }
}
