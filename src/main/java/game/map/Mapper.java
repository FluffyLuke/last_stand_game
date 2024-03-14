package game.map;

import game.GameManager;
import game.GameOptions;
import game.Side;
import game.units.Attack;
import game.units.Unit;
import jdk.jshell.spi.ExecutionControl;
import text.Texter;

import java.io.IOException;
import java.util.*;
import java.util.stream.Collectors;

public class Mapper {
    public enum UnitCharacter {
        RifleMan('r'),
        Monster('s'),
        HumanSpawner('h'),
        MonsterSpawner('m'),
        HumanBase('b');
        public final char symbol;
        private UnitCharacter(char label) {
            this.symbol = label;
        }
    }
    //private ArrayList<MapItem> mapItems = new ArrayList<>();
    private ArrayList<Spawner> spawners = new ArrayList<>();
    private ArrayList<Unit> units = new ArrayList<>();
    private static Mapper mapperInstance = null;
    private char[][] map_terrain = new char[25][25];
    private Base base;
    private Mapper() {
        try {
            this.map_terrain = Texter.getTexter().getMap();

        } catch (IOException e) {
            System.out.println("Cannot get a map");
            return;
        }
        for(char[] c1 : map_terrain) {
            for(char c2 : c1) {
                System.out.print(c2);
            }
            System.out.println();
        }

        placeSpawners();
        placeBase();
    }

    // Reasons why unit cannot move
    public enum CannotMove {
        CannotFindPath;
    }
    public Optional<CannotMove> moveAUnit(Unit unit, Point destination) {
        // Check if it can even go there
        map_terrain.clone();
        Pathfinder pf = new Pathfinder(getCopiedMap());
        var path = pf.findAPath(unit.getPoint(), destination);
        //var path = Optional.of(new Path());
        if(path.isEmpty()) {
            return Optional.of(CannotMove.CannotFindPath);
        }
        Point pointAfterTravel = path.get().follow(unit.getPoint(), unit.getSpeed());
        this.map_terrain[unit.getX()][unit.getY()] = '.';
        this.map_terrain[pointAfterTravel.x][pointAfterTravel.y] = unit.getUnitSymbol();

        unit.setPoint(pointAfterTravel);

        return Optional.empty();
    }

    public Optional<CannotMove> canMoveAUnit(Unit unit, Point destination) {
        Pathfinder pf = new Pathfinder(getCopiedMap());
        var path = pf.findAPath(unit.getPoint(), destination);
//        var path = Optional.of(new Path());
//        path.get().steps.add(Path.Step.UpLeft);
//        path.get().steps.add(Path.Step.UpLeft);
        if(path.isEmpty()) {
            return Optional.of(CannotMove.CannotFindPath);
        }
        return Optional.empty();
    }

    public static Mapper getMapper() {
        if(Mapper.mapperInstance != null) {
            return Mapper.mapperInstance;
        }
        Mapper.mapperInstance = new Mapper();
        return mapperInstance;
    }

    private void placeSpawners() {
        int x_index = 0;
        int y_index = 0;
        for(char[] x : map_terrain) {
            for(char y : x) {
                if(y == UnitCharacter.HumanSpawner.symbol) {
                    this.spawners.add(new Spawner(x_index, y_index, Side.Humans));
                }
                if(y == UnitCharacter.MonsterSpawner.symbol) {
                    this.spawners.add(new Spawner(x_index, y_index, Side.Monsters));
                }
                y_index++;
            }
            y_index = 0;
            x_index++;
        }
    }
    private void placeBase() {
        int x_index = 0;
        int y_index = 0;
        for(char[] x : map_terrain) {
            for(char y : x) {
                if(y == UnitCharacter.HumanBase.symbol) {
                    this.base = new Base(x_index, y_index, Side.Humans);
                    System.out.println("Base coordinates are: " + base.getPoint().x + " " + base.getPoint().y);
                    return;
                }
                y_index++;
            }
            y_index = 0;
            x_index++;
        }
        System.out.println("Could not find the base");
    }

