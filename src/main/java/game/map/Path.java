package game.map;

import java.util.ArrayList;
import java.util.List;

public class Path {
    public enum Step {
        Up,
        UpRight,
        Right,
        DownRight,
        Down,
        DownLeft,
        Left,
        UpLeft
    }
    public List<Step> steps = new ArrayList<>();
}
