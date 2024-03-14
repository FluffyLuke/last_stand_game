package game.map;

import java.util.*;

public class Path implements Iterator<Path.Step> {
    private int currentIndex = 0;
    @Override
    public boolean hasNext() {
        return currentIndex < steps.size();
    }

    @Override
    public Step next() {
        if(!hasNext()) {
            return null;
        }
        Step step = steps.get(currentIndex);
        currentIndex++;
        return step;
    }

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
    public Point followToTheEnd(Point startingPoint) {
        var newPoint = startingPoint.clone();
        this.steps.forEach(x -> System.out.println(x.name()));
        while (this.hasNext()) {
            Step nextStep = this.next();
            switch (nextStep) {
                case Step.Up -> {newPoint.x--;}
                case Step.UpRight -> {newPoint.x--;newPoint.y++;}
                case Step.Right -> {newPoint.y++;}
                case Step.DownRight -> {newPoint.x++;newPoint.y++;}
                case Step.Down -> {newPoint.x++;}
                case Step.DownLeft -> {newPoint.x++;newPoint.y--;}
                case Step.Left -> {newPoint.y--;}
                case Step.UpLeft -> {newPoint.x--;newPoint.y--;}
            }
        }
        return newPoint;
    }
    public Point follow(Point startingPoint, int numSteps) {
        var newPoint = startingPoint.clone();
        int index = 1;
        while (this.hasNext()) {
            if(index > numSteps) {
                break;
            }
            Step nextStep = this.next();
            switch (nextStep) {
                case Step.Up -> {newPoint.x--;}
                case Step.UpRight -> {newPoint.x--;newPoint.y++;}
                case Step.Right -> {newPoint.y++;}
                case Step.DownRight -> {newPoint.x++;newPoint.y++;}
                case Step.Down -> {newPoint.x++;}
                case Step.DownLeft -> {newPoint.x++;newPoint.y--;}
                case Step.Left -> {newPoint.y--;}
                case Step.UpLeft -> {newPoint.x--;newPoint.y--;}
            }
            index++;
        }
        return newPoint;
    }
}
