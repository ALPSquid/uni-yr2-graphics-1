#include "stdafx.h"
#include "SaveManager.h"
#include "Utils.h"
#include <iostream>

using std::string;			using std::cout;
using std::ifstream;		using std::endl;
using std::vector;
using std::map;


SaveManager::SaveManager() {}

void SaveManager::startSave(string file) {
	if (!saveFile.is_open() && !currentlyWriting) {
		saveFile.open(file);
		currentlyWriting = true;
	} else {
		throw std::exception("Saving already in progress, call stopSave() first!");
	}
}

void SaveManager::stopSave() {
	saveFile.close();
	currentlyWriting = false;
}

/************************************************************************/
/* LOADING                                                              */
/************************************************************************/

bool SaveManager::load(string file) {
	// Clear any saved settings
	sectionValues.clear();
	sectionArrays.clear();
	sectionKeyValues.clear();
	sectionKeyArrays.clear();

	bool success = false;
	ifstream is(file);
	if (is) {
		string buffer;

		string section = "";
		string key = "";
		int keyIndex = 0; // Occurrence of the current key
		bool isArray = false; // Whether an array is currently being built
		string arrayName;
		vector<string> stringParts; // Split string cache
		while (getline(is, buffer)) {
			Utils::removeFromString(buffer, '\t');
			stringParts.clear();
			if (buffer.find(Syntax::VALUE_SEPARATOR) != string::npos || isArray) {
				if (!isArray) {
					// Split value at separator to get label and value
					Utils::splitString(buffer, Syntax::VALUE_SEPARATOR, stringParts);
					// Removing leading space from value
					if (stringParts[1][0] == ' ') stringParts[1] = stringParts[1].substr(1);
				}
				// Create array if saved value is array
				if (buffer.find(Syntax::ARRAY_START) != string::npos) {
					isArray = true;
					arrayName = stringParts[0];
					continue; // Skip extra processing on this iteration
				}
				if (isArray) {
					// If end of array
					if (buffer.find(Syntax::ARRAY_END) != string::npos) {
						isArray = false;
						continue; // Skip extra processing on this iteration
					}
					// Add array to appropriate arrays map
					if (key == "") sectionArrays[section][arrayName].push_back(buffer);
					else sectionKeyArrays[section][key][keyIndex][arrayName].push_back(buffer);
				} else if (key == "") {
					// Add value to current section identified by the label
					sectionValues[section][stringParts[0]] = stringParts[1];
				} else {
					// Add values to current section -> key
					sectionKeyValues[section][key][keyIndex][stringParts[0]] = stringParts[1];
				}
			} else if (buffer.find(Syntax::KEY_START) != string::npos) {
				// Starting a new key, set current key name to it and get the next index
				Utils::removeFromString(buffer, Syntax::KEY_START);
				key = buffer;
				int newKeyIndex = sectionKeyValues[section][key].size();
				if (newKeyIndex == 0) sectionKeys[section].push_back(key);
				if (keyIndex != newKeyIndex || newKeyIndex == 0) {
					// Create new value and array map for the new key
					map<string, string> newMap;
					map<string, vector<string>> newArrayMap;
					sectionKeyValues[section][key].push_back(newMap);
					sectionKeyArrays[section][key].push_back(newArrayMap);
					keyIndex = newKeyIndex;
				}
			} else if (buffer.find(Syntax::KEY_END) != string::npos) {
				// Ended current key, clear current key name
				key = "";
				keyIndex = 0;
			} else if (buffer[0] == Syntax::SECTION_START) {
				// New section started
				Utils::removeFromString(buffer, Syntax::SECTION_START);
				Utils::removeFromString(buffer, Syntax::SECTION_END);
				section = buffer;
				sections.push_back(section);
				// Create maps for this section
				map<string, string> newMap;
				map<string, vector<string>> newArrayMap;
				sectionValues[section] = newMap;
				sectionArrays[section] = newArrayMap;
				map<string, vector<map<string, string>>> newKeyMap;
				map<string, vector<map<string, vector<string>>>> newKeyArrayMap;
				sectionKeyValues[section] = newKeyMap;
				sectionKeyArrays[section] = newKeyArrayMap;
			}
		}
		success = true;
	}
	is.close();
	return success;
}

void SaveManager::prettyPrint() {
	int indentLevel = 0;
	for (const auto& section : sections) {
		cout << Syntax::SECTION_START << section << Syntax::SECTION_END << endl;
		prettyPrintValues(sectionValues[section], indentLevel);
		prettyPrintArrays(sectionArrays[section], indentLevel);
		for (const auto& key : sectionKeys[section]) {
			// Print each key in a section
			for (unsigned i = 0; i < sectionKeyValues[section][key].size(); i++) {
				cout << key << Syntax::KEY_START << endl;
				indentLevel++;
				prettyPrintValues(sectionKeyValues[section][key][i], indentLevel);
				prettyPrintArrays(sectionKeyArrays[section][key][i], indentLevel);
				indentLevel--;
				cout << Syntax::KEY_END << endl;
			}
		}
	}
}

void SaveManager::prettyPrintArrays(std::map<string, std::vector<std::string>>& arryMap, int indentLevel) {
	string tabs = string(indentLevel, '\t');
	for (const auto& value : arryMap) {
		cout << tabs << value.first << Syntax::VALUE_SEPARATOR << " " << Syntax::ARRAY_START << endl;
		for (const auto& arrVal : value.second) {
			cout << tabs << "\t" << arrVal << endl;
		}
		cout << tabs << Syntax::ARRAY_END << endl;
	}
}
void SaveManager::prettyPrintValues(std::map<string, std::string>& valMap, int indentLevel) {
	string tabs = string(indentLevel, '\t');
	for (const auto& value : valMap) {
		cout << tabs << value.first << Syntax::VALUE_SEPARATOR << " " << value.second << endl;
	}
}


/************************************************************************/
/* SAVING                                                               */
/************************************************************************/

void SaveManager::startSection(string key) {
	write(Syntax::SECTION_START + key + Syntax::SECTION_END);
	section = key;
}

void SaveManager::endSection() {
	newItem();
	section = "";
}

void SaveManager::startKey(string key) {
	write(key + Syntax::KEY_START);
	key = key;
	keyLevel++;
}

void SaveManager::endKey() {
	keyLevel--;
	if (keyLevel < 0) keyLevel = 0;
	write(Syntax::KEY_END);
	key = "";
}


void SaveManager::newItem() {
	if (currentlyWriting) saveFile << Syntax::ITEM_SEPERATOR;
}