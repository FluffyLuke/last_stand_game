package game.units.humans;

import game.Side;
import game.map.Mapper;
import game.units.Unit;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Optional;

public class RifleMan extends Unit {

    private Optional<RifleManAbility> nextAbility;

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
            Side side) {
        super(health, maxHealth, armor, unitName, threatLevel, damage, initiative, unitSymbol, speed, side);
        this.nextAbility = Optional.empty();
    }

    public enum RifleManAbility {
        ThrowGrenade
    }


    @Override
    public void move() {
        if(destination.isEmpty()) {
            return;
        }
        if(!canMove()) {
            return;
        }
        Mapper.getMapper().moveAUnit(this, this.destination.get());
    }

    @Override
    public boolean canMove() {
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
        // TODO create this function
        // Map is needed to do so
    }

    @Override
    public boolean canAttack() {
        // TODO create this function
        // Map is needed to do so
        return true;
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

    @Override
    public void die() {
        // TODO create this function
        // Map is needed to do so
    }
}
