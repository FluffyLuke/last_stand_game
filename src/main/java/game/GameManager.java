package game;

import com.googlecode.lanterna.screen.Screen;
import com.googlecode.lanterna.terminal.DefaultTerminalFactory;
import levels.Level;
import levels.MainMenu;
import levels.OptionsMenu;
import ui.UIManager;

import javax.swing.event.ChangeEvent;
import java.io.IOException;
import java.util.ArrayList;

public class GameManager {
    static ArrayList<OptionWatcher> optionWatchers = new ArrayList<>();
    static ArrayList<Level> levels = getLevels();
    static UIManager uiManager = getUIManager();

    public static boolean playLevel(String lvlName) {
        Level level = levels.stream()
                .filter(x -> x.getLevelName().equalsIgnoreCase(lvlName))
                .findFirst()
                .orElse(null);

        if(level == null) {
            return false;
        }

        uiManager.renderLevel(level);
        return true;
    }

    private static UIManager getUIManager() {
        try {
            Screen screen = new DefaultTerminalFactory().createScreen();
            return new UIManager(screen);
        } catch (IOException e) {
            System.out.println("Cannot create UIManager: " + e);
        }
        return null;
    }
    private static ArrayList<Level> getLevels() {
        ArrayList<Level> levels = new ArrayList<>();

        levels.add(new MainMenu("Main menu"));
        levels.add(new OptionsMenu("Options"));

        return levels;
    }

    private void optionsChange() {
        optionWatchers.forEach((OptionWatcher::optionChange));
    }
}
