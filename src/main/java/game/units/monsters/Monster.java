package game.units.monsters;

import game.Side;
import game.map.Mapper;
import game.units.Attack;
import game.units.Unit;
import game.units.humans.RifleMan;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Optional;

public class Monster  extends Unit {

    //private Optional<Monster.MonsterAbility> nextAbility;
    private Attack attack;

    public Monster(
            int health,
            int maxHealth,
            int armor,
            String unitName,
            int threatLevel,
            int damage,
            int initiative,
            char unitSymbol,
            int speed,
            int sight,
            Side side) {
        super(health, maxHealth, armor, unitName, threatLevel, damage, initiative, unitSymbol, speed,sight,side);
        //this.nextAbility = Optional.empty();
        this.attack = new Attack(25, 10, 50);
    }

    @Override
    public void move() {
        System.out.println("Monster " +
                this.point.x + " " + this.point.y + " " +
                "is trying to move");
        if(!canMove()) {
            System.out.println("Monster cannot move");
            return;
        }
        if(destination.isEmpty()) {
            System.out.println("Destination is empty, getting location of the base");
            this.destination = Optional.of(Mapper.getMapper().getBasePoint());
            return;
        }
        System.out.println("Monster is trying to get to: "
            + this.destination.get().x + " " + this.destination.get().y);
        if(this.destination.get().ifTheSame(this.point)) {
            System.out.println("Monster is at the base");
            this.destination = Optional.empty();
            this.pathToDestination = Optional.empty();
            return;
        }
        // TODO fix this later
        // Sometimes pathfinding is glitching, propably by recalculating
        // the path every tick
        //if(this.pathToDestination.isEmpty()) {
            //
        //}
        System.out.println("Moving a monster unit to: "+this.destination.get().x +" "+
                "row," + this.destination.get().y+"col");
        Mapper.getMapper().moveAUnit(this, this.destination.get());
    }

    @Override
    public boolean canMove() {
        if(!isAlive()) {
            return false;
        }
//        if(destination.isEmpty()) {
//            return false;
//        }
//        Optional<Mapper.CannotMove> ifCanMove = Mapper.getMapper().canMoveAUnit(this, destination.get());
//        if(ifCanMove.isEmpty()){
//            return true;
//        }
        return true;
    }

    @Override
    public void attack() {
        if(!canAttack()) {
            System.out.println("Monster " +
                    this.point.x + " " + this.point.y + " " +
                    "cannot attack");
            return;
        }
        System.out.println("Monster " +
                this.point.x + " " + this.point.y + " " +
                "can attack");

        if(this.point.ifTheSame(Mapper.getMapper().getBasePoint())) {
            System.out.println("Monster attacks base");
            Mapper.getMapper().attackBase(this.attack);
            return;
        }

        var units = Mapper.getMapper().getVisibleUnits(this);
        if(units.isEmpty()) {
            return;
        }
        units.sort(Comparator.comparingInt(Unit::getThreatLevel).reversed());
        units.getFirst().getHit(this.attack);
        for(Unit unit : units) {
            if(unit.getSide() != Side.Monsters) {
                unit.getHit(this.attack);
                return;
            }
        }
    }

    @Override
    public boolean canAttack() {
        return isAlive();
    }

    @Override
    public ArrayList<String> listAbilities() {
        ArrayList<String> listOfAbilities = new ArrayList<>();
        for(RifleMan.RifleManAbility ability : RifleMan.RifleManAbility.values()) {
            listOfAbilities.add(ability.name());
        }
        return listOfAbilities;
    }

    @Override
    public boolean setNextAbility(String abilityName) {
//        RifleMan.RifleManAbility parsedAbility;
//        try {
//            parsedAbility = RifleMan.RifleManAbility.valueOf(abilityName);
//        } catch (IllegalArgumentException e) {
//            System.out.println("Ability \"" + abilityName + "\" does not exist on rifle man");
//            return false;
//        }
//        this.nextAbility = Optional.of(parsedAbility);
            return true;
    }

    @Override
    public void useAbility() {
        if (!canUseAbility()) {
            return;
        }
        // TODO make abilities actually do something
        // Map is needed to do so
    }

    @Override
    public boolean canUseAbility() {
        // TODO create this function
        // Map is needed to do so
        return true;
    }
}