package Map;

import game.map.Mapper;
import game.map.Point;
import org.junit.Assert;
import org.junit.Test;

public class PointTest {
    @Test
    public void calculateDistanceBetweenPointsTest() {
        Point pointA = new Point(0,0);
        Point pointB = new Point(4,3);
        Assert.assertEquals(5, Point.getDistance(pointA, pointB), 0.00005);
    }
}
