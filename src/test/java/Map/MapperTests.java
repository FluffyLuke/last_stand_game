package Map;

import game.Side;
import game.map.Mapper;
import game.map.Point;
import game.units.Unit;
import game.units.humans.RifleMan;
import game.units.monsters.Monster;
import org.junit.Assert;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

public class MapperTests {
//    @Test
//    public void spawnUnitTest() {
//        Mapper mapper = Mapper.getMapper();
//        var result = mapper.addUnit(new RifleMan(
//                10,
//                10,
//                10,
//                "Rifleman",
//                10,
//                10,
//                10,
//                'r',
//                3,
//                4,
//                Side.Humans
//        ));
//        Assert.assertTrue(result);
//        mapper.printMap();
//    }
//    @Test
//    public void foundUnits() {
//        ArrayList<Unit> units = new ArrayList<>();
//        var rm = new RifleMan(10,10,10,"Rifleman",10,10,10,'r',3,1, Side.Humans);
//        rm.setPoint(new Point(1,1));
//        var mon = new Monster(10,10,10,"Rifleman",10,10,10,'r',3,1, Side.Monsters);
//        mon.setPoint(new Point(1,3));
//        units.add(rm);
//        units.add(mon);
//
//
//        int row = rm.getX();
//        int col = rm.getY();
//        int sight = rm.sight;
//        List<Unit> unitsInRange = new ArrayList<>();
//        for(int x = 0; x < sight*2+1; x++) {
//            for(int y = 0; y < sight*2+1; y++) {
//                System.out.println(x + " " + y);
//                Point currentPoint = new Point(row-sight+x, col-sight+y);
//                units.forEach(u -> {
//                    if(u.getPoint().ifTheSame(currentPoint) && u != rm && u.getSide() != rm.getSide()) {
//                        System.out.println("Found unit!");
//                        unitsInRange.add(u);
//                    }
//                });
//            }
//        }
//        Assert.assertTrue(!unitsInRange.isEmpty());
//    }
}
