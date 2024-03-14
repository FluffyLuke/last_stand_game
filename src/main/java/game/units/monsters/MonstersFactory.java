package game.units.monsters;

import game.Side;
import game.units.humans.RifleMan;

public class MonstersFactory {
    public static Monster getMonster() {
        return new Monster(
                100,
                100,
                5,
                "Monster",
                10,
                20,
                15,
                'e',
                3,
                2,
                Side.Monsters
        );
    }
}
