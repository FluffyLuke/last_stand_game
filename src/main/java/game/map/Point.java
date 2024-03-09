package game.map;

import java.util.NoSuchElementException;

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
    public double getDistance(Point point) {
        return sqrt(pow(this.x-point.x, 2) + pow(this.y-point.y, 2));
    }
    public boolean ifTheSame(Point point) {
        return (this.x == point.x && this.y == point.y);
    }
    public Point clone() {
        return new Point(this.x, this.y);
    }

    public Path.Step getDirection(Point point) {
        //if(this.getDistance(point) != 1.0d && this.getDistance(point) != )

        // UP
        if(this.y == point.y && this.x > point.x) {
            return Path.Step.Up;
        }
        // UP-RIGHT
        if(this.y < point.y && this.x > point.x) {
            return Path.Step.UpRight;
        }
        // RIGHT
        if(this.y < point.y && this.x == point.x) {
            return Path.Step.Right;
        }
        // DOWN-RIGHT
        if(this.y < point.y && this.x < point.x) {
            return Path.Step.DownRight;
        }
        // DOWN
        if(this.y == point.y && this.x < point.x) {
            return Path.Step.Down;
        }
        // DOWN-LEFT
        if(this.y > point.y && this.x < point.x) {
            return Path.Step.DownLeft;
        }
        // LEFT
        if(this.y > point.y && this.x == point.x) {
            return Path.Step.Left;
        }
        // UP-LEFT
        if(this.y > point.y && this.x > point.x) {
            return Path.Step.UpLeft;
        }
        throw new NoSuchElementException();
    }
}
