///  Individual.h
///
///	 Defines a member of the population for the EA
///

#pragma once

#include "Property.h"

#include <random>
#include <vector>


enum EvolutionAction
{
	MUTATE,
	SWAP,
	SHUFFLE
};

enum IndividualElement
{
	MIN_FUNDS,
	ADDITIONAL_EMPLOYEES,
	ALLOCATION
};

class Individual
{
public:
	Individual(std::vector<Property> properties, std::mt19937 generator);
	Individual(int minimumFunds, int additionalEmployees, std::vector<std::pair<AllocationAction, Property>> allocation);
	~Individual() {}

	void CalculateMonth(std::mt19937& generator);
	void CalculateFitness();
	int GenerateRandom(int from, int to, std::mt19937& engine);
	void Initialize();

	bool operator<(const Individual &rhs) const;

	int m_CurrentFunds;
	int m_CurrentEmployees;
	int m_Fitness;
   int m_MinimumFunds;
	int m_AdditionalEmployees;

	std::vector<Property> m_OwnedProperties;
	std::vector<std::pair<AllocationAction, Property>> m_WorkingAllocation;
	std::vector<std::pair<AllocationAction, Property>> m_Allocation;
};