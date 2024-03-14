package levels;

import com.googlecode.lanterna.TerminalSize;
import com.googlecode.lanterna.TextColor;
import com.googlecode.lanterna.graphics.SimpleTheme;
import com.googlecode.lanterna.gui2.*;
import com.googlecode.lanterna.gui2.dialogs.MessageDialog;
import com.googlecode.lanterna.gui2.dialogs.TextInputDialog;
import com.googlecode.lanterna.gui2.dialogs.TextInputDialogBuilder;
import com.googlecode.lanterna.gui2.menu.Menu;
import com.googlecode.lanterna.gui2.menu.MenuBar;
import com.googlecode.lanterna.gui2.menu.MenuItem;
import com.googlecode.lanterna.gui2.table.Table;
import com.googlecode.lanterna.terminal.Terminal;
import com.googlecode.lanterna.terminal.TerminalResizeListener;
import com.sun.tools.javac.Main;
import game.GameManager;
import game.GameOptions;
import game.GameStateWatcher;
import game.map.Mapper;
import game.map.Point;
import game.units.Unit;
import ui.UIManager;

import java.text.ParseException;
import java.util.Collections;
import java.util.Optional;
import java.util.regex.Pattern;

public class MainLevel extends Level implements GameStateWatcher {
    private Window window;
    private TerminalSize screenSize;
    private Panel topPanel;
    private Panel optionsPanel;
    private MenuBar menuBar;
    private Panel speedPanel;
    private int speed;
    private Label speedLabel;
    private Panel mainPanel;
    private Panel bottomPanel;
    private Table<Character> mapTable;
    private Label radioLabel;
    private final int topPanelHeight = 1;
    private final int levelHeight =35;
    private final int levelWidth = 120;
    //private final GameManager gameManager;
    private Mapper mapper;
    public MainLevel(String lvlName) {
        super(lvlName);
        this.mapper = Mapper.getMapper();
    }

