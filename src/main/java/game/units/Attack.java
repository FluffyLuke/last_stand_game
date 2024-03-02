package game.units;

public class Attack {
    public int damage;
    public int critChance;
    public int armorPen;
    public Attack(int damage, int critChance, int armorPen) {
        this.damage = damage;
        if(critChance > 100) {
            this.critChance = 100;
        } else if(critChance < 0) {
            this.critChance = 0;
        } else {
            this.critChance = critChance;
        }
        this.armorPen = armorPen;
    }
}
