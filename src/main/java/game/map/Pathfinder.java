package game.map;

import java.util.Optional;

public class Pathfinder {
    public static Optional<Path> findAPath(char[][] map, Point point_a, Point point_b) {
        var path = new Path();

        return Optional.of(path);
    }
    //TODO end this!
//    private static nextMove(char[][] map, Path path, Point point_a, Point point_b) {
//        // List of possible points
//        Point[] points = new Point[8];
//
//        for(int x = 0; x < 3; x++) {
//            for(int y = 0; y < 3; y++) {
//                try {
//                    // Search 3x3 area around the point
//                    if(map[point_a.x-1+x][point_a.y-1+y] != '.') {
//                        continue;
//                    }
//                    this.map_terrain[x_spawner-1+x][y_spawner-1+y] = unit.getUnitSymbol();
//                    unit.setCoordinates(x_spawner-1+x, y_spawner-1+y);
//                    this.units.add(unit);
//                    return true;
//                } catch (IndexOutOfBoundsException e) {
//                    continue;
//                }
//            }
//        }
//    }

}
