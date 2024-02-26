package text;

import org.junit.Test;

import java.io.*;
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.regex.PatternSyntaxException;

public class Texter {
    Language langChoosed;
    ClassLoader loader = this.getClass().getClassLoader();;
    String delimiter = "`";
    public Texter(Language lang) {
        this.langChoosed = lang;
    }

    public HashMap<String, String> getText(String fileName) throws FileNotFoundException, IOException {
        String path;
        switch (langChoosed) {
            case Language.English -> path = "text/english/" + fileName + ".txt";
            default -> path = "text/english/" + fileName + ".txt";
        }

        var reader = new BufferedReader(new InputStreamReader(loader.getResourceAsStream(path)));
        HashMap<String, String> dict = new HashMap<String, String>();
        for(String line : reader.lines().toList()) {
            try {
                String[] kv = line.split(delimiter);
                dict.put(kv[0], kv[1]);
            } catch (PatternSyntaxException e) {
                System.out.println("Error in text file pattern detected : " + line);
            } catch (IndexOutOfBoundsException e) {
                System.out.println("Error while indexing text : " + line);
            }
        }
        return dict;
    }
}

