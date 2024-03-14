package game;

import com.googlecode.lanterna.TextColor;
import com.googlecode.lanterna.graphics.SimpleTheme;
import com.googlecode.lanterna.graphics.Theme;
import com.googlecode.lanterna.graphics.ThemeDefinition;
import com.googlecode.lanterna.gui2.WindowDecorationRenderer;
import com.googlecode.lanterna.gui2.WindowPostRenderer;

public class GameOptions {

    private static Difficulty difficulty = Difficulty.Easy
            ;
    private static GameTheme gameTheme = GameTheme.Black;

    public enum GameTheme {
        Black,
        White,
    }
    public enum Difficulty {
        Easy,
        Medium,
        Hard
    }
    public static void setSettings(
            Difficulty difficulty,
            GameTheme gameTheme
    ) {
        GameOptions.difficulty = difficulty;
        GameOptions.gameTheme = gameTheme;
    }

    public static TextColor.ANSI getBackgroundColor() {
        return switch (GameOptions.gameTheme) {
            case Black -> TextColor.ANSI.BLACK;
            case White -> TextColor.ANSI.WHITE;
            default -> TextColor.ANSI.BLACK;
        };
    }

    public static TextColor.ANSI getForegroundColor() {
        return switch (GameOptions.gameTheme) {
            case Black -> TextColor.ANSI.WHITE;
            case White -> TextColor.ANSI.BLACK;
            default -> TextColor.ANSI.WHITE;
        };
    }

    public static Theme getTheme() {
        return switch (GameOptions.gameTheme) {
            case Black -> new SimpleTheme(TextColor.ANSI.WHITE, TextColor.ANSI.BLACK);
            case White -> new SimpleTheme(TextColor.ANSI.BLACK, TextColor.ANSI.WHITE);
            default -> new SimpleTheme(TextColor.ANSI.WHITE, TextColor.ANSI.BLACK);
        };
    }

    public static Difficulty getDifficulty() {
        return GameOptions.difficulty;
    }
    public static GameTheme getGameTheme() {
        return GameOptions.gameTheme;
    }
}
