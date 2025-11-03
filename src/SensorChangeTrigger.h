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
		/// @brief Trigger configuration
		struct {
			/// @brief The value the measurement has to change by to trigger 
			double valueDeltaMin = 0;
		} sensor_change_config;

		/// @brief Path to configuration file
		String config_path;

		/// @brief Stores the last sensor value
		double old_value = 0;

		/// @brief Provides access to sensor value
		ParameterTrigger sensor_value;

		/// @brief Index for action that Checks if the sensor should trigger the action
		int forceTrigger;

		bool configureOutput();
		bool triggerAction(String payload) override;
		JsonDocument addAdditionalConfig();

	public:
		SensorChangeTrigger(String Name, String configFile = "SensorChangeTrigger.json");
		std::tuple<bool, String> receiveAction(int action, String payload);
		bool begin();
		String getConfig();
		bool setConfig(String config, bool save);
};
