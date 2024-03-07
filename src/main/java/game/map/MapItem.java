package game.map;

import java.util.Optional;

public interface MapItem {
    public int getX();
    public int getY();
    public void setCoordinates(int x, int y);
}
