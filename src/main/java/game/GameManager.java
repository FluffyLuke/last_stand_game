package game;

import com.googlecode.lanterna.screen.Screen;
import com.googlecode.lanterna.terminal.DefaultTerminalFactory;
import game.map.Mapper;
import game.units.Unit;
import game.units.humans.RifleMan;
import game.units.humans.RiflemanFactory;
import game.units.monsters.MonstersFactory;
import levels.Level;
import levels.MainLevel;
import levels.MainMenu;
import levels.OptionsMenu;
import ui.UIManager;

import javax.swing.event.ChangeEvent;
import java.io.IOException;
import java.util.*;
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
    private final int eventInterval;
    private int currentTick = 0;
    private List<String> radioMessages;

    private enum Event {
        MonstersSpawned,
        UnitsSpawned,
        Wipe,
        Win
    }
    private Iterator<Event> events;
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
        this.events = createEvents().iterator();
        this.eventInterval = 10;
        this.radioMessages = new ArrayList<>();
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
    private List<Event> createEvents() {
        ArrayList<Event> events = new ArrayList<>();
        switch (GameOptions.getDifficulty()) {
            case Easy -> {
                events.add(Event.UnitsSpawned);
                events.add(Event.UnitsSpawned);
                events.add(Event.UnitsSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.MonstersSpawned);
            }
            case Medium -> {
                events.add(Event.UnitsSpawned);
                events.add(Event.UnitsSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.MonstersSpawned);
            }
            case Hard -> {
                events.add(Event.UnitsSpawned);
                events.add(Event.UnitsSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.MonstersSpawned);
                events.add(Event.Wipe);
            }
        }
        Collections.shuffle(events);
        events.add(Event.Win);
        return events;
    }

    public void startGame() {
        playLevel("Main menu");
        var startUnit1 = RiflemanFactory.getRifleMan();
        var startUnit2 = RiflemanFactory.getRifleMan();
        this.units.add(startUnit1);
        mapper.addUnit(startUnit1);
        this.units.add(startUnit2);
        mapper.addUnit(startUnit2);
        while(true) {
            // For some reason code does not work without a print here??
            System.out.print("");
            if(this.gameSpeed == 0) {
                continue;
            }
            if(!mapper.isBaseAlive()) {
                this.endGame(false);
            }
            System.out.println("Running next tick!");
            currentTick++;
            try {
                TimeUnit.MILLISECONDS.sleep(1000L / this.gameSpeed);
            } catch (InterruptedException e) {
                System.out.println("Tick was interrupted!!!");
                return;
            }
            //mapper.printMap();
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
        if(this.currentTick % 10 != 0) {
            return;
        }
        Event e = events.next();
            switch (e) {
                case UnitsSpawned -> {
                    Unit unit1 = RiflemanFactory.getRifleMan();
                    Unit unit2 = RiflemanFactory.getRifleMan();
                    this.units.add(unit1); this.units.add(unit2);
                    mapper.addUnit(unit1); mapper.addUnit(unit2);
                    this.uiManager.diplayMessage("New units have spawned", "2 additional units have been spawned");
                }
                case MonstersSpawned -> {
                    Unit unit1 = MonstersFactory.getMonster();
                    Unit unit2 = MonstersFactory.getMonster();
                    Unit unit3 = MonstersFactory.getMonster();
                    this.units.add(unit1); this.units.add(unit2); this.units.add(unit3);
                    mapper.addUnit(unit1); mapper.addUnit(unit2); mapper.addUnit(unit3);
                    this.uiManager.diplayMessage("New monsters have spawned", "3 additional monsters have been spawned");
                }
                case Wipe -> {
                    this.units.forEach(u -> mapper.removeUnit(u));
                    this.units.clear();
                    Unit unit1 = RiflemanFactory.getRifleMan();
                    Unit unit2 = RiflemanFactory.getRifleMan();
                    this.units.add(unit1); this.units.add(unit2);
                    mapper.addUnit(unit1); mapper.addUnit(unit2);
                    this.uiManager.diplayMessage("Massive wipe", "Massive explosion have wiped the map clean. New reinforcements have been sent");
                }
                case Win -> {
                    endGame(true);
                }
            }
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
        int numOfUnitsBefore = this.units.size();
        this.units.forEach(x -> {
            if(!x.isAlive()) {
                System.out.println("Found dead unit: " + x.getUnitName() + " " + x.getX() + " " + x.getY());
                deadUnits.add(x);
            }
        });

        // No dead units found
        if (deadUnits.isEmpty()) {
            return;
        }

        deadUnits.forEach(x -> this.mapper.removeUnit(x));
        this.units.removeAll(deadUnits);
        System.out.println("Number of units went from "+numOfUnitsBefore+" to "+this.units.size());
    }
    public String getRadioMessages() {
        String messages = "";
        int index = 0;
        for(String m : radioMessages.reversed()) {
            if(index > 20) {
                continue;
            }
            messages += m;
            messages += "\n";
            index++;
        }
        return messages;
    }
    public void addRadioMessage(Unit unit, String message) {
        this.radioMessages.add(unit.getUnitName() + ": " +message);
    }
    public void addRadioMessage(String message) {
        this.radioMessages.add(message);
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
    public void endGame(boolean ifWin) {
        if(ifWin) {
            System.out.println("You defeated the monsters!");
        } else {
            System.out.println("Monsters have destroyed your base!");
        }
        System.exit(0);
    }

    public UIManager getUiManager() {
        return uiManager;
    }
}
