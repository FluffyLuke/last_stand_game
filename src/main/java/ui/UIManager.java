package ui;

import com.googlecode.lanterna.TextColor;
import com.googlecode.lanterna.graphics.SimpleTheme;
import com.googlecode.lanterna.graphics.ThemeStyle;
import com.googlecode.lanterna.gui2.AsynchronousTextGUIThread;
import com.googlecode.lanterna.gui2.MultiWindowTextGUI;
import com.googlecode.lanterna.gui2.SeparateTextGUIThread;
import com.googlecode.lanterna.gui2.Window;
import com.googlecode.lanterna.gui2.dialogs.MessageDialog;
import com.googlecode.lanterna.screen.Screen;
import game.GameManager;

import java.io.IOException;

public class UIManager {
    private UIComponent uiComponent;
    private final Screen screen;
    private final MultiWindowTextGUI textGUI;
    public UIManager(Screen screen) {
        this.screen = screen;
        this.textGUI = new MultiWindowTextGUI(new SeparateTextGUIThread.Factory(), screen);
        this.textGUI.setTheme(new SimpleTheme(TextColor.ANSI.WHITE, TextColor.ANSI.BLACK));
        try {
            this.screen.startScreen();
            ((AsynchronousTextGUIThread)this.textGUI.getGUIThread()).start();
        } catch (IOException e) {
            System.out.println("Exception accured in UI manager: " + e);
        }
    }

    public void renderLevel(UIComponent level) {
        // Close if there is a level running
        if(this.uiComponent != null) {
            this.uiComponent.close();
        }

        this.uiComponent = level;

        this.uiComponent.prepare(this.textGUI);
        this.uiComponent.render();
    }
    public void diplayMessage(String title, String description) {
        int speedBefore = GameManager.getGameManager().getGameSpeed();
        GameManager.getGameManager().setGameSpeed(0);
        MessageDialog.showMessageDialog(this.textGUI, title, description);
        GameManager.getGameManager().setGameSpeed(speedBefore);
    }
}
