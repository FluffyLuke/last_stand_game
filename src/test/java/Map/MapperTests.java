package Map;

import game.Side;
import game.map.Mapper;
import game.units.humans.RifleMan;
import org.junit.Assert;
import org.junit.Test;

public class MapperTests {
    @Test
    public void spawnUnitTest() {
        Mapper mapper = Mapper.getMapper();
        var result = mapper.addUnit(new RifleMan(
                10,
                10,
                10,
                "Rifleman",
                10,
                10,
                10,
                'r',
                3,
                Side.Humans
        ));
        Assert.assertTrue(result);
        mapper.printMap();
    }
}
