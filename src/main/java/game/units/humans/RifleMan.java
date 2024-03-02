package game.units.humans;

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
            int initiative) {
        super(health, maxHealth, armor, unitName, threatLevel, damage, initiative);
        this.nextAbility = Optional.empty();
    }

    public enum RifleManAbility {
        ThrowGrenade
    }


    @Override
    public void move() {
        // TODO create this function
        // Map is needed to do so
    }

    @Override
    public boolean canMove() {
        // TODO create this function
        // Map is needed to do so
        return true;
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