    public boolean addUnit(Unit unit) {
        Random rand = new Random();
        List<Spawner> spawners = this.spawners
                .stream()
                .filter(spawner -> spawner.getSide() == unit.getSide())
                .toList();
        Spawner spawner = spawners.get(rand.nextInt(spawners.size()));
        //System.out.println(spawner.getPoint().x + " " + spawner.getPoint().y);
        // Find next close space near the spawner
        int x_spawner = spawner.getPoint().x;
        int y_spawner = spawner.getPoint().y;

        for(int x = 0; x < 3; x++) {
            for(int y = 0; y < 3; y++) {
                try {
                    // Search 3x3 area around the spawner
                    if(this.map_terrain[x_spawner-1+x][y_spawner-1+y] != '.') {
                        continue;
                    }
                    this.map_terrain[x_spawner-1+x][y_spawner-1+y] = unit.getUnitSymbol();
                    unit.setPoint(new Point(x_spawner-1+x, y_spawner-1+y));
                    this.units.add(unit);
                    return true;
                } catch (IndexOutOfBoundsException e) {
                    //System.out.println("index out of bounds!!!");
                    continue;
                }
            }
        }
        return false;
    }
    public List<Unit> getVisibleUnits(Unit unit) {
        // TODO make walls do something xD
        int row = unit.getX();
        int col = unit.getY();
        int sight = unit.sight;
        List<Unit> unitsInRange = new ArrayList<>();
        for(int x = 0; x < sight*2+1; x++) {
            for(int y = 0; y < sight*2+1; y++) {
                Point currentPoint = new Point(row-sight+x, col-sight+y);
                units.forEach(u -> {
                    if(u.getPoint().ifTheSame(currentPoint) && u != unit && u.getSide() != unit.getSide()) {
                        System.out.println("Found unit!");
                        unitsInRange.add(u);
                    }
                });
            }
        }
        return unitsInRange;
    }
    public void removeUnit(Unit unit) {
        if(!this.units.contains(unit)) {
            return;
        }
        this.map_terrain[unit.getX()][unit.getY()] = '.';
        this.units.remove(unit);
    }
    public void printMap() {
        for(char[] x : this.map_terrain) {
            for(char y : x) {
                System.out.print(y);
            }
            System.out.println(" ");
        }
    }
    public char[][] getCopiedMap() {
        return java.util.Arrays.stream(this.map_terrain)
                .map(char[]::clone)
                .toArray(char[][]::new);
    }
    public Point getBasePoint() {
        var point = this.base.getPoint().clone();
        point.x -= 1;
        return point;
    }
    public void attackBase(Attack attack) {
        this.base.getHit(attack);
    }
    public boolean isBaseAlive() {
        return (base.getHp() > 0);
    }
}

class Spawner implements MapItem {
    public Spawner(int x, int y, Side side) {
        this.side = side;
        this.point = new Point(x,y);
    }
    public Spawner(Point point, Side side) {
        this.side = side;
        this.point = point;
    }
    private Point point;
    private final Side side;

    @Override
    public void setPoint(Point point) {
        this.point = point;
    }

    @Override
    public Point getPoint() {
        return this.point;
    }

    public Side getSide() {
        return side;
    }
}
class Base implements MapItem {
    private int hp;
    public Base(int x, int y, Side side) {
        this.side = side;
        this.point = new Point(x,y);
        switch (GameOptions.getDifficulty()) {
            case GameOptions.Difficulty.Easy -> {this.hp = 100;}
            case GameOptions.Difficulty.Medium -> {this.hp = 50;}
            case GameOptions.Difficulty.Hard -> {this.hp = 25;}
        }
    }
    public Base(Point point, Side side) {
        this.side = side;
        this.point = point;
        switch (GameOptions.getDifficulty()) {
            case GameOptions.Difficulty.Easy -> {this.hp = 100;}
            case GameOptions.Difficulty.Medium -> {this.hp = 50;}
            case GameOptions.Difficulty.Hard -> {this.hp = 25;}
        }
    }
    private Point point;
    private final Side side;

    @Override
    public void setPoint(Point point) {
        this.point = point;
    }

    @Override
    public Point getPoint() {
        return this.point.clone();
    }

    public Side getSide() {
        return side;
    }
    public void getHit(Attack attack) {
        GameManager.getGameManager().addRadioMessage("Base is being attacked!");
        System.out.println("Base got hit! Current hp: " + this.hp);
        this.hp -= attack.damage;
        if(hp <= 0) {
            GameManager.getGameManager().endGame(false);
        }
    }

    public int getHp() {
        return hp;
    }
}
