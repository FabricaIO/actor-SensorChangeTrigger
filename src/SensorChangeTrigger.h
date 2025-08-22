/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2025 Sam Groveman
* 
* External libraries needed:
* ArduinoJSON: https://arduinojson.org/
*
* Contributors: Sam Groveman
*/
#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PeriodicActionTrigger.h>
#include <ParameterTrigger.h>

/// @brief Class providing action triggers based on sensor readings
class SensorChangeTrigger : public PeriodicActionTrigger {
	protected:
		/// @brief Path to configuration file
		String config_path;

		/// @brief Stores the last sensor value
		double old_value = 0;

		/// @brief Provides access to sensor value
		ParameterTrigger sensor_value;

		bool configureOutput();
		bool triggerAction(String payload) override;
		JsonDocument addAdditionalConfig();

	public:
		SensorChangeTrigger(String Name, String configFile = "SensorChangeTrigger.json");
		bool begin();
		String getConfig();
		bool setConfig(String config, bool save);
};
