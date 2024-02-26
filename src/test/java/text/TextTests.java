package text;

import org.junit.Test;
import org.junit.Assert;

import java.util.HashMap;

public class TextTests {
    @Test
    public void getTextTest() {
        var texter = new Texter(Language.English);

        HashMap<String, String> text;
        try {
            text = texter.getText("test_text");
            for(String v : text.values()) {
                System.out.println(v);
            }
            Assert.assertEquals("this is test text nr 1", text.get("test1"));
        } catch (Exception e) {
            System.out.println(e);
        }
    }
}
