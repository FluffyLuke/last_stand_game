package game.units.humans;

import game.GameManager;
import game.Side;

public class RiflemanFactory {
    private static int id = 1;
    public static RifleMan getRifleMan() {
        var rifleMan = new RifleMan(
                100,
                100,
                5,
                "RifleMan"+id,
                10,
                50,
                10,
                'r',
                3,
                5,
                Side.Humans
        );
        GameManager.getGameManager().addRadioMessage(rifleMan,"New soldier is ready to fight!");
        id++;
        return rifleMan;
    }
}
