///  Individual.h
///
///	 Defines a member of the population for the EA
///

#pragma once

#include "Property.h"

#include <vector>


enum EvolutionAction
{
	MUTATE,
	SWAP,
	INVERT
};

class Individual
{
public:
	Individual(std::vector<Property> properties);
	~Individual() {}

	void CalculateMonth();
	void CalculateFitness();
	int GenerateRandom(int from, int to);

	bool operator<(const Individual &rhs) const;

	int m_CurrentFunds;
	int m_CurrentEmployees;
	int m_Fitness;
   int m_MinimumFunds;
	int m_AdditionalEmployees;

	std::vector<Property> m_ownedProperties;
	std::vector<std::pair<AllocationAction, Property>> m_WorkingAllocation;
	std::vector<std::pair<AllocationAction, Property>> m_Allocation;
};