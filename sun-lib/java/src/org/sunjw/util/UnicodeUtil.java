package org.sunjw.util;

import me.xuender.unidecode.Unidecode;

public class UnicodeUtil {

    public static String unidecodeString(String input) {
        return Unidecode.decode(input);
    }

}
