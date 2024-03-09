package game.map;

import java.util.ArrayList;
import java.util.List;
import java.util.NoSuchElementException;

public class Path {
    public enum Step {
        Up,
        UpRight,
        Right,
        DownRight,
        Down,
        DownLeft,
        Left,
        UpLeft;
    }
    public List<Step> steps = new ArrayList<>();
    public Point follow(Point startingPoint) {
        var newPoint = startingPoint.clone();
        for(Step s : steps) {
            switch (s) {
                case Step.Up -> {newPoint.x++;}
                case Step.UpRight -> {newPoint.x++;newPoint.y--;}
                case Step.Right -> {newPoint.y--;}
                case Step.DownRight -> {newPoint.x--;newPoint.y--;}
                case Step.Down -> {newPoint.x--;}
                case Step.DownLeft -> {newPoint.x--;newPoint.y++;}
                case Step.Left -> {newPoint.y++;}
                case Step.UpLeft -> {newPoint.x++;startingPoint.y++;}
            }
        }
        return newPoint;
    }
    public Point follow(Point startingPoint, int numSteps) {
        var newPoint = startingPoint.clone();
        int index = 1;
        for(Step s : steps) {
            if(index > numSteps) {
                break;
            }
            switch (s) {
                case Step.Up -> {newPoint.x++;}
                case Step.UpRight -> {newPoint.x++;newPoint.y--;}
                case Step.Right -> {newPoint.y--;}
                case Step.DownRight -> {newPoint.x--;newPoint.y--;}
                case Step.Down -> {newPoint.x--;}
                case Step.DownLeft -> {newPoint.x--;newPoint.y++;}
                case Step.Left -> {newPoint.y++;}
                case Step.UpLeft -> {newPoint.x++;startingPoint.y++;}
            }
        }
        return newPoint;
    }
}
