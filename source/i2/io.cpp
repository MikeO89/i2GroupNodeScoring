/*****************************************************************//**
 * @file   io.cpp
 * @brief  The IO implementation - source file separated from header for security
 * 
 * @author Mike Orr
 * @date   April 2025
 *********************************************************************/

#include "i2/io.hpp"
#include <fstream>
#include <iostream>

namespace I2
{
	namespace IO
	{
		bool loadJSONFromFile(std::string path, Json::Value &result)
		{
			std::ifstream file(path, std::ifstream::binary);
			Json::CharReaderBuilder builder;
			std::string errors;

			if(!file.is_open())
			{
				std::cerr << "Error: opening file with path '" + path + "' failed.";
				return false;
			}

			if(Json::parseFromStream(builder, file, &result, &errors))
				return true;

			std::cerr << "Error: Failed to parse JSON, errors:\n" << errors << std::endl;
			return false;
		}
	}
}