#pragma once
#pragma warning(disable:4503) // Ignore truncated names warning 

#include <string>
#include <fstream>
#include <vector>
#include <map>

/**
* Manages loading and saving of the scene, including file writing format
* Uses a custom format inspired by JSON and config file format using the following structure:
*
*	[unique_section]					// Arbitrary number of unique sections supported
*	unique_value_label: value			// Arbitrary number of unique (within the section) value labels supported, values are loaded as strings, 
*										//		so can be any type that supports the << operator
*	unique_value_label: [				// Values can be one dimensional arrays/vectors
*		str_val (index 0)				
*		str_val2 (index 1)
*		etc...
*	]
*
*	non_unique_key_label {				// Arbitrary number of non unique keys supported which are stored as an array of keys when loaded
*		unique_value_label: value		// Keys can have values that are unique within the key
*		unique_value_label: [			// Keys support arrays as well
*			str_val (index 0)
*			str_val2 (index 1)
*			etc...
*		]
*	}
*
* Saving:
*	- Call startSave(file_name)
*	- Start section with startSection(section_name)
*	- [optional] Start key with startKey(key_name)
*	- Add value to the current key (if started) or section with addValue(label_name, value) - same for vectors
*	- End key (if started) with endKey();
*	- End section with endSection();
*
* Loading:
*	- Call load(file_name)
*	- Use the getter methods to get values/arrays from sections and keys
*
*	Note: Keys cannot contain sub-keys and arrays must only be 1 dimensional
*	
*	
*/
class SaveManager {

	// Characters used to format the file
	static struct Syntax {
		static const char SECTION_START = '[';
		static const char SECTION_END = ']';
		static const char KEY_START = '{';
		static const char KEY_END = '}';
		static const char VALUE_SEPARATOR = ':';
		static const char ARRAY_START = '[';
		static const char ARRAY_END = ']';
		static const char ARRAY_COL_SEPARATOR = '\n';
		static const char ITEM_SEPERATOR = '\n';
	};

protected:
	// Current section name
	std::string section = "";
	// Current key name
	std::string key = "";
	// Current key level, used for indentation
	int keyLevel = 0;
	// File to save/load
	std::ofstream saveFile;

	// Array of loaded sections
	std::vector<std::string> sections;
	// Array of loaded keys for each section
	std::map<std::string, std::vector<std::string>> sectionKeys;

	// Section Values in the format: sectionValues[section_name][value_label] = value;
	std::map<std::string, std::map<std::string, std::string>> sectionValues;

	// Section arrays in the format: sectionArrays[section_name[array_label] = array
	std::map<std::string, std::map<std::string, std::vector<std::string>>> sectionArrays;

	// Section Keys in the format: sectionKeyValues[section_name][key_label] = vector of ([value_label] = value);
	// Example:  sectionKeyValues[shape_manager][shape][0][name] = Pentagon
	// Gets the name stored in the first shape key entry in the shape_manager section
	std::map<std::string, std::map<std::string, std::vector<std::map<std::string, std::string>>>> sectionKeyValues;

	// Section Key arrays in the format: sectionKeyArrays[section_name][key_label] = vector of ([value_label] = array);
	// Example:  sectionKeyArrays[shape_manager][shape][0][vertices] = vertices array
	// Gets the vertices array stored in the first shape key entry in the shape_manager section
	std::map<std::string, std::map<std::string, std::vector<std::map<std::string, std::vector<std::string>>>>> sectionKeyArrays;
	
private:
	bool currentlyWriting = false;

public:
	SaveManager();

	/**
	* Opens a file for saving. stopSave must be called once you've finished writing!
	* Parameter: std::string file  File to write to
	*/
	void startSave(std::string file);

	/**
	* Closes the current file.
	*/
	void stopSave();

	/**
	* Loads settings from a previously saved file. Only supports one level of keys
	* Parameter: std::string file  File to load
	* Returns: bool  True if the file was loaded successfully
	*/
	bool load(std::string file);

	/**
	* Prints the loaded settings to stdout
	*/
	void prettyPrint();

