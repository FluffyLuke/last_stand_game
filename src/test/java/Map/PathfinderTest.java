package Map;

import game.map.Pathfinder;
import game.map.Path;
import game.map.Point;
import org.junit.Assert;
import org.junit.Test;

import java.util.Optional;

public class PathfinderTest {
    @Test
    public void pathTest() {
        char[][] map = {
                {'.','.','.','.','.'},
                {'.','.','.','.','.'},
                {'.','.','W','W','.'},
                {'.','W','W','W','.'},
                {'.','W','.','.','.'},
        };

        Point startingPoint = new Point(0,2);
        Point destination = new Point(4, 2);

        Pathfinder pf = new Pathfinder(map);

        Optional<Path> path = pf.findAPath(startingPoint, destination);
        Path expectedPath = new Path();
        expectedPath.steps.add(Path.Step.Down);
        expectedPath.steps.add(Path.Step.Right);
        expectedPath.steps.add(Path.Step.DownRight);
        expectedPath.steps.add(Path.Step.Down);
        expectedPath.steps.add(Path.Step.DownLeft);
        expectedPath.steps.add(Path.Step.Left);
        Assert.assertEquals(expectedPath.steps, path.get().steps);
    }
//    @Test
//    public void gettingDirectionsTest() {
//        Point a = new Point(2, 2);
//        Point b1 = new Point(2, 4);
//        Point b2 = new Point(3, 4);
//        Point b3 = new Point(3, 2);
//        Point b4 = new Point(4, -1);
//        Point b5 = new Point(2, -2);
//        Point b6 = new Point(0, -1);
//        Point b7 = new Point(0, 2);
//        Point b8 = new Point(0, 5);
//        Assert.assertEquals(Path.Step.Up, a.getDirection(b1));
//        Assert.assertEquals(Path.Step.UpRight, a.getDirection(b2));
//        Assert.assertEquals(Path.Step.Right, a.getDirection(b3));
//        Assert.assertEquals(Path.Step.DownRight, a.getDirection(b4));
//        Assert.assertEquals(Path.Step.Down,a.getDirection(b5));
//        Assert.assertEquals(Path.Step.DownLeft,a.getDirection(b6));
//        Assert.assertEquals(Path.Step.Left, a.getDirection(b7));
//        Assert.assertEquals(Path.Step.UpLeft, a.getDirection(b8));
//    }
}
