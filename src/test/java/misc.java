import org.junit.Assert;
import org.junit.Test;

public class misc {
    @Test
    public void arrayCopyTest() {
        int[][] a = new int[25][25];
        int[][] b = a.clone();
        Assert.assertNotEquals(a,b);
    }
}
