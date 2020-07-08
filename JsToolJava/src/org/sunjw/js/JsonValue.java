package org.sunjw.js;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map.Entry;
import java.util.Vector;

/**
 * JsonValue <br>
 * Based on jsonpp.h & jsonValue.cpp.<br>
 * <br>
 * Copyright (c) 2012-
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 * 
 * @author Sun Junwen
 * @date 2013-1-29
 * @version 0.9
 * 
 */
public class JsonValue {
	public enum VALUE_TYPE {
		STRING_VALUE, NUMBER_VALUE, BOOL_VALUE, REGULAR_VALUE, UNKNOWN_VALUE, ARRAY_VALUE, MAP_VALUE
	}

	private VALUE_TYPE mValType;
	private String mStrValue;
	private LinkedHashMap<String, JsonValue> mMapValue;
	private Vector<JsonValue> mArrayValue;

	public long line; // Line number

	/**
	 * Default constructor
	 */
	public JsonValue() {
		init();
	}

	public JsonValue(String strValue) {
		init();
		mStrValue = strValue;
	}

	/**
	 * Init me
	 */
	private void init() {
		mValType = VALUE_TYPE.STRING_VALUE;
		mStrValue = new String();
		mMapValue = new LinkedHashMap<String, JsonValue>();
		mArrayValue = new Vector<JsonValue>();
		line = -1;
	}

	/**
	 * Switch this JsonValue type.
	 * 
	 * @param newType
	 */
	private void changeType(VALUE_TYPE newType) {
		switch (newType) {
		case STRING_VALUE:
		case NUMBER_VALUE:
		case BOOL_VALUE:
		case REGULAR_VALUE:
		case UNKNOWN_VALUE:
			mArrayValue.clear();
			mMapValue.clear();
			break;
		case MAP_VALUE:
			mStrValue = new String();
			mArrayValue.clear();
			break;
		case ARRAY_VALUE:
			mStrValue = new String();
			mMapValue.clear();
			break;
		}

		mValType = newType;
	}

	/**
	 * Get a string value from this JsonValue.
	 * 
	 * @return
	 */
	public String getStrValue() {
		return mStrValue;
	}

	/**
	 * Set a string value to this JsonValue.
	 * 
	 * @param str
	 */
	public void setStrValue(String str) {
		mStrValue = str;
		if (mValType != VALUE_TYPE.STRING_VALUE
				&& mValType != VALUE_TYPE.NUMBER_VALUE
				&& mValType != VALUE_TYPE.BOOL_VALUE
				&& mValType != VALUE_TYPE.REGULAR_VALUE
				&& mValType != VALUE_TYPE.UNKNOWN_VALUE)
			changeType(VALUE_TYPE.STRING_VALUE);
	}

	/**
	 * Get an array from this JsonValue.
	 * 
	 * @return
	 */
	public Vector<JsonValue> getArrayValue() {
		return mArrayValue;
	}

	/**
	 * Set an array to this JsonValue.
	 * 
	 * @param str
	 */
	public void setArrayValue(Vector<JsonValue> jArray) {
		mArrayValue = jArray;
		changeType(VALUE_TYPE.ARRAY_VALUE);
	}

	/**
	 * Get an object from this JsonValue.
	 * 
	 * @return
	 */
	public LinkedHashMap<String, JsonValue> getMapValue() {
		return mMapValue;
	}

	/**
	 * Set an object to this JsonValue.
	 * 
	 * @param str
	 */
	public void setMapValue(LinkedHashMap<String, JsonValue> jMap) {
		mMapValue = jMap;
		changeType(VALUE_TYPE.MAP_VALUE);
	}

	/**
	 * Get data type of this JsonValue.
	 * 
	 * @return
	 */
	public VALUE_TYPE getValueType() {
		return mValType;
	}

	/**
	 * Set data type of this JsonValue.
	 * 
	 * @param valType
	 */
	public void setValueType(VALUE_TYPE valType) {
		changeType(valType);
	}

	/**
	 * Put a JsonValue into this json array.
	 * 
	 * @param value
	 */
	public void arrayPut(JsonValue value) {
		mArrayValue.add(value);
	}

	/**
	 * Put a String-JsonValue pair into this json map.
	 * 
	 * @param key
	 * @param value
	 */
	public void mapPut(String key, JsonValue value) {
		mMapValue.put(key, value);
	}

	private void extendArray(int idx) {
		while (mArrayValue.size() <= idx) {
			// need to expand
			mArrayValue.add(new JsonValue());
		}
	}

	/**
	 * Get a JsonValue on specific index from this json array.
	 * 
	 * @param idx
	 * @return
	 */
	public JsonValue get(int idx) {
		// Change to ARRAY_VALUE
		changeType(VALUE_TYPE.ARRAY_VALUE);
		extendArray(idx);

		return mArrayValue.get(idx);
	}

	/**
	 * Set a JsonValue into this json array on specific index.
	 * 
	 * @param idx
	 * @param value
	 */
	public void set(int idx, JsonValue value) {
		// Change to ARRAY_VALUE
		changeType(VALUE_TYPE.ARRAY_VALUE);
		extendArray(idx);

		mArrayValue.set(idx, value);
	}

	/**
	 * Get a JsonValue of specific key from this json map.
	 * 
	 * @param key
	 * @return
	 */
	public JsonValue get(String key) {
		// Change to MAP_VALUE
		changeType(VALUE_TYPE.MAP_VALUE);

		if (!mMapValue.containsKey(key)) {
			mMapValue.put(key, new JsonValue());
		}

		return mMapValue.get(key);
	}

	/**
	 * Put a JsonValue into this json array of specific key.
	 * 
	 * @param idx
	 * @param value
	 */
	public void put(String key, JsonValue value) {
		mMapValue.put(key, new JsonValue());
	}

	@Override
	public String toString() {
		return toString(0);
	}

	public String toString(int nRecuLevel) {
		StringBuffer ret = new StringBuffer("");

		switch (mValType) {
		case STRING_VALUE: {
			ret.append("\"");
			ret.append(mStrValue);
			ret.append("\"");
		}
			break;
		case NUMBER_VALUE:
		case BOOL_VALUE:
		case REGULAR_VALUE:
		case UNKNOWN_VALUE:
			ret.append(mStrValue);
			break;
		case MAP_VALUE: {
			++nRecuLevel;

			ret.append("{");
			ret.append("\n");

			Iterator<Entry<String, JsonValue>> itr = mMapValue.entrySet()
					.iterator();
			while (itr.hasNext()) {
				Entry<String, JsonValue> entry = itr.next();
				String key = entry.getKey();
				JsonValue value = entry.getValue();

				for (int r = 0; r < nRecuLevel; ++r)
					ret.append("\t");
				ret.append("\"");
				ret.append(key);
				ret.append("\"");
				ret.append(" : ");
				ret.append(value.toString(nRecuLevel));
				if (itr.hasNext()) {
					ret.append(",");
				}
				ret.append("\n");
			}

			for (int r = 0; r < nRecuLevel - 1; ++r)
				ret.append("\t");
			ret.append("}");
		}
			break;
		case ARRAY_VALUE: {
			ret.append("[");

			Iterator<JsonValue> itr = mArrayValue.iterator();
			while (itr.hasNext()) {
				JsonValue value = itr.next();

				ret.append(value.toString(nRecuLevel));
				if (itr.hasNext()) {
					ret.append(",");
				}
			}

			ret.append("]");
		}
			break;
		}

		return ret.toString();
	}
}