    // TODO Take text from outside file instead of hard coding it
    @Override
    public void prepare(MultiWindowTextGUI textGUI) {
        this.textGUI = textGUI;
        this.window = new BasicWindow(lvlName);
        this.setSpeed(0);

        this.screenSize = textGUI.getScreen().getTerminalSize();
        window.setHints(Collections.singleton(Window.Hint.FULL_SCREEN));

        this.mainPanel = new Panel();
        mainPanel.setLayoutManager(new GridLayout(1));

        mainPanel.setTheme(GameOptions.getTheme());
        mainPanel.setSize(new TerminalSize(levelWidth,levelHeight));

        //  --- Setting top panel and its child panels ---
        topPanel = new Panel();
        topPanel.setTheme(GameOptions.getTheme());
        topPanel.setPreferredSize(new TerminalSize(levelWidth, 1));
        topPanel.setLayoutManager(new GridLayout(2));
        topPanel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER,
                GridLayout.Alignment.BEGINNING,
                true,
                true,
                1,1
        ));


        topPanel.setTheme(GameOptions.getTheme());

        optionsPanel = new Panel();
        optionsPanel.setTheme(GameOptions.getTheme());
        optionsPanel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.BEGINNING,
                GridLayout.Alignment.CENTER,
                true,
                true,
                1,1
        ));
        optionsPanel.setLayoutManager(new LinearLayout(Direction.HORIZONTAL));
        optionsPanel.setPreferredSize(new TerminalSize(levelWidth, 1));
        optionsPanel.setTheme(GameOptions.getTheme());

        menuBar = new MenuBar();
        Menu optionsMenu = new Menu("Options");
        optionsMenu.add(new MenuItem("Exit", new ExitMenuItem()));
        Menu speedOptions = new Menu("Game Speed");
        speedOptions.add(new MenuItem("Change game speed", new Runnable() {
            @Override
            public void run() {
                String result = new TextInputDialogBuilder()
                        .setTitle("Get game speed")
                        .setDescription("Enter speed of the game (0-3)")
                        .setValidationPattern(Pattern.compile("[0-9]+", Pattern.CASE_INSENSITIVE), "Not a game speed!")
                        .build()
                        .showDialog(textGUI);
                int parsedResult = Integer.parseInt(result);
                GameManager.getGameManager().setGameSpeed(parsedResult);
            }
        }));
        Menu actionsMenu = new Menu("Actions");

        // -- MENU ACTIONS ---
        actionsMenu.add(new MenuItem("Move Soldiers", new Runnable() {
            @Override
            public void run() {
                int lastSpeed = GameManager.getGameManager().getGameSpeed();
                GameManager.getGameManager().units.forEach(x -> System.out.println("Unit[ column: " + x.getY() + ", row: "+x.getX()+" ]"));
                String column = new TextInputDialogBuilder()
                        .setTitle("Get column")
                        .setDescription("Enter a column number of a unit")
                        .setValidationPattern(Pattern.compile("[0-9]+", Pattern.CASE_INSENSITIVE), "Not a column!")
                        .build()
                        .showDialog(textGUI);
                String row = new TextInputDialogBuilder()
                        .setTitle("Get row")
                        .setDescription("Enter a row number of a unit")
                        .setValidationPattern(Pattern.compile("[0-9]+", Pattern.CASE_INSENSITIVE), "Not a row!")
                        .build()
                        .showDialog(textGUI);
                int parsedCol;
                int parsedRow;
                parsedCol = Integer.parseInt(column)-1;
                parsedRow = Integer.parseInt(row)-1;
                Optional<Unit> unit = GameManager.getGameManager().getUnit(parsedRow, parsedCol);

                if(unit.isEmpty()) {
                    MessageDialog.showMessageDialog(textGUI, "No unit found", "Did not found specified unit!");
                    return;
                }
                column = new TextInputDialogBuilder()
                        .setTitle("Get column")
                        .setDescription("Enter a column number where unit should go")
                        .setValidationPattern(Pattern.compile("[0-9]+", Pattern.CASE_INSENSITIVE), "Not a column!")
                        .build()
                        .showDialog(textGUI);
                row = new TextInputDialogBuilder()
                        .setTitle("Get row")
                        .setDescription("Enter a row number where unit should go")
                        .setValidationPattern(Pattern.compile("[0-9]+", Pattern.CASE_INSENSITIVE), "Not a row!")
                        .build()
                        .showDialog(textGUI);
                parsedCol = Integer.parseInt(column) - 1;
                parsedRow = Integer.parseInt(row) - 1;
                unit.get().setDestination(new Point(parsedRow, parsedCol));
            }
        }));
        // -------------------

        menuBar.add(optionsMenu);
        menuBar.add(speedOptions);
        menuBar.add(actionsMenu);
        menuBar.setTheme(GameOptions.getTheme());

        optionsPanel.addComponent(menuBar);

        speedPanel = new Panel();
        speedPanel.setTheme(GameOptions.getTheme());
        speedPanel.setLayoutManager(new LinearLayout(Direction.HORIZONTAL));
        speedPanel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.END,
                GridLayout.Alignment.CENTER,
                true,
                true,
                1,1
        ));
        speedPanel.setPreferredSize(new TerminalSize(
                20, 1
            ));
        speedPanel.setTheme(GameOptions.getTheme());

        speedLabel = new Label("abc");
        setProperSpeedLabel();
        // speedLabel.setTheme(GameOptions.getTheme());
        speedLabel.setTheme(GameOptions.getTheme());
        speedPanel.addComponent(speedLabel);
        speedPanel.setTheme(GameOptions.getTheme());

        topPanel.addComponent(optionsPanel);
        topPanel.addComponent(speedPanel);

        // --- Setting main panel -> map and radio

        this.bottomPanel = new Panel();
        this.bottomPanel.setLayoutManager(new GridLayout(2));
        this.bottomPanel.setPreferredSize(new TerminalSize(levelWidth, 35));
        this.bottomPanel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER,
                GridLayout.Alignment.BEGINNING,
                true,
                true,
                1,1
        ));
        bottomPanel.setTheme(GameOptions.getTheme());

        this.mapTable = new Table<>("1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25");
        this.mapTable.setTheme(GameOptions.getTheme());
        this.mapTable.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER,
                GridLayout.Alignment.BEGINNING,
                false,
                false,
                1,1
        ));
        this.mapTable.setPreferredSize(new TerminalSize(70,70));
        for(char[] line : mapper.getCopiedMap()) {
            this.mapTable.getTableModel().addRow(line[0],line[1],line[2],line[3],line[4],line[5],line[6],line[7],line[8],line[9],line[10],line[11],line[12],line[13],line[14],line[15],line[16],line[17],line[18],line[19],line[20],line[21],line[22],line[23],line[24]);
        }
        bottomPanel.addComponent(mapTable);

        this.radioLabel = new Label("Test text");
        renderRadio();
        this.radioLabel.setSize(new TerminalSize(50, 70));
        this.radioLabel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER,
                GridLayout.Alignment.BEGINNING,
                true,
                true,
                1,1
        ));
        radioLabel.setTheme(GameOptions.getTheme());
        this.bottomPanel.addComponent(radioLabel);


        mainPanel.addComponent(topPanel);
        mainPanel.addComponent(bottomPanel);
        this.window.setComponent(mainPanel);
    }

    @Override
    public void render() {
        this.textGUI.addWindow(this.window);
        GameManager.getGameManager().getUiManager().diplayMessage("Welcome commander",
                "Commander - we have been ambushed by a swarm of monster-like creatures.\n" +
                        "Destroy them before they get to our base!\n" +
                        "\n" +
                        "Game difficulty: " + GameOptions.getDifficulty());
    }

    @Override
    public void close() {
        this.window.close();
    }

    @Override
    public void refresh() {
        // TODO
        setProperSpeedLabel();
        renderMap();
        renderRadio();
    }
    public void setProperSpeedLabel() {
        this.speed = GameManager.getGameManager().getGameSpeed();
        switch (speed) {
            case 0 -> {this.speedLabel.setText("||");}
            case 1 -> {this.speedLabel.setText(">");}
            case 2 -> {this.speedLabel.setText(">>");}
            case 3 -> {this.speedLabel.setText(">>>");}
            default -> {this.speedLabel.setText("ERR WRONG SPEED!!!");}
        }
    }
    public void setSpeed(int speed) {
        if(speed > 3) {
            this.speed = 3;
            System.out.println("Speed tried to be set too high: "+speed);
        }
        if(speed < 0) {
            this.speed = 0;
            System.out.println("Speed tried to be set too low: "+speed);
        }
        this.speed = speed;
    }

    public int getSpeed() {
        return speed;
    }
    public void renderMap() {
        char[][] newMap = Mapper.getMapper().getCopiedMap();
        for(int x = 0; x < 25; x++) {
            for(int y = 0; y < 25; y++) {
                this.mapTable.getTableModel().setCell(y, x, newMap[x][y]);
            }
        }
    }
    public void renderRadio() {
        this.radioLabel.setText(GameManager.getGameManager().getRadioMessages());
    }

    @Override
    public void gameStateChange() {
        this.refresh();
    }
}


class ExitMenuItem implements Runnable {
    @Override
    public void run() {
        System.exit(0);
    }
}
