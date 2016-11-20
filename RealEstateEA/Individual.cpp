///  Individual.h
///
///	 Defines a member of the population for the EA
///

#include "stdafx.h"

#include "Constants.h"
#include "Individual.h"

#include <algorithm>
#include <assert.h>
#include <random>
#include <stdlib.h>

Individual::Individual(std::vector<Property> properties, std::mt19937 generator)
{
	m_MinimumFunds = GenerateRandom(0, STARTING_FUNDS, generator);
	m_AdditionalEmployees = GenerateRandom(0, 10, generator);

	//Define common starting company/individual variables
	m_CurrentFunds = STARTING_FUNDS;
	m_CurrentEmployees = STARTING_EMPLOYEES;

	//sell  //rent //buy //renovated //rented
	m_OwnedProperties.push_back(Property(70000, 80000, 500, 0, STARTER));
	m_OwnedProperties.push_back(Property(130000, 150000, 700, 0, STARTER));

	m_OwnedProperties[0].m_AssignedAction = RENT;
	m_OwnedProperties[1].m_AssignedAction = BUY;

	for (std::vector<Property>::iterator it = properties.begin(); it != properties.end(); it++)
	{
		//50% chance to rent 
		int randomAction = GenerateRandom((int)RENT, (int)BUY, generator);

		if (randomAction == BUY)
		{
			//50% chance to rennovate or not
			randomAction = GenerateRandom((int)BUY, (int)RENOVATE, generator);
		}

		m_Allocation.push_back(std::make_pair((AllocationAction)randomAction, (*it)));
	}

	//randomize the allocation
	std::shuffle(m_Allocation.begin(), m_Allocation.end(), generator);

	//Make a copy that can be altered
	m_WorkingAllocation = m_Allocation;
}


Individual::Individual(int minimumFunds, int additionalEmployees, std::vector<std::pair<AllocationAction, Property>> allocation)
{
	m_MinimumFunds = minimumFunds;
	m_AdditionalEmployees = additionalEmployees;
	m_Allocation = allocation;
}


void Individual::Initialize()
{
	m_CurrentFunds = STARTING_FUNDS;
	m_CurrentEmployees = STARTING_EMPLOYEES;
	m_Fitness = 0;
	m_OwnedProperties.clear();
	m_WorkingAllocation = m_Allocation;

	//sell  //rent //buy //renovated //rented
	m_OwnedProperties.push_back(Property(70000, 80000, 500, 0, STARTER));
	m_OwnedProperties.push_back(Property(130000, 150000, 700, 0, STARTER));

	m_OwnedProperties[0].m_AssignedAction = RENT;
	m_OwnedProperties[1].m_AssignedAction = BUY;
}


int Individual::GenerateRandom(int from, int to, std::mt19937& generator)
{
	std::uniform_int_distribution<int> distr(from, to);

	return distr(generator);
}


void Individual::CalculateFitness()
{
	int sum = 0;

	//sum minimum values of rented properties 
	std::vector<Property>::iterator ownedProperty = m_OwnedProperties.begin();
	for(ownedProperty; ownedProperty != m_OwnedProperties.end(); ++ownedProperty)
	{
		sum += ownedProperty->m_MinValueToSell;
	}

	//fitness includes a deduction for each employee and the total value of owner property
	m_Fitness = sum + m_CurrentFunds - 5000 * m_CurrentEmployees;
}

