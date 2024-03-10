package levels;

import com.googlecode.lanterna.TerminalSize;
import com.googlecode.lanterna.TextColor;
import com.googlecode.lanterna.graphics.SimpleTheme;
import com.googlecode.lanterna.gui2.*;
import com.googlecode.lanterna.gui2.menu.Menu;
import com.googlecode.lanterna.gui2.menu.MenuBar;
import com.googlecode.lanterna.gui2.menu.MenuItem;
import com.googlecode.lanterna.gui2.table.Table;
import com.googlecode.lanterna.terminal.Terminal;
import com.googlecode.lanterna.terminal.TerminalResizeListener;
import game.GameOptions;
import game.map.Mapper;

import java.util.Collections;

public class MainLevel extends Level {
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
    public MainLevel(String lvlName) {
        super(lvlName);
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
        // REMOVE LATER
        mainPanel.setTheme(new SimpleTheme(TextColor.ANSI.WHITE, TextColor.ANSI.GREEN_BRIGHT));
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

        // REMOVE LATER
        topPanel.setTheme(new SimpleTheme(TextColor.ANSI.BLUE, TextColor.ANSI.RED));

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
        // REMOVE LATER
        optionsPanel.setTheme(new SimpleTheme(TextColor.ANSI.YELLOW, TextColor.ANSI.YELLOW));

        menuBar = new MenuBar();
        Menu optionsMenu = new Menu("Options");
        optionsMenu.add(new MenuItem("Exit", new ExitMenuItem()));
        Menu actionsMenu = new Menu("Actions");
        actionsMenu.add(new MenuItem("Move Soldiers", new MoveSoldiersMenuItem()));
        actionsMenu.add(new MenuItem("Move MG", new MoveMGsMenuItem()));
        actionsMenu.add(new MenuItem("Defend point", new DefendSoldiersMenuItem()));
        menuBar.add(optionsMenu);
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
        // REMOVE LATER
        speedPanel.setTheme(new SimpleTheme(TextColor.ANSI.YELLOW, TextColor.ANSI.GREEN));

        speedLabel = new Label("abc");
        setProperSpeedLabel();
        // speedLabel.setTheme(GameOptions.getTheme());
        // REMOVE LATER
        speedLabel.setTheme(new SimpleTheme(TextColor.ANSI.BLUE_BRIGHT, TextColor.ANSI.GREEN));
        speedPanel.addComponent(speedLabel);
        // REMOVE LATER
        speedPanel.setTheme(new SimpleTheme(TextColor.ANSI.CYAN, TextColor.ANSI.MAGENTA));

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
        bottomPanel.setTheme(new SimpleTheme(TextColor.ANSI.GREEN, TextColor.ANSI.CYAN_BRIGHT));

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

        renderMap();
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
        // REMOVE LATER!!!
        radioLabel.setTheme(GameOptions.getTheme());
        this.bottomPanel.addComponent(radioLabel);


        mainPanel.addComponent(topPanel);
        mainPanel.addComponent(bottomPanel);
        this.window.setComponent(mainPanel);
    }

    @Override
    public void render() {
        this.textGUI.addWindow(this.window);
    }

    @Override
    public void close() {
        this.window.close();
    }

    @Override
    public void refresh() {
        // TODO
        setProperSpeedLabel();
        renderTopPanel(screenSize);
        renderMap();
        renderRadio();
    }
    public void renderTopPanel(TerminalSize screenSize) {
        topPanel.setPreferredSize(new TerminalSize(screenSize.getColumns(), topPanelHeight));
        optionsPanel.setPreferredSize(new TerminalSize(
                topPanel.getSize().getColumns()/3*2,
                topPanel.getSize().getRows()
        ));
        optionsPanel.addComponent(menuBar);
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
    public void setProperSpeedLabel() {
        switch (speed) {
            case 0 -> {this.speedLabel.setText("||");}
            case 1 -> {this.speedLabel.setText(">");}
            case 2 -> {this.speedLabel.setText(">>");}
            case 3 -> {this.speedLabel.setText(">>>");}
            default -> {this.speedLabel.setText("ERR WRONG SPEED!!!");}
        }
    }
    public void renderMap() {
        char[][] map = Mapper.getMapper().getMap();
        for(char[] line : map) {
            this.mapTable.getTableModel().addRow(line[0],line[1],line[2],line[3],line[4],line[5],line[6],line[7],line[8],line[9],line[10],line[11],line[12],line[13],line[14],line[15],line[16],line[17],line[18],line[19],line[20],line[21],line[22],line[23],line[24]);
        }
    }
    public void renderRadio() {
        // TODO
    }
}

class ExitMenuItem implements Runnable {
    @Override
    public void run() {
        System.exit(0);
    }
}

class MoveSoldiersMenuItem implements Runnable {
    @Override
    public void run() {
        System.out.println("Moving soldiers...");
        // TODO
    }
}
class DefendSoldiersMenuItem implements Runnable {
    @Override
    public void run() {
        System.out.println("Defending...");
        // TODO
    }
}
class MoveMGsMenuItem implements Runnable {
    @Override
    public void run() {
        System.out.println("Moving MGs...");
        // TODO
    }
}
