import com.googlecode.lanterna.screen.Screen;
import com.googlecode.lanterna.terminal.DefaultTerminalFactory;
import game.GameManager;
import levels.MainMenu;
import ui.UIManager;

import java.io.IOException;

public class LastStand {
    public static void main(String[] args) {
        GameManager.getGameManager().startGame();
    }
}
