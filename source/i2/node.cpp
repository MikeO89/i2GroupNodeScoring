/*****************************************************************//**
 * @file   node.cpp
 * @brief  The Node class (and related) function definitions - source file separated from header for security
 * 
 * @author Mike Orr
 * @date   April 2025
 *********************************************************************/

#include "i2/node.hpp"
#include <stdexcept>
#include <iostream>

namespace I2
{
	Node::Node(std::string name) : _name(name), _weightedDegree(0)
	{
		if(!name.length())
			throw std::runtime_error("Node invalid: no name provided!");
	}

	Node::Node(const Node &other)
	{
		this->_name = other._name;
		this->addLinks(other._link); // The addLinks call will update _weightedDegree
	}

	Node::Node(Node &&other) noexcept
	{
		std::shared_lock<std::shared_mutex> locker(other._lock); // Lock for reading (allow simultaneous reads, but prevent writes)

		this->_name									= std::move(other._name);
		this->_weightedDegree						= std::move(other._weightedDegree);
		this->_link									= std::move(other._link);
	}

	std::string Node::getName(void) const noexcept
	{
		return this->_name;
	}

	std::unordered_map<std::shared_ptr<Node>, unsigned int> Node::getLinks(void) const noexcept
	{
		return this->_link;
	}

	unsigned int Node::getWeightedDegree(void) const noexcept
	{
		return this->_weightedDegree;
	}

	unsigned int Node::getLinkCount(void) const noexcept
	{
		return static_cast<unsigned int>(this->_link.size());
	}

	unsigned int Node::recalculateWeightedDegree(void) noexcept
	{
		unsigned int weightedDegree = 0; // Temporarily stores the accumulation of the link weights

		std::shared_lock<std::shared_mutex> locker(this->_lock); // Lock for reading (allow simultaneous reads, but prevent writes)

		for(std::unordered_map<std::shared_ptr<Node>,unsigned int>::const_iterator itL=this->_link.cbegin(),endL=this->_link.cend();itL!=endL;++itL)
			weightedDegree += itL->second; // Accumulate the weight of each node

		this->_weightedDegree = weightedDegree; // Save the weighted result

		return weightedDegree;
	}

	void Node::addLink(std::shared_ptr<Node> n, unsigned int weight)
	{
		if(!n)
			throw std::runtime_error("Error: Invalid node.");

		std::unique_lock<std::shared_mutex> locker(this->_lock); // Lock for writing (prevent simultaneous read/writes)

		auto insertResult = this->_link.insert(std::pair<std::shared_ptr<Node>,unsigned int>(n,weight));

		if(insertResult.second) // Insert was successful (link did not pre-exist)
			this->_weightedDegree += weight;
		else
			std::cerr << "Attempted to add a link that pre-exists!";
	}

	void Node::addLinks(std::unordered_map<std::shared_ptr<Node>, unsigned int> link)
	{
		if(this->getLinkCount())
		{ // If there are pre-existing links then we need to check if each link pre-exists and only modify the stored weighted degree accordingly
			for(std::unordered_map<std::shared_ptr<Node>,unsigned int>::const_iterator itL=link.cbegin(),endL=link.cend();itL!=endL;++itL)
				this->addLink(itL->first, itL->second);
		}
		else 
		{ // Cut out of the overhead in Node::addLink if there are no pre-existing links to be checked
			{ // Provide a separate scope so the lock releases prior to the recalculateWeightedDegree call - equivalent to 'locker.release()'
				std::unique_lock<std::shared_mutex> locker(this->_lock); // Lock for writing (prevent simultaneous read/writes)
				this->_link.insert(link.cbegin(),link.cend());
			}

			this->recalculateWeightedDegree(); // Determine and store the weighted degree after mass insertion of links
		}
	}

	void Node::removeLink(std::shared_ptr<Node> n)
	{
		std::unordered_map<std::shared_ptr<Node>, unsigned int>::node_type link;

		if(!n) // Unable to process n if it is not valid
			throw std::runtime_error("Error: Invalid node.");

		std::unique_lock<std::shared_mutex> locker(this->_lock); // Lock for writing (prevent simultaneous read/writes)

		link = this->_link.extract(n); // Extracting the link (if it exists) so that we can use to adjust the weighted degree

		if(!link.empty()) // Adjust the over all weighted degree of the weight associated with the link, if the link exists
			this->_weightedDegree -= link.mapped();
	}

	std::strong_ordering Node::operator<=>(const Node &other) const noexcept
	{
		return this->_weightedDegree <=> other._weightedDegree;
	}

	Node::operator std::string(void) const noexcept
	{
		return this->_name + ": " + std::to_string(this->_weightedDegree);
	}

	std::ostream & operator<<(std::ostream& os, const Node &n) noexcept
	{
		os << static_cast<std::string>(n) << std::endl;
		return os;
	}

	bool nodeCompareGT(const std::shared_ptr<I2::Node>& n1, const std::shared_ptr<I2::Node>& n2)
	{
		if(!n1 || !n2)
			throw std::runtime_error("Error: n1 and/or n2 are invalid");

		return (*n1 <=> *n2) == std::strong_ordering::greater;
	}
}

