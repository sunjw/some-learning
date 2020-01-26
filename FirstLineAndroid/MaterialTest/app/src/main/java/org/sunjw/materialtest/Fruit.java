package org.sunjw.materialtest;


import android.os.Parcel;
import android.os.Parcelable;

/**
 * å
 * Created by sunjw on 2017/9/10.
 */

public class Fruit implements Parcelable {

    private String name;
    private int imageId;

    public static final Parcelable.Creator<Fruit> CREATOR = new Parcelable.Creator<Fruit>() {
        public Fruit createFromParcel(Parcel in) {
            return new Fruit(in);
        }

        public Fruit[] newArray(int size) {
            return new Fruit[size];
        }

    };

    public Fruit(String name, int imageId) {
        this.name = name;
        this.imageId = imageId;
    }

    public Fruit(Parcel in) {
        this.name = in.readString();
        this.imageId = in.readInt();
    }

    public String getName() {
        return name;
    }

    public int getImageId() {
        return imageId;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(name);
        parcel.writeInt(imageId);
    }
}