	/**
	* Starts a new section in the save file with the provided label 
	* Parameter: std::string key  Unique label for the section without spaces
	*/
	void startSection(std::string key);
	void endSection();

	/**
	* Starts a new key in the save file with the provided label
	* Parameter: std::string key  Unique (within the current section/key) label for the key without spaces
	*/
	void startKey(std::string key);
	void endKey();

	/**
	* Writes a value under the current key/section
	* Parameter: std::string key  Unique (within the current section/key) label for the value without spaces
	* Parameter: T value  Value to write (using output operator)
	* Parameter: bool newValue  True if value key, separator and value should be written, 
	*					        otherwise value will be written on same line
	*/
	template <typename T>
	void addValue(std::string label, const T& value, bool newValue=true) {
		if (newValue) write(label + Syntax::VALUE_SEPARATOR + " ");
		write(value, false);
	}

	/**
	* Writes a 1 dimensional vector to the current key/section
	* Parameter: std::string key  Unique (within the current section/key) label for the value without spaces
	* Parameter: std::vector<T> vector  Vector to write
	*/
	template <typename T>
	void addValue(std::string label, std::vector<T>& vector) {
		write(label + Syntax::VALUE_SEPARATOR + " ");
		write(Syntax::ARRAY_START, false);
		keyLevel++;
		for (auto& item : vector) {
			write(item);
		}
		keyLevel--;
		write(Syntax::ARRAY_END);
	}

	/**
	* Returns: bool  True if the save file is opened for writing
	*/
	inline bool saveInProgress() { return currentlyWriting; }


	/**
	* Returns: std::vector<std::string>& Vector of loaded section names
	*/
	inline std::vector<std::string>& getSections() {
		return sections;
	}
	/**
	* Returns: std::vector<std::string>& Vector of loaded key names for a section
	*/
	inline std::vector<std::string>& getSectionKeys(std::string section) {
		return sectionKeys[section];
	}
	/**
	* Returns: std::map<std::string, std::string>&  Map of values in a section in the format [value_label] = value
	*/
	inline std::map<std::string, std::string>& getSectionValues(std::string section) {
		return sectionValues[section];
	}
	/**
	* Returns: std::map<std::string, std::vector<std::string>>&  Map of arrays in a section in the format [value_label] = array
	*/
	inline std::map<std::string, std::vector<std::string>>& getSectionArrays(std::string section) {
		return sectionArrays[section];
	}
	/**
	* For each target key in the section, returns a map of value mappings in the format [value_label] = value
	* Parameter: std::string key  Key to get values for
	* Returns: std::vector<std::map<std::string, std::string>>&  Vector of value mappings for all target keys in the section
	*/
	inline std::vector<std::map<std::string, std::string>>& getSectionKeyValues(std::string section, std::string key) {
		return sectionKeyValues[section][key];
	}
	/**
	* For each target key in the section, returns a map of array mappings in the format [value_label] = array
	* Parameter: std::string key  Key to get arrays for
	* Returns: std::vector<std::map<std::string, std::vector<std::string>>>&  Vector of array mappings for all target keys in the section
	*/
	inline std::vector<std::map<std::string, std::vector<std::string>>>& getSectionKeyArrays(std::string section, std::string key) {
		return sectionKeyArrays[section][key];
	}



protected:
	/**
	* Writes a new item character (Syntax::ITEM_SEPERATOR)
	*/
	void newItem();
	/**
	* Writes a value to the open file, starting a new a line and indenting appropriately
	* Parameter: T
	* Parameter: bool newEntry  True if a new item character and indentation should be written first
	*/
	template <typename T>
	void write(const T& value, bool newEntry = true) {
		if (currentlyWriting) {
			if (newEntry) {
				newItem();
				saveFile << std::string(keyLevel, '\t');
			}
			saveFile << value;
		}
	}

	// Pretty prints an array map or value map from saved settings at the passed indentation level
	void prettyPrintArrays(std::map<std::string, std::vector<std::string>>& arryMap, int indentLevel);
	void prettyPrintValues(std::map<std::string, std::string>& valMap, int indentLevel);
};

