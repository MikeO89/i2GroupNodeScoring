/*****************************************************************//**
 * @file   node.hpp
 * @brief  Declarations of a Node structure representation from a graph
 * 
 * @author Mike Orr
 * @date   April 2025
 *********************************************************************/

#pragma once

#ifndef I2_NODE_HPP
#define I2_NODE_HPP

#include <string>
#include <map>
#include <memory>
#include <shared_mutex>
#include "i2/directives.hpp"

namespace I2
{
	/**
	 * @class Node
	 * @brief Declarations representing a node in a graph
	 * 
	 * This class is used to store information about a node, including links to other nodes and associated weight.
	 * Nodes can be linked to other nodes to form a graph-like structure
	 */
	class I2LIB_API Node
	{
	private:
		std::map<std::shared_ptr<Node>, unsigned int> _link; // Using a heap Node pointer, controlled with shared_ptr might be slower than stack memory but it is safer when it comes to referencing
		mutable std::shared_mutex _lock; // To ensure thread safety
		std::string _name;
		unsigned int _weightedDegree; // Modified upon appending/removing a given link. More efficient to store/modify the result than to calculate each time it is needed

	public:
		/**
		 * @brief Constructs a new Node object with the given name.
		 * @param[in] name The unique name of the node
		 */
		explicit Node(std::string name);

		/**
		 * @brief Constructs a new Node object by copying an existing Node instance.
		 * @param[in] other The Node instance to copy
		 */
		Node(const Node &other);

		/**
		 * @brief Changes the ownership of a pre-existing Node to this instance.
		 * @param[in] other The Node instance of which will we take ownership of the memory
		 */
		Node(Node &&other) noexcept;

		/**
		 * @return The node's name
		 */
		[[nodiscard]] std::string getName(void) const noexcept;

		/**
		 * @return All linked nodes with associated weights
		 */
		[[nodiscard]] std::map<std::shared_ptr<Node>, unsigned int> getLinks(void) const noexcept;

		/**
		* @brief Retrieves the pre-stored weighted degree
		* @return The accumulation of all link weights
		**/
		[[nodiscard]] unsigned int getWeightedDegree(void) const noexcept;

		/**
		* @return The total number of linked nodes
		**/
		[[nodiscard]] unsigned int getLinkCount(void) const noexcept;

		/**
		* @brief Calculates and stores - within Node::_weightedDegree - the accumulation of all link weights
		* @return The accumulation of all link weights
		**/
		unsigned int recalculateWeightedDegree(void) noexcept;

		/**
		* @brief Adds link n to Node::_link and updates Node::_weightedDegree (if n does not pre-exist within Node::_link)
		* @param[in] n The link to add to Node::_link
		* @param[in] weight the weight associated to n
		**/
		void addLink(std::shared_ptr<Node> n, unsigned int weight);

		/**
		* @brief Attempts to insert many Node/weight link pairs
		* @details Inserts are more optimal if links to not pre-exist
		* @param[in] n The link to add to Node::_link
		* @param[in] weight the weight associated to n
		**/
		void addLinks(std::map<std::shared_ptr<Node>, unsigned int> link);

		/**
		* @brief Removes the node/weight for n and updates Node::_weightedDegree, if the pair is present in Node::_link
		**/
		void removeLink(std::shared_ptr<Node> n);

		/**
		 * @brief Spaceship operator to handle comparisons based on weighted degree.
		 * @param[in] other The instance to be compared with.
		 * @return The comparison of both instance weighted degrees
		 */
		[[nodiscard]] std::strong_ordering operator<=>(const Node &other) const noexcept;

		/**
		 * @brief String operator to output the class name together with the weighted degree.
		 * @return The result of the anme and weighted degree concatenation
		 */
		[[nodiscard]] operator std::string(void) const noexcept;
	};

	/**
	 * @brief Makes use of the Node string operator to make Node streamable.
	 * @param[out] os The output stream to output the node to.
	 * @param[in] The node to output
	 * @return The output stream containing the streamed node
	 */
	I2LIB_API std::ostream & operator<<(std::ostream& os, const Node &n) noexcept;

	/**
	 * @brief A comparator function that compares two node pointers: useful with the standard library container manipulation functions.
	 * @param[in] n1 The first node for comparison
	 * @param[in] n2 The second node for comparison
	 */
	I2LIB_API bool nodeCompareGT(const std::shared_ptr<I2::Node>& n1, const std::shared_ptr<I2::Node>& n2);
}

#endif