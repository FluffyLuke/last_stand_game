package game.units.humans;

import game.GameManager;
import game.Side;
import game.map.Mapper;
import game.map.Path;
import game.units.Attack;
import game.units.Unit;

import java.util.*;

public class RifleMan extends Unit {

    private Optional<RifleManAbility> nextAbility;
    private Attack attack;

    public RifleMan(
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
        this.nextAbility = Optional.empty();
        this.attack = new Attack(damage, 10, 50);
    }

    public enum RifleManAbility {
        ThrowGrenade
    }


    @Override
    public void move() {
        if(!canMove()) {
            return;
        }
        if(destination.isEmpty()) {
            return;
        }
        if(this.destination.get().x == this.point.x && this.destination.get().y == this.point.y) {
            this.destination = Optional.empty();
            this.pathToDestination = Optional.empty();
            GameManager.getGameManager().addRadioMessage(this,"Got to position!");
            return;
        }
        // TODO fix this later
        // Sometimes pathfinding is glitching, propably by recalculating
        // the path every tick
        if(this.pathToDestination.isEmpty()) {
            //
        }
        GameManager.getGameManager().addRadioMessage(this,"Moving to position!");
        Mapper.getMapper().moveAUnit(this, this.destination.get());
    }

    @Override
    public boolean canMove() {
        if(!isAlive()) {
            return false;
        }
        if(destination.isEmpty()) {
            return false;
        }
        Optional<Mapper.CannotMove> ifCanMove = Mapper.getMapper().canMoveAUnit(this, destination.get());
        if(ifCanMove.isEmpty()){
            return true;
        }
        // TODO add radio texts depending on what is in "ifCanMove"
        return false;
    }

    @Override
    public void attack() {
        System.out.println("Soldier "+
                this.point.x + " " + this.point.y +
                " tries to attack attacks");
        if(!canAttack()) {
            System.out.println("Soldier cannot attack");
            return;
        }
        System.out.println("Soldier can attack");
        var units = Mapper.getMapper().getVisibleUnits(this);
        if(units.isEmpty()) {
            System.out.println("No units in range");
            return;
        }
        units.sort(Comparator.comparingInt(Unit::getThreatLevel).reversed());
        for(Unit unit : units) {
            if(unit.getSide() != Side.Humans) {
                System.out.println("Found target");
                GameManager.getGameManager().addRadioMessage(this,"I see an enemy!");
                GameManager.getGameManager().addRadioMessage(this,"Shooting at the enemy! Coordinates: "+unit.getX()+" "+unit.getY());
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
        for(RifleManAbility ability : RifleManAbility.values()) {
            listOfAbilities.add(ability.name());
        }
        return listOfAbilities;
    }

    @Override
    public boolean setNextAbility(String abilityName) {
        RifleManAbility parsedAbility;
        try {
            parsedAbility = RifleManAbility.valueOf(abilityName);
        } catch (IllegalArgumentException e) {
            System.out.println("Ability \"" + abilityName + "\" does not exist on rifle man");
            return false;
        }
        this.nextAbility = Optional.of(parsedAbility);
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
