package Map;

import game.map.Mapper;
import game.map.Pathfinder;
import game.map.Path;
import game.map.Point;
import org.junit.Assert;
import org.junit.Test;
import text.Texter;

import java.io.IOException;
import java.util.Optional;

public class PathfinderTest {
//    @Test
//    public void pathTest() {
//        char[][] map = {
//                {'.','.','.','.','.'},
//                {'.','.','.','.','.'},
//                {'.','.','W','W','.'},
//                {'.','W','W','W','.'},
//                {'.','W','.','.','.'},
//        };
//
//        Point startingPoint = new Point(0,2);
//        Point destination = new Point(4, 2);
//
//        Pathfinder pf = new Pathfinder(map);
//
//        Optional<Path> path = pf.findAPath(startingPoint, destination);
//        Path expectedPath = new Path();
//        expectedPath.steps.add(Path.Step.Down);
//        expectedPath.steps.add(Path.Step.Right);
//        expectedPath.steps.add(Path.Step.DownRight);
//        expectedPath.steps.add(Path.Step.Down);
//        expectedPath.steps.add(Path.Step.DownLeft);
//        expectedPath.steps.add(Path.Step.Left);
//        Assert.assertEquals(expectedPath.steps, path.get().steps);
//    }
//    @Test
//    public void pathTest2() throws IOException {
//        char[][] map = Texter.getTexter().getMap();
//
//        Point startingPoint = new Point(22,20);
//        Point destination = new Point(1, 1);
//
//        Pathfinder pf = new Pathfinder(map);
//
//        Optional<Path> path = pf.findAPath(startingPoint, destination);
//        Point exitPoint = path.get().followToTheEnd(startingPoint);
//        System.out.println("Destination: " + destination.y + ", " + destination.x);
//        System.out.println("Returned point: " + exitPoint.y + ", " + exitPoint.x);
//        Assert.assertEquals(destination, exitPoint);
//    }
//    @Test
//    public void pathTest3() throws IOException {
//        char[][] map = Texter.getTexter().getMap();
//
//        Point startingPoint = new Point(22,20);
//        Point destination = new Point(1, 1);
//        Point whereItShouldBe = new Point(20,18);
//
//        Pathfinder pf = new Pathfinder(map);
//
//        Optional<Path> path = pf.findAPath(startingPoint, destination);
//        Point exitPoint = path.get().follow(startingPoint,2);
//        System.out.println("exit point: " + exitPoint.y + ", " + exitPoint.x);
//        System.out.println("where it should be: " + whereItShouldBe.y + ", " + whereItShouldBe.x);
//        Assert.assertEquals(destination, exitPoint);
//    }
}
