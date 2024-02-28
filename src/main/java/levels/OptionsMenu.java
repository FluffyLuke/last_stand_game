package levels;

import com.googlecode.lanterna.TerminalSize;
import com.googlecode.lanterna.TextColor;
import com.googlecode.lanterna.gui2.*;
import com.googlecode.lanterna.gui2.GridLayout;
import com.googlecode.lanterna.gui2.Label;
import com.googlecode.lanterna.gui2.Panel;
import com.googlecode.lanterna.gui2.Window;
import game.GameManager;
import game.GameOptions;
import text.Texter;

import java.util.Collections;
import java.util.HashMap;
import java.util.Optional;

import static game.GameOptions.Difficulty.Easy;
import static game.GameOptions.Difficulty.Medium;

public class OptionsMenu extends Level{
    private Window window;

    public OptionsMenu(String lvlName) {
        super(lvlName);
    }

    private GameOptions.Difficulty difficulty = Medium;
    private GameOptions.GameTheme theme = GameOptions.GameTheme.Black;

    private Label difficultyLabel;
    private Label themeLabel;

    @Override
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

        Optional<String> titleOption = Texter.getTexter().getTitle();

        if(titleOption.isEmpty()) {
            System.out.println("Cannot find title text");
            this.close();
            return;
        }

        String titleText = titleOption.get();

        var titleLabel = new Label(titleText);
        titleLabel.setForegroundColor(GameOptions.getForegroundColor());
        titleLabel.setBackgroundColor(GameOptions.getBackgroundColor());
        titleLabel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER, // Horizontal alignment in the grid cell if the cell is larger than the component's preferred size
                GridLayout.Alignment.CENTER, // Vertical alignment in the grid cell if the cell is larger than the component's preferred size
                true,       // Give the component extra horizontal space if available
                false,        // Give the component extra vertical space if available
                1,                  // Horizontal span
                1));                  // Vertical span
        mainPanel.addComponent(titleLabel);

        HashMap<String, String> optionsMenuText;
        try {
            optionsMenuText = Texter.getTexter().getText("options_menu");
        } catch (Exception e) {
            System.out.println("Exception caught while getting options menu text: " + e);
            this.close();
            return;
        }

        Panel buttonPanel = new Panel(new GridLayout(2));

        buttonPanel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER, // Horizontal alignment in the grid cell if the cell is larger than the component's preferred size
                GridLayout.Alignment.CENTER, // Vertical alignment in the grid cell if the cell is larger than the component's preferred size
                true,       // Give the component extra horizontal space if available
                false,        // Give the component extra vertical space if available
                1,                  // Horizontal span
                1));                  // Vertical span
        mainPanel.addComponent(titleLabel);

        GridLayout buttonLayout = (GridLayout)mainPanel.getLayoutManager();
        buttonLayout.setHorizontalSpacing(3);
        buttonLayout.setVerticalSpacing(3);

        Button difficultyButton = new Button(optionsMenuText.get("difficulty_button"), new ChangeDifficulty(this));
        Button themeButton = new Button(optionsMenuText.get("theme_button"), new ChangeTheme(this));
        Button saveButton = new Button(optionsMenuText.get("save_button"), new SaveAndQuit(this));

        difficultyButton.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.BEGINNING, // Horizontal alignment in the grid cell if the cell is larger than the component's preferred size
                GridLayout.Alignment.CENTER, // Vertical alignment in the grid cell if the cell is larger than the component's preferred size
                true,       // Give the component extra horizontal space if available
                false,        // Give the component extra vertical space if available
                1,                  // Horizontal span
                1));                  // Vertical span


        themeButton.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.BEGINNING, // Horizontal alignment in the grid cell if the cell is larger than the component's preferred size
                GridLayout.Alignment.CENTER, // Vertical alignment in the grid cell if the cell is larger than the component's preferred size
                true,       // Give the component extra horizontal space if available
                false,        // Give the component extra vertical space if available
                1,                  // Horizontal span
                1));                  // Vertical span


        saveButton.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER, // Horizontal alignment in the grid cell if the cell is larger than the component's preferred size
                GridLayout.Alignment.CENTER, // Vertical alignment in the grid cell if the cell is larger than the component's preferred size
                true,       // Give the component extra horizontal space if available
                false,        // Give the component extra vertical space if available
                2,                  // Horizontal span
                1));                  // Vertical span


        difficultyButton.setTheme(GameOptions.getTheme());
        themeButton.setTheme(GameOptions.getTheme());
        saveButton.setTheme(GameOptions.getTheme());

        this.difficultyLabel = new Label(GameOptions.getDifficulty().name());
        this.themeLabel = new Label(GameOptions.getGameTheme().name());

        this.difficultyLabel.setTheme(GameOptions.getTheme());
        this.themeLabel.setTheme(GameOptions.getTheme());

        difficultyLabel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER,
                GridLayout.Alignment.CENTER,
                true,
                false,
                1,
                1
        ));

        titleLabel.setLayoutData(GridLayout.createLayoutData(
                GridLayout.Alignment.CENTER,
                GridLayout.Alignment.CENTER,
                true,
                false,
                1,
                1
        ));

        buttonPanel.addComponent(difficultyButton);
        buttonPanel.addComponent(difficultyLabel);
        buttonPanel.addComponent(themeButton);
        buttonPanel.addComponent(themeLabel);
        buttonPanel.addComponent(saveButton);


        mainPanel.addComponent(buttonPanel);
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

    public void changeDifficulty() {
        this.difficulty = switch (this.difficulty) {
            case Easy -> Medium;
            case Medium -> GameOptions.Difficulty.Hard;
            case Hard -> Easy;
        };
        this.difficultyLabel.setText(this.difficulty.name());
    }

    public void changeTheme() {
        this.theme = switch (this.theme) {
            case Black -> GameOptions.GameTheme.White;
            case White -> GameOptions.GameTheme.Black;
        };
        this.themeLabel.setText(this.theme.name());
    }

    public GameOptions.Difficulty getDifficulty() {
        return this.difficulty;
    }

    public GameOptions.GameTheme getTheme() {
        return this.theme;
    }
}

class ChangeDifficulty implements Runnable {
    private OptionsMenu menu;
    ChangeDifficulty(OptionsMenu menu) {
        this.menu = menu;
    }

    @Override
    public void run() {
        menu.changeDifficulty();
    }
}

class ChangeTheme implements Runnable {
    private OptionsMenu menu;
    ChangeTheme(OptionsMenu menu) {
        this.menu = menu;
    }

    @Override
    public void run() {
        menu.changeTheme();
    }
}

class SaveAndQuit implements Runnable {
    private OptionsMenu menu;
    SaveAndQuit(OptionsMenu menu) {
        this.menu = menu;
    }
    @Override
    public void run() {
        GameOptions.setSettings(
                menu.getDifficulty(),
                menu.getTheme()
        );
        GameManager.playLevel("Main menu");
    }
}


