package game;

import com.googlecode.lanterna.screen.Screen;
import com.googlecode.lanterna.terminal.DefaultTerminalFactory;
import levels.Level;
import levels.MainLevel;
import levels.MainMenu;
import levels.OptionsMenu;
import ui.UIManager;

import javax.swing.event.ChangeEvent;
import java.io.IOException;
import java.util.ArrayList;

public class GameManager {
    private static volatile ArrayList<OptionWatcher> optionWatchers = new ArrayList<>();
    private static volatile ArrayList<GameStateWatcher> stateWatchers;
    private static volatile UIManager uiManager = getUIManager();
    private static volatile ArrayList<Level> levels = getLevels();
    private static volatile boolean gamePaused = false;
    public enum GameSpeed {
        Normal,
        Fast,
        Fastest,
    }

    private static volatile GameSpeed gameSpeed = GameSpeed.Normal;

    public static boolean playLevel(String lvlName) {
        Level level = levels.stream()
                .filter(x -> x.getLvlName().equalsIgnoreCase(lvlName))
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
        levels.add(new MainLevel("Main"));

        return levels;
    }

    public static void startGame() {
        while(true) {
            if(GameManager.gamePaused) {
                continue;
            }
            nextTick();
        }
    }

    // Here game logic is resolved
    private static void nextTick() {
        event();
        moveUnits();
        unitsFire();
        unitsSkills();
    }

    private static void event() {
        // TODO
    }

    private static void moveUnits() {
        // TODO
    }

    private static void unitsFire() {
        // TODO
    }

    private static void unitsSkills() {
        // TODO
    }

    public static void pauseGame() {
        GameManager.gamePaused = true;
    }
    public static void unpauseGame() {
        GameManager.gamePaused = false;
    }

    public static void setGameSpeed(GameSpeed speed) {
        GameManager.gameSpeed = speed;
    }

    public static void registerStateWatcher(GameStateWatcher watcher) {
        GameManager.stateWatchers.add(watcher);
    }

    public static void removeStateWatcher(GameStateWatcher watcher) {
        GameManager.stateWatchers.remove(watcher);
    }

    private void optionsChange() {
        optionWatchers.forEach((OptionWatcher::optionChange));
    }
}
