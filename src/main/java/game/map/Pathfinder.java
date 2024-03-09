package game.map;

import java.util.*;

public class Pathfinder {
    private char[][] world_map;
    public Pathfinder(char[][] map) {
        this.world_map = map;
    }
    public Optional<Path> findAPath(Point point_a, Point point_b) {
        // Since we don't want to overwrite the map we create a clone of it
        var map_copy = world_map.clone();

        var path = new Path();

        var result = nextMove(map_copy, path, point_a.clone(), point_b.clone());

        if(result.isEmpty()) {
            return Optional.empty();
        }



        return Optional.of(path);
    }
    private Optional<Path> nextMove(char[][] map, Path path, Point currentLocation, Point destination) {
        //If this is destination, return
        if(currentLocation.ifTheSame(destination)) {
            return Optional.of(path);
        }

        // Mark current location as checked for future recursions
        map[currentLocation.x][currentLocation.y] = 'W';

        // Get all possible directions
        List<Point> possibleDirections = new ArrayList<>();
        for(int x = 0; x < 3; x++) {
            for(int y = 0; y < 3; y++) {
                try {
                    // Search 3x3 area around the point
                    if(map[currentLocation.x-1+x][currentLocation.y-1+y] != '.') {
                        continue;
                    }
                    var possibleDirection = new Point(currentLocation.x-1+x, currentLocation.y-1+y);
                    possibleDirections.add(possibleDirection);
                } catch (IndexOutOfBoundsException e) {
                    continue;
                }
            }
        }

        // if no more directions available - return empty
        if (possibleDirections.isEmpty()) {
            path.steps.removeLast();
            return Optional.empty();
        }

        // Sort them by distance to destination
        possibleDirections.sort(Comparator.comparingDouble(p -> Point.getDistance(p, destination)));

        // Check every one of them for possible path to destination.
        for(Point p : possibleDirections) {
            // List appending in loop, since if direction fails
            // it should be recursively removed
            path.steps.add(currentLocation.getDirection(p));
            var result = nextMove(map, path, p, destination);
            if (result.isEmpty()) {
                continue;
            }
            // If found a path - return it
            return result;
        }
        // If did not find the path - return empty
        path.steps.removeLast();
        return Optional.empty();
    }
}
