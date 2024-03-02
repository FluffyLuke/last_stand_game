package game.units;

import java.util.ArrayList;
import java.util.Random;

public abstract class Unit {
    protected int armor;
    protected int maxHealth;
    protected int health;
    protected String unitName;
    protected int threatLevel;
    protected int damage;
    protected int initiative;

    public Unit(int health, int maxHealth, int armor, String unitName, int threatLevel, int damage, int iniciative) {
        this.health = health;
        this.maxHealth = maxHealth;
        this.armor = armor;
        this.unitName = unitName;
        this.threatLevel = threatLevel;
        this.damage = damage;
        this.initiative = iniciative;
    }

    public void getHit(Attack attack) {
        // Calculate how much damage it takes
        float armorCalculated = ((float)attack.armorPen/(float)this.armor) * 100;
        Random r = new Random();
        float nonPenChance = r.nextFloat(97) + 3;

        if(nonPenChance > armorCalculated) {
            return;
        }

        float critChance = r.nextInt(100) + 1;

        // Crit attack
        if(critChance < attack.critChance) {
            this.alterHealthFor(-(attack.damage*2));
            return;
        }
        this.alterHealthFor(-attack.damage);
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
            this.die();
            return;
        }
        this.health = health;
    }
    public void alterHealthFor(int health) {
        if((this.health - health) < 0) {
            die();
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

    public abstract void die();

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
}
