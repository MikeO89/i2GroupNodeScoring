/*****************************************************************//**
 * @file   directives.hpp
 * @brief  Contains directive definitions to handle the import vs export of symbols
 * 
 * @author Mike Orr
 * @date   April 2025
 *********************************************************************/

#pragma once

#ifdef _WIN32
    #ifdef BUILDING_I2LIB
        #define I2LIB_API __declspec(dllexport)  // Export symbols for Windows
    #else
        #define I2LIB_API __declspec(dllimport)  // Import symbols for Windows
    #endif
#elif defined(__unix__) || defined(__APPLE__)
    #ifdef BUILDING_I2LIB
        #define I2LIB_API __attribute__((visibility("default")))  // Export symbols for UNIX-like systems
    #else
        #define I2LIB_API  // Default is hidden visibility on UNIX-like systems
    #endif
#else
    #error "Unsupported platform"
#endif
