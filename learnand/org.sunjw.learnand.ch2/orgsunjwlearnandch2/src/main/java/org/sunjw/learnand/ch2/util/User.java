package org.sunjw.learnand.ch2.util;

import java.io.Serializable;

/**
 * Created by sunjw on 16/4/20.
 */
public class User implements Serializable {
    private static final long serialVersionUID = 1L;

    public int userId;
    public String userName;
    public boolean isMale;

    public User() {

    }

    public User(int userId, String userName, boolean isMale) {
        this.userId = userId;
        this.userName = userName;
        this.isMale = isMale;
    }

    @Override
    public String toString() {
        return "{ " +
                "userId(" + userId + "), " +
                "userName(\"" + userName + "\"), " +
                "isMale(\"" + isMale + "\") " +
                "}";
    }
}
