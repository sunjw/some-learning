package org.sunjw.networktest;

import android.util.Log;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Created by sunjw on 2017/7/10.
 */

public class ContentHandler extends DefaultHandler {

    private static final String TAG = ContentHandler.class.getSimpleName();

    private String nodeName;
    private StringBuilder id;
    private StringBuilder name;
    private StringBuilder version;

    @Override
    public void startDocument() throws SAXException {
        id = new StringBuilder();
        name = new StringBuilder();
        version = new StringBuilder();
    }

    @Override
    public void endDocument() throws SAXException {
        super.endDocument();
    }

    @Override
    public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
        nodeName = localName;
    }

    @Override
    public void endElement(String uri, String localName, String qName) throws SAXException {
        if ("app".equals(localName)) {
            Log.d(TAG, "id is " + id.toString().trim());
            Log.d(TAG, "name is " + name.toString().trim());
            Log.d(TAG, "version is " + version.toString().trim());
            id.setLength(0);
            name.setLength(0);
            version.setLength(0);
        }
    }

    @Override
    public void characters(char[] ch, int start, int length) throws SAXException {
        StringBuilder sbTemp = null;
        if ("id".equals(nodeName)) {
            sbTemp = id;
        } else if ("name".equals(nodeName)) {
            sbTemp = name;
        } else if ("version".equals(nodeName)) {
            sbTemp = version;
        }

        if (sbTemp != null) {
            sbTemp.append(ch, start, length);
        }
    }
}
