#include "ConfigDB.h"
#include <OgreResourceGroupManager.h>
#include <OgreStringConverter.h>

ConfigDB* ConfigDB::_instance = nullptr;

ConfigDB::ConfigDB(const std::string& filePath = "/cfg/parameters.cfg")
{
	// this assures constructor is called only once after a config file is correctly loaded!
	assert(!mConfigFileLoaded);
	try
	{
		mConfigFile.load(filePath, "=", true);
	}
	catch (Ogre::FileNotFoundException &e)
	{
		// if file is not found, object is not created, creator is able to decide what to do
		throw e;
	}
	// instance fully created. Not possible to call constructor anymore (singleton paradigm)
	mConfigFileLoaded = true;
	_instance = this;

	// Init parsing
	Ogre::ConfigFile::SectionIterator seci = mConfigFile.getSectionIterator();
	Ogre::String sectionName;
	Ogre::String keyName;
	Ogre::String valueName;

	while (seci.hasMoreElements())
	{
		sectionName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			keyName = i->first;
			valueName = i->second;
			// populate map
			mConfigMap.insert(std::pair<std::string, std::string>(sectionName + "/" + keyName, valueName));
		}
	}

}

ConfigDB::~ConfigDB() { }

ConfigDB* ConfigDB::getInstance()
{
	return _instance;
}

bool ConfigDB::getKeyExists(std::string key)
{
	if (mConfigMap.count(key) > 0)
	{
		return true;
	}
	return false;
}

std::string ConfigDB::getValueAsString(std::string key)
{
	if (getKeyExists(key) == true)
	{
		return mConfigMap[key];
	}
	else
	{
		throw Ogre::Exception(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Configuration key: " + key + " not found", "ConfigDB::getValue");
	}

}

int ConfigDB::getValueAsInt(std::string key)
{
	return atoi(getValueAsString(key).c_str());
}
Ogre::Real ConfigDB::getValueAsReal(std::string key)
{
	return Ogre::StringConverter::parseReal(getValueAsString(key));
}
bool ConfigDB::getValueAsBool(std::string key)
{
	return Ogre::StringConverter::parseBool(getValueAsString(key));
}
Ogre::Vector3 ConfigDB::getValueAsVector3(std::string key)
{
	return Ogre::StringConverter::parseVector3(getValueAsString(key));
}
Ogre::ColourValue ConfigDB::getValueAsColourValue(std::string key)
{
	return Ogre::StringConverter::parseColourValue(getValueAsString(key));
}