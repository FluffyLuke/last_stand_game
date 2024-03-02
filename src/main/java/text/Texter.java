package text;

import java.io.*;
import java.util.HashMap;
import java.util.Optional;
import java.util.regex.PatternSyntaxException;

public class Texter {
    private static Texter texter;
    private Language langChosen = Language.English;
    private final ClassLoader loader = this.getClass().getClassLoader();
    private final String delimiter = "`";

    private Texter() {}

    public static Texter getTexter() {
        if(texter != null) {
            return texter;
        }
        texter = new Texter();
        return texter;
    }
    public Texter(Language lang) {
        this.langChosen = lang;
    }
    public void setLanguage(Language lang) {
        this.langChosen = lang;
    }

    public HashMap<String, String> getText(String fileName) throws IOException {
        String path;
        switch (langChosen) {
            case Language.English -> path = "text/english/" + fileName + ".txt";
            default -> path = "text/english/" + fileName + ".txt";
        }

        if(fileName.equals("title")) {
            path = "text/english/title.txt";
        }

        var stream = loader.getResourceAsStream(path);
        if(stream == null) {
            throw new FileNotFoundException();
        }

        var reader = new BufferedReader(new InputStreamReader(stream));
        HashMap<String, String> dict = new HashMap<>();
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
        reader.close();
        stream.close();
        return dict;
    }

    public Optional<String> getTitle() {
        String titleText;
        try {
            StringBuilder builder = new StringBuilder();
            HashMap<String, String> text = texter.getText("title");
            for(String line : text.values()) {
                builder.insert(0, (line+"\n"));
            }
            titleText = builder.toString();
        } catch (Exception e) {
            System.out.println("Exception caught in while getting title text: " + e);
            return Optional.empty();
        }
        return Optional.of(titleText);
    }
}

