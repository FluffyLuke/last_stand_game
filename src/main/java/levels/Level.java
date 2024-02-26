package levels;

import com.googlecode.lanterna.gui2.MultiWindowTextGUI;
import com.googlecode.lanterna.gui2.Window;
import com.googlecode.lanterna.screen.Screen;
import ui.UIComponent;

public abstract class Level implements UIComponent {
    protected String lvlName;
    protected MultiWindowTextGUI textGUI;

    public Level(String lvlName) {
        this.lvlName = lvlName;
    }
}
