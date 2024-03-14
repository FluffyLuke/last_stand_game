package game.units;

import game.Side;
import game.map.MapItem;
import game.map.Path;
import game.map.Point;

import java.util.ArrayList;
import java.util.Optional;
import java.util.Random;

public abstract class Unit implements MapItem {
    protected Point point;
    protected char unitSymbol;
    protected int armor;
    protected int maxHealth;
    protected int health;
    protected String unitName;
    protected int threatLevel;
    protected int damage;
    protected int initiative;
    protected int speed;
    protected Side side;
    public int sight;
    protected Optional<Point> destination;
    protected Optional<Path> pathToDestination;

    public Unit(
            int health,
            int maxHealth,
            int armor, String unitName,
            int threatLevel,
            int damage,
            int iniciative,
            char unitSymbol,
            int speed,
            int sight,
            Side side) {
        this.health = health;
        this.maxHealth = maxHealth;
        this.armor = armor;
        this.unitName = unitName;
        this.threatLevel = threatLevel;
        this.damage = damage;
        this.initiative = iniciative;
        this.speed = speed;
        this.unitSymbol = unitSymbol;
        this.side = side;
        this.destination = Optional.empty();
        this.sight = sight;
    }

    public void getHit(Attack attack) {
        // Calculate how much damage it takes
        float armorCalculated = ((float)attack.armorPen/(float)this.armor) * 100;
        Random r = new Random();
        float nonPenChance = r.nextFloat(97) + 3;

        if(nonPenChance > armorCalculated) {
            System.out.println("Unit got hit, attack did not penetrate");
            return;
        }

        float critChance = r.nextInt(100) + 1;

        // Crit attack
        if(critChance < attack.critChance) {
            this.alterHealthFor(-(attack.damage*2));
            System.out.println("Unit got hit, " + this.health + " hp remains");
            return;
        }
        this.alterHealthFor(-attack.damage);
        System.out.println("Unit got hit, " + this.health + " hp remains");
    }
    public abstract void move();
    public abstract boolean canMove();
    public abstract void attack();
    public abstract boolean canAttack();

    // List all abilities for UI. Abilities are individual and cannot be created into an interface.
    public abstract ArrayList<String> listAbilities();
    // Sets next ability to use.
    public abstract boolean setNextAbility(String abilityName);
    public abstract void useAbility();
    public abstract boolean canUseAbility();


    public void setHealthTo(int health) {
        if(health > this.maxHealth) {
            this.health = maxHealth;
            return;
        }
        if(health <= 0) {
            this.health = 0;
            return;
        }
        this.health = health;
    }
    public void alterHealthFor(int health) {
        if((this.health - health) < 0) {
            return;
        }
        this.health += health;
    }
    public void setArmorTo(int armorLevel) {
        if (armorLevel < 0) {
            this.armor = 0;
            return;
        }

        this.armor = armorLevel;
    }
    public void alterArmorFor(int armorLevel) {
        if((this.armor - armorLevel) < 0) {
            this.armor = 0;
        }
        this.armor += armorLevel;
    }

    public String getUnitName() {
        return unitName;
    }

    public void setUnitName(String unitName) {
        this.unitName = unitName;
    }

    public int getThreatLevel() {
        return threatLevel;
    }

    public void setThreatLevel(int threatLevel) {
        this.threatLevel = threatLevel;
    }

    public int getDamage() {
        return damage;
    }

    public void setDamage(int damage) {
        this.damage = damage;
    }

    public int getInitiative() {
        return initiative;
    }

    public void setInitiative(int initiative) {
        this.initiative = initiative;
    }
    public int getX() {
        return this.point.x;
    }
    public int getY() {
        return this.point.y;
    }
    public void setPoint(Point point) {
        this.point = point;
    }
    public Point getPoint() {
        return this.point;
    }

    public Side getSide() {
        return side;
    }

    public char getUnitSymbol() {
        return unitSymbol;
    }

    public int getSpeed() {
        return speed;
    }
    public void setSpeed(int newSpeed) {
        if(newSpeed <= 0) {
            this.speed = 1;
        }
        this.speed = newSpeed;
    }
    public boolean isAlive() {
        return this.health > 0;
    }

    public void setDestination(Point destination) {
        this.destination = Optional.of(destination);
        this.pathToDestination = Optional.empty();
    }
}
