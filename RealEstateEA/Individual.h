///  Individual.h
///
///	 Defines a member of the population for the EA
///

#pragma once

#include "Property.h"

#include <vector>

class Individual
{
public:
	Individual(std::vector<Property> properties);
	~Individual() {}

	void CalculateMonth();

	int m_currentFunds;
	int m_currentEmployees;
   int m_MinimumFunds;
	int m_AdditionalEmployees;

	std::vector<Property> m_ownedProperties;
	std::vector<std::pair<AllocationAction, Property>> m_Allocation;
};