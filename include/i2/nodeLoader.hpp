/*****************************************************************//**
 * @file   nodeLoader.hpp
 * @brief  Contains useful functions for building out nodes from JSON/file
 * 
 * @author Mike Orr
 * @date   April 2025
 *********************************************************************/

#pragma once

#ifndef I2_NODE_LOADER_HPP
#define I2_NODE_LOADER_HPP

#include "i2/node.hpp"
#include <json/json.h>

namespace I2
{
	namespace NodeLoader
	{
		/**
		 * @brief Builds nodes and adds the relevant links (node and associated weight) from JSON.
		 * @param[in] data The JSON object containing a list of nodes with names, as well as a list of links to other nodes and associated weights
		 * @param[in] nodesCanLinkToSelf when true links are valid if source and target match
		 * @return List of constructed node instances
		 */
		std::vector<std::shared_ptr<Node>> I2LIB_API constructNodes(const Json::Value &data, bool nodesCanLinkToSelf = false);

		/**
		 * @brief Applies Google's PageRank formula to a list of nodes.
		 * @param[in] nodeList The list of nodes on which to apply the PageRank formula.
		 * @param[in] dampeningFactor Ensures that nodes with fewer links are not penalised too much. The damping factor is a constant used to control the redistribution of ranks.
		 * @param[in] tolerance Used to determine if the ranking adjustments are too miniscule to continue recursive ranking.
		 * @return An unsorted list of ranked nodes.
		 */
		std::vector<std::pair<std::shared_ptr<Node>,double>> I2LIB_API computePageRank(const std::vector<std::shared_ptr<Node>> &nodeList, double dampeningFactor = 0.85, double tolerance = 1e-1);

		/**
		 * @brief Takes a path to a file containing JSON data of nodes, links to other nodes and the weight associated with the given link, converts it to a Json::Value instance, and produces a list of accurate nodes by calling constructNodes.
		 * @param[in] path The path to a file containing the JSON data
		 * @param[in] nodesCanLinkToSelf when true links are valid if source and target match
		 * @return List of constructed node instances
		 */
		std::vector<std::shared_ptr<Node>> I2LIB_API loadNodesFromFile(std::string path, bool nodesCanLinkToSelf = false);

		/**
		 * @brief Comparator used for sorting a PageRank list in descending order.
		 * @param[in] a The left-hand parameter for comparison.
		 * @param[in] b The right-hand parameter for comparison.
		 * @return true if the left-hand parameter is greater than the right-hand parameter.
		 */
		bool I2LIB_API pageRankComparatorGT(const std::pair<std::shared_ptr<Node>,double> &a, const std::pair<std::shared_ptr<Node>,double> &b);
	}
}

#endif