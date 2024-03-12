package game;

import com.googlecode.lanterna.screen.Screen;
import com.googlecode.lanterna.terminal.DefaultTerminalFactory;
import game.map.Mapper;
import game.units.Unit;
import game.units.humans.RifleMan;
import levels.Level;
import levels.MainLevel;
import levels.MainMenu;
import levels.OptionsMenu;
import ui.UIManager;

import javax.swing.event.ChangeEvent;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Optional;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;

public class GameManager {
    private static volatile GameManager gMInstance;
    private  ArrayList<OptionWatcher> optionWatchers;
    private  ArrayList<GameStateWatcher> stateWatchers;
    private  volatile UIManager uiManager;
    private  ArrayList<Level> levels;
    public ArrayList<Unit> units;
    private volatile Mapper mapper;

    private  int gameSpeed = 0;

    private GameManager() {
        this.optionWatchers = new ArrayList<>();
        this.stateWatchers = new ArrayList<>();
        this.uiManager = getUIManager();
        this.levels = getLevels();
        // Game is paused at the start
        this.gameSpeed = 0;
        this.units = new ArrayList<>();
        this.mapper = Mapper.getMapper();
    }
    public static GameManager getGameManager() {
        if(gMInstance == null) {
            GameManager.gMInstance = new GameManager();
        }
        return gMInstance;
    }

    public boolean playLevel(String lvlName) {
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

    private UIManager getUIManager() {
        try {
            Screen screen = new DefaultTerminalFactory().createScreen();
            return new UIManager(screen);
        } catch (IOException e) {
            System.out.println("Cannot create UIManager: " + e);
        }
        return null;
    }
    private ArrayList<Level> getLevels() {
        ArrayList<Level> levels = new ArrayList<>();

        levels.add(new MainMenu("Main menu"));
        levels.add(new OptionsMenu("Options"));
        var mainLevel = new MainLevel("Main");
        stateWatchers.add(mainLevel);
        levels.add(mainLevel);

        return levels;
    }

    public void startGame() {
        playLevel("Main menu");
        var unit = new RifleMan(10, 10 ,10, "d", 3, 13,1,'c',5,Side.Humans);
        this.units.add(unit);
        mapper.addUnit(unit);
        while(true) {
            // For some reason code does not work without a print here??
            System.out.print("");
            if(this.gameSpeed == 0) {
                continue;
            }
            System.out.println("Running next tick!");
            try {
                TimeUnit.MILLISECONDS.sleep(1000L / this.gameSpeed);
            } catch (InterruptedException e) {
                System.out.println("Tick was interrupted!!!");
                return;
            }
            mapper.printMap();
            nextTick();
        }
    }

    // Here game logic is resolved
    private void nextTick() {
        event();
        moveUnits();
        unitsFire();
        unitsSkills();
        checkForDeadUnits();
        this.stateWatchers.forEach(GameStateWatcher::gameStateChange);
    }

    private void event() {
        // TODO
    }

    private void moveUnits() {
        this.units.forEach(Unit::move);
    }

    private void unitsFire() {
        this.units.forEach(Unit::attack);
    }

    private void unitsSkills() {
        this.units.forEach(Unit::useAbility);
    }
    private void checkForDeadUnits() {
        ArrayList<Unit> deadUnits = new ArrayList<>();
        this.units.forEach(x -> {
            if(!x.isAlive()) {
                deadUnits.add(x);
            }
        });
        deadUnits.forEach(x -> this.mapper.removeUnit(x));
        this.units.removeAll(deadUnits);
    }

    public void setGameSpeed(int speed) {
        if(speed > 3) {
            this.gameSpeed = 3;
            System.out.println("Changed game speed to 3");
            return;
        }
        if(speed < 0) {
            this.gameSpeed = 0;
            System.out.println("Changed game speed to 0");
            return;
        }
        System.out.println("Changed game speed to " + speed);
        this.gameSpeed = speed;
    }
    public Optional<Unit> getUnit(int x, int y) {
        var units = this.units.stream()
                .filter(e -> e.getX() == x && e.getY() == y)
                .toList();
        if(units.isEmpty()) {
            return Optional.empty();
        }
        return Optional.of(units.getFirst());
    }
    public int getGameSpeed() {
        return gameSpeed;
    }

    public  void registerStateWatcher(GameStateWatcher watcher) {
        this.stateWatchers.add(watcher);
    }

    public  void removeStateWatcher(GameStateWatcher watcher) {
        this.stateWatchers.remove(watcher);
    }

    private void optionsChange() {
        optionWatchers.forEach((OptionWatcher::optionChange));
    }
}