void Individual::CalculateMonth(std::mt19937& generator)
{
	//keep count of employee actions
	int buyingSelling = 0;
	int renovating = 0;
	int renting = 0;
	bool employeesIdle = false;

	std::vector<Property> accountedRentals;
	std::vector<Property> newPurchases;
	std::vector<Property> accountedRenovations;

	//Find an action for each employee in the company/individual 
	for(int i = 0; i < m_CurrentEmployees; ++i)
	{
		bool employeeAssigned = false;

		//First sell any owned houses ready to sell
		std::vector<Property>::iterator ownedProperty = m_OwnedProperties.begin();
		for(ownedProperty; ownedProperty != m_OwnedProperties.end(); ++ownedProperty)
		{
			//assign employees to manage each rental property for the month
			if(ownedProperty->m_AssignedAction == RENT)
			{
				++renting;
				employeeAssigned = true;
				m_CurrentFunds += ownedProperty->m_ValueForRent;

				//take this rental out of further calculations, added back after done
				accountedRentals.push_back(*(ownedProperty));
				m_OwnedProperties.erase(ownedProperty);

				//move on to next employee
				break;
			}
			//sell houses that were bought to be sold or renovated
			else if(ownedProperty->m_AssignedAction == BUY || (ownedProperty->m_AssignedAction == RENOVATE && ownedProperty->m_Renovated == true))
			{
				++buyingSelling;
				employeeAssigned = true;

				//random range between min and max value of house
				int sellValue = ownedProperty->m_MinValueToSell + (GenerateRandom(0, ownedProperty->m_MaxValueToSell - ownedProperty->m_MinValueToSell, generator));
				m_CurrentFunds += sellValue;
				m_OwnedProperties.erase(ownedProperty);

				//move on to next employee
				break;
			}
			//renovate houses that have been allocated for renovation and have not been 
			else if(ownedProperty->m_AssignedAction == RENOVATE && ownedProperty->m_Renovated == false)
			{
				++renovating;
				employeeAssigned = true;

				//increase the bounds of the random sell range
				int difference = ownedProperty->m_MaxValueToSell - ownedProperty->m_MinValueToSell;
				ownedProperty->m_MinValueToSell = ownedProperty->m_MaxValueToSell;
				ownedProperty->m_MaxValueToSell += difference / 2;
				ownedProperty->m_Renovated = true;

				//take this rental out of further calculations, added back after done
				accountedRenovations.push_back(*(ownedProperty));
				m_OwnedProperties.erase(ownedProperty);

				//move on to next employee
				break;
			}
			else
			{
				assert(false);
			}
		}//end owned properties for loop


		if(!employeeAssigned)
		{
			//owned properties handled, look for new properties if funds sufficient
			if(m_CurrentFunds > m_MinimumFunds)
			{
				if(m_WorkingAllocation.size() > 0)
				{
					++buyingSelling;

					std::pair<AllocationAction, Property> nextAllocation = m_WorkingAllocation[0];
					m_WorkingAllocation.erase(m_WorkingAllocation.begin());

					Property newProperty = nextAllocation.second;
					newProperty.m_AssignedAction = nextAllocation.first;
					newPurchases.push_back(newProperty);
					m_CurrentFunds -= newProperty.m_ValueToBuy;

					//move on to next employee
					break;
				}
			}
			else
			{
				//There was no work for this employee to perform
				employeesIdle = true;
				++buyingSelling;
			}
		}
	}//End employees for loop

	//All employee have been assigned, pay employees
	m_CurrentFunds -= std::abs((renting * LANDLORD_MONTLY_COST) - (buyingSelling * REALTOR_MONTLY_COST) - (renovating * RENOVATOR_MONTLY_COST));

	//Pay an interest fee on debt to punish agressive investment
	if(m_CurrentFunds < 0)
	{
		m_CurrentFunds -= (int) std::abs(m_CurrentFunds * INTEREST);
	}

   //hire new ones if not underminumum funds and no employees have been idle
	if(m_CurrentFunds > m_MinimumFunds && !employeesIdle && m_CurrentEmployees < STARTING_EMPLOYEES + m_AdditionalEmployees)
	{
		m_CurrentEmployees++;
	}

	//add new purchases to owned properties
	std::vector<Property>::iterator purchase = newPurchases.begin();
	for(purchase; purchase != newPurchases.end(); ++purchase)
	{
		m_OwnedProperties.insert(m_OwnedProperties.begin(), *(purchase));
	}

	std::vector<Property>::iterator renovated = accountedRenovations.begin();
	for(renovated; renovated != accountedRenovations.end(); ++renovated)
	{
		m_OwnedProperties.insert(m_OwnedProperties.begin(), *(renovated));
	}

	//add rentals back to owned properties
	std::vector<Property>::iterator rental = accountedRentals.begin();
	for(rental; rental != accountedRentals.end(); ++rental)
	{
		m_OwnedProperties.insert(m_OwnedProperties.begin(), *(rental));
	}
}


bool Individual::operator<(const Individual &rhs) const
{
	return m_Fitness < rhs.m_Fitness;
}