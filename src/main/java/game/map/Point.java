package game.map;

import static java.lang.Math.pow;
import static java.lang.Math.sqrt;

public class Point {
    public int x;
    public int y;
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public void setXY(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public static double getDistance(Point pointA, Point pointB) {
        return sqrt(pow(pointB.x-pointA.x, 2) + pow(pointB.y-pointA.y, 2));
    }
}
