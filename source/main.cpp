/*****************************************************************//**
 * @file   main.cpp
 * @brief  Utilises the Node/NodeLoader functionality to implement the required output for the i2 Group technical task (Question #1)
 * 
 * @author Mike Orr
 * @date   April 2025
 *********************************************************************/

#include <i2/nodeLoader.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <iomanip>

namespace po = boost::program_options;

/**
 * @brief i2GroupTechTest entry point.
 * @param[in] argC The argument count contained in argV
 * @param[in] argV List of arguments passed to the application: the first argument is the path to the executable
 * @return Response code: 0 indicates success, non-zero indicates failure.
 */
int main(int argC, char **argV)
{
	std::vector<std::shared_ptr<I2::Node>> nodeList;
	std::vector<std::pair<std::shared_ptr<I2::Node>,double>> pageRank;
	po::options_description allOptions("Menu"), generalOptions("General"), processOptions("Process"), rankOptions("Rank");
	std::string path = "";

	generalOptions.add_options() // Build out the CLI menu options
		("help,h", "Display the help content.");

	processOptions.add_options()
		("process,p", po::value<std::string>(&path),"Processes the specified JSON Node file and outputs the weighted results.");

	rankOptions.add_options()
		("rank,r","PageRank the nodes and output the PageRank results.");

	allOptions.add(generalOptions).add(processOptions).add(rankOptions);

	po::variables_map varMap;

	try
	{
		po::store(po::parse_command_line(argC,argV,allOptions),varMap);
		po::notify(varMap); // Load in the expected command line arguments, if they have been passed

		if(argC == 1 || varMap.count("help") || !path.length())
		{ // Display the menu if no arguments were passed (other than the executable path), if help was specifically requested, or if the path to the JSON was not passed
			std::cout << allOptions << std::endl;
			return 0;
		}

		if(varMap.count("process"))
		{
			nodeList = std::move(I2::NodeLoader::loadNodesFromFile(path)); // Utilise the I2 library to load nodes and associate nodes linked to weights
			std::sort(nodeList.begin(),nodeList.end(),I2::nodeCompareGT); // Sort into descending order (by weighted degree)

			for(std::vector<std::shared_ptr<I2::Node>>::const_iterator itN=nodeList.cbegin(),endN=nodeList.cend();itN!=endN;++itN)
				std::cout << **itN; // Output each node: should be highest weighted degree first

			if(varMap.count("rank"))
			{
				std::cout << std::endl; // Separte this output from the above output
				pageRank = I2::NodeLoader::computePageRank(nodeList); // Determine the rankings

				std::sort(pageRank.begin(),pageRank.end(),I2::NodeLoader::pageRankComparatorGT); // Sort the rankings, descending

				// Output the PageRank results
				for(std::vector<std::pair<std::shared_ptr<I2::Node>,double>>::const_iterator itR=pageRank.cbegin(),endR=pageRank.cend();itR!=endR;++itR)
					std::cout << itR->first->getName() << ": " << std::fixed << std::setprecision(2) << itR->second << std::endl;
			}
		}
	}
	catch(const po::error &e)
	{
		std::cerr << "Error: " << e.what() << std::endl; // Report unexpected issues with program_options
		return 1;
	}
	catch(const std::runtime_error &e)
	{
		std::cerr << "Error: " << e.what() << std::endl; // Report unexpected issues that occurred during runtime
		return 2;
	}

	return 0; // Execution completed successfully
}