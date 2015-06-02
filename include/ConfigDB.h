#include <OgreConfigFile.h>
#include <map>

// This class is enhanced from example at: http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Simple+config+text+parser
// Ogre::ConfigFile class is reused to store data from a custom config file (parameters.cfg) and keep it loaded into a map
// N.B. you must know the type of the value you want in advance and call the right function (that will convert it from string)

// USAGE
// 
class ConfigDB
{
public:
	ConfigDB(const std::string& filePath);
	static ConfigDB* getInstance();
	
	//various getters used by class clients to get configuration directives
	int getValueAsInt(std::string key);
	Ogre::Real getValueAsReal(std::string key);
	std::string getValueAsString(std::string key);
	Ogre::Vector3 getValueAsVector3(std::string key);
	bool getValueAsBool(std::string key);
	Ogre::ColourValue getValueAsColourValue(std::string key);
	bool getKeyExists(std::string key);

private:
	static ConfigDB* _instance;
	~ConfigDB();
	ConfigDB(const ConfigDB &); //standard protection for singletons; prevent copy construction
	ConfigDB& operator=(const ConfigDB &); //standard protection for singletons; prevent assignment

	//this is our config file
	Ogre::ConfigFile mConfigFile;
	bool mConfigFileLoaded = false;
	//this is where our configuration data is stored
	std::map<std::string, std::string> mConfigMap;
};