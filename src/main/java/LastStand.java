import com.googlecode.lanterna.screen.Screen;
import com.googlecode.lanterna.terminal.DefaultTerminalFactory;
import levels.MainMenu;
import ui.UIManager;

import java.io.IOException;

public class LastStand {
    public static void main(String[] args) {
        try {
            var screen = new DefaultTerminalFactory().createScreen();
            var ui = new UIManager(screen);
            var level = new MainMenu("Main menu");
            ui.renderLevel(level);
        } catch (IOException e) {
            System.out.println("Error while creating ui manager: " + e);
        }
    }
}
