/*****************************************************************//**
 * @file   nodeLoader.cpp
 * @brief  Implements useful functions for loading nodes and associated links with weights - source file separated from header for security
 * 
 * @author Mike Orr
 * @date   April 2025
 *********************************************************************/

#include "i2/nodeLoader.hpp"
#include "i2/io.hpp"
#include <iostream>

namespace I2
{
	namespace NodeLoader
	{
		std::vector<std::shared_ptr<Node>> constructNodesFromJSON(const Json::Value &data, bool nodesCanLinkToSelf)
		{
			const Json::String nodeKey = "nodes", linkKey = "links", nameKey = "name", sourceKey = "source", targetKey = "target", valueKey = "value";

			std::vector<std::shared_ptr<Node>> result;
			std::shared_ptr<Node> sourceNode = nullptr, targetNode = nullptr;
			std::size_t nodeCount = 0, linkCount = 0;
			std::string error = "";
			unsigned int weight = 0, sourceIndex = 0, targetIndex = 0;

			if(!data.isObject() || !data.isMember(nodeKey) || !data.isMember(linkKey) || !data[nodeKey].isArray() || !data[linkKey].isArray())
				throw std::runtime_error("Error: JSON data is not valid: expected an object containing arrays 'nodes' and 'links'.");

			// Pre-allocate the size for optimisation (without this the entire result list would have to be relocated in memory on each append,
			// to make room for the next entry, as the data in a std::vector sits at consecutive memory addresses
			nodeCount = data[nodeKey].size();
			result.reserve(nodeCount);

			// Load the nodes without links (need all nodes to exist prior to linking)
			for(int i=0;i<nodeCount;++i)
			{
				if(!data[nodeKey][i].isObject() || !data[nodeKey][i].isMember(nameKey) || !data[nodeKey][i][nameKey].isString())
					throw std::runtime_error("Invalid node at index '" + std::to_string(i) + "'.");

				result.push_back(std::make_shared<Node>(data[nodeKey][i][nameKey].asString()));
			}

			// Link the nodes
			linkCount = data[linkKey].size();

			for(int i=0;i<linkCount;++i)
			{
				error = "Invalid link at index '" + std::to_string(i) + "'.";

				if(!data[linkKey][i].isObject() || !data[linkKey][i].isMember(sourceKey)  || !data[linkKey][i].isMember(targetKey)  || !data[linkKey][i].isMember(valueKey) // Ensure the JSON structure is valid
					|| !data[linkKey][i][sourceKey].isUInt() || !data[linkKey][i][targetKey].isUInt() || !data[linkKey][i][valueKey].isUInt()) // Ensure each member is an unsigned integer
				{
					throw std::runtime_error(error);
				}

				sourceIndex = data[linkKey][i][sourceKey].asUInt();
				targetIndex = data[linkKey][i][targetKey].asUInt();

				if((sourceIndex >= nodeCount || targetIndex >= nodeCount) // Ensure source and target indexes reference a valid node
					|| (!nodesCanLinkToSelf && sourceIndex == targetIndex))
				{
					throw std::runtime_error(error);
				}

				// Extract the referenced nodes, as well as the assigned weight
				sourceNode = result[sourceIndex];
				targetNode = result[targetIndex];
				weight = data[linkKey][i][valueKey].asUInt();

				// Add the link to both the source and the target node, together with the associated weight
				sourceNode->addLink(targetNode,weight);
				targetNode->addLink(sourceNode,weight);
			}

			return result;
		}

		std::vector<std::pair<std::shared_ptr<Node>,double>> computePageRank(const std::vector<std::shared_ptr<Node>> &nodeList, double dampeningFactor, double tolerance)
		{
			const std::size_t nodeCount = nodeList.size();
			const double nodeCountD = static_cast<double>(nodeCount);
			const double initialRank = 1.0 / nodeCountD;
			const double dampeningDiff = 1.0 - dampeningFactor;
			const double maxRankValue = 1e3;  // Limit rank to 3-4 figures (e.g., max of 1000)
			constexpr double maxDouble = std::numeric_limits<double>::max();

			std::vector<std::pair<std::shared_ptr<Node>,double>> result(nodeCount);
			std::map<std::shared_ptr<Node>,double> pageRank;
            bool finishedRanking = false;
			int index = 0;

            // Initialize each node's PageRank value

			for(std::vector<std::shared_ptr<Node>>::const_iterator itN=nodeList.cbegin(),endN=nodeList.cend();itN!=endN;++itN)
				pageRank[*itN] = initialRank; // Equal distribution of rank initially

            while(!finishedRanking)
            {
                finishedRanking = true;
                std::map<std::shared_ptr<Node>,double> newPageRank;
				
                // Iterate over each node and calculate its new PageRank
                for(const std::shared_ptr<Node> &node : nodeList)
                {
                    double rankSum = 0.0;
					std::map<std::shared_ptr<Node>,unsigned int> linkList = node->getLinks();

                    // Get each linked node and accumulate rank contributions
                    for(const auto &link : linkList)
                    {
						const auto &linkedNode = link.first;
						double weight = static_cast<double>(link.second);
						double linkCount = static_cast<double>(linkedNode->getLinkCount());

						// If the linked node has an outgoing link, contribute to the rank of this node
						if(pageRank[linkedNode] < maxDouble / weight)
							rankSum += (pageRank[linkedNode] * weight) / (linkCount ? linkCount : 1.0);
						else // If it's too large, apply a maximum rank value
							rankSum += std::min(maxDouble, pageRank[linkedNode]) / (linkCount ? linkCount : 1.0);
                    }

                    // Apply the dampening factor and set the new PageRank for this node
                    newPageRank[node] = dampeningDiff / nodeCountD + dampeningFactor * rankSum;
                    newPageRank[node] = std::min(newPageRank[node], maxRankValue);
                }

                // Check for convergence (difference between old and new PageRank values)
                for(const auto &newPR : newPageRank)
                {
                    if(std::fabs(pageRank[newPR.first] - newPR.second) > tolerance)
                    {
                        finishedRanking = false;
                        break;
                    }
                }

                // Update the PageRank values
                pageRank = newPageRank;
            }

			index = 0;

			for(std::map<std::shared_ptr<Node>,double>::iterator itPR=pageRank.begin(),endPR=pageRank.end();itPR!=endPR;++itPR)
				result[index++] = std::make_pair(std::move(itPR->first),std::move(itPR->second)); // Move the results to the output container

			return result;
		}

		std::vector<std::shared_ptr<Node>> loadNodesFromFile(std::string path, bool nodesCanLinkToSelf)
		{
			Json::Value data;
			std::vector<std::shared_ptr<Node>> result;

			if(I2::IO::loadJSONFromFile(path, data))
				result = constructNodesFromJSON(data,nodesCanLinkToSelf);

			return result;
		}

		bool pageRankComparatorGT(const std::pair<std::shared_ptr<Node>,double> &a, const std::pair<std::shared_ptr<Node>,double> &b)
		{
			return a.second > b.second;
		}
	}
}
