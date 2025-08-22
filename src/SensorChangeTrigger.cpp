#include"SensorChangeTrigger.h"

extern bool POSTSuccess;

/// @brief Creates a sensor action trigger
/// @param Name The device name
/// @param configFile Name of the config file to use
SensorChangeTrigger::SensorChangeTrigger(String Name, String configFile) : PeriodicActionTrigger(Name, configFile) {
	config_path = "/settings/act/" + configFile;
}

/// @brief Starts a sensor trigger 
/// @return True on success
bool SensorChangeTrigger::begin() {
	sensor_value.parameter_config.Enabled = true;
	sensor_value.parameter_config.Parameters.resize(1);
	// Check if file exists
	bool configExists = checkConfig(config_path);
	// Create settings directory if necessary
	if (PeriodicActionTrigger::begin()) {
		// Set description
		if (!configExists) {
			// Set defaults
			trigger_config.payload = "%SENSOR_VALUE%";
			return setConfig(getConfig(), true);
		} else {
			// Load settings
			return setConfig(Storage::readFile(config_path), false);
		}
	}
	return false;
}

/// @brief Gets the current config
/// @return A JSON string of the config
String SensorChangeTrigger::getConfig() {
	JsonDocument doc = addAdditionalConfig();
	doc["valueDeltaMin"] = sensor_change_config.valueDeltaMin;
	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool SensorChangeTrigger::setConfig(String config, bool save) {
	if (PeriodicActionTrigger::setConfig(config, false)) {
		// Allocate the JSON document
		JsonDocument doc;
		// Deserialize file contents
		DeserializationError error = deserializeJson(doc, config);
		// Test if parsing succeeds.
		if (error) {
			Logger.print(F("Deserialization failed: "));
			Logger.println(error.f_str());
			return false;
		}
		// Parse sensor parameter
		String sensor_combined = doc["sensorParameter"]["current"].as<String>();
		int colon;
		if ((colon = sensor_combined.indexOf(':')) != -1) {
			std::pair<String, String> chosen {sensor_combined.substring(0, colon), sensor_combined.substring(colon + 1)};
			sensor_value.parameter_config.Parameters[0] = chosen;
		}
		sensor_change_config.valueDeltaMin = doc["valueDeltaMin"].as<double>();
		if (save) {
			return saveConfig(config_path, config);
		}
		return true;
	}
	return false;
}

/// @brief Triggers the set action
/// @param payload A payload to pass to the action
/// @return True on success
bool SensorChangeTrigger::triggerAction(String payload) {
	if (POSTSuccess) {
		std::map<String, std::map<String, double>> params = sensor_value.getParameterValues();
		double value = params[sensor_value.parameter_config.Parameters[0].first][sensor_value.parameter_config.Parameters[0].second];
		double delta = std::abs(old_value - value);
		if (delta >= sensor_change_config.valueDeltaMin) {
			old_value = value;
			// Parse payload template
			payload.replace("%SENSOR_VALUE%", String(value));
			return trigger.triggerActions({{action[0], {{action[1], payload}}}});
		}
		return true;
	}
	return false;
}

/// @brief Collects all the base class parameters and additional parameters
/// @return a JSON document with all the parameters
JsonDocument SensorChangeTrigger::addAdditionalConfig() {
	// Allocate the JSON document
  	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, PeriodicActionTrigger::getConfig());
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return doc;
	}
	// Add all sensor options to dropdown
	doc["sensorParameter"]["current"] = sensor_value.parameter_config.Parameters.size() > 0 ? sensor_value.parameter_config.Parameters[0].first + ":" + sensor_value.parameter_config.Parameters[0].second : "";
	std::map<String, std::vector<String>> sensors = sensor_value.listAllParameters();
	int i = 0;
	if (sensors.size() > 0) {
		i = 0;
		for (std::map<String, std::vector<String>>::iterator sensor = sensors.begin(); sensor != sensors.end(); sensor++) {
			for (const auto& p : sensor->second) {
				doc["sensorParameter"]["options"][i] = sensor->first + ":" + p;
				i++;
			}
		}
	} else {
		doc["sensorParameter"]["options"][0] = "";
	}
	return doc;
}
