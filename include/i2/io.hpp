/*****************************************************************//**
 * @file   io.hpp
 * @brief  Abstracted IO functionality
 * 
 * @author Mike Orr
 * @date   April 2025
 *********************************************************************/

#pragma once

#ifndef I2_IO_HPP
#define I2_IO_HPP

#include <json/json.h>
#include "i2/directives.hpp"

namespace I2
{
	namespace IO
	{
		/**
		 * @brief Takes the contents of a file and loads it into a Json::Value instance.
		 * @param[in] path The path to the JSON file
		 * @param[out] result Contains the converted result, if the file contents was valid JSON
		 * @return true if valid JSON was successfully loaded into result, false if valid JSON was not loaded into result
		 */
		bool I2LIB_API loadJSONFromFile(std::string path, Json::Value &result);
	}
}

#endif