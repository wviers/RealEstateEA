///  Individual.h
///
///	 Defines a member of the population for the EA
///

#include "stdafx.h"

#include "Constants.h"
#include "Individual.h"

#include <algorithm>
#include <random>
#include <stdlib.h>

Individual::Individual(std::vector<Property> properties) : m_MinimumFunds(rand() % STARTING_FUNDS), m_AdditionalEmployees(rand() % 10)
{
	//Define common starting company/individual variables
	m_currentFunds = STARTING_FUNDS;
	m_currentEmployees = STARTING_EMPLOYEES;

	//sell  //rent //buy //renovated //rented
	m_ownedProperties.push_back(Property(70000, 80000, 500, 0, false));
	m_ownedProperties.push_back(Property(130000, 150000, 700, 0, false));

	m_ownedProperties[0].m_AssignedAction = RENT;
	m_ownedProperties[1].m_AssignedAction = BUY;

	auto engine = std::default_random_engine{};

	for (std::vector<Property>::iterator it = properties.begin(); it != properties.end(); it++)
	{
		//50% chance to rent 
		int randomAction = rand() % 2;

		if (randomAction == BUY)
		{
			//50% chance to rennovate or not
			randomAction = rand() % 2 + 1;
		}

		m_Allocation.push_back(std::make_pair(AllocationAction(randomAction), (*it)));
	}

	//randomize the allocation
	std::shuffle(m_Allocation.begin(), m_Allocation.end(), engine);
}


void Individual::CalculateMonth()
{
	//keep count of employee actions
	int buyingSelling = 0;
	int renovating = 0;
	int renting = 0;
	bool employeesIdle = false;

	//Find an action for each employee in the company/individual 
	for(int i = 0; i < m_currentEmployees; ++i)
	{
		//First sell any owned houses ready to sell
		std::vector<Property>::iterator ownedProperty;
		for(ownedProperty; ownedProperty != m_ownedProperties.end(); ++ownedProperty)
		{
			//assign employees to manage each rental property for the month
			if(ownedProperty->m_AssignedAction == RENT)
			{
				++renting;
				m_currentFunds += ownedProperty->m_ValueForRent;
				break;
			}
			//sell houses that were bought to be sold or renovated
			else if(ownedProperty->m_AssignedAction == BUY || (ownedProperty->m_AssignedAction == RENOVATE && ownedProperty->m_Renovated == true))
			{
				++buyingSelling;
				//random range between min and max value of house
				int sellValue = ownedProperty->m_MinValueToSell + rand() % ownedProperty->m_MinValueToSell - ownedProperty->m_MaxValueToSell;
				m_currentFunds += sellValue;
				m_ownedProperties.erase(ownedProperty);

				//move on to next employee
				break;
			}
			else if(ownedProperty->m_AssignedAction == RENOVATE && ownedProperty->m_Renovated == false)
			{
				++renovating;

				//increase the bounds of the random sell range
				int differnece = ownedProperty->m_MaxValueToSell - ownedProperty->m_MinValueToSell;
				ownedProperty->m_MinValueToSell = ownedProperty->m_MaxValueToSell;
				ownedProperty->m_MaxValueToSell += differnece / 2;
				ownedProperty->m_Renovated = true;

				//move on to next employee
				break;
			}
			else
			{
				//owned properties handled look for new properties if funds sufficient
				if(m_currentFunds > m_MinimumFunds)
				{
					if(m_Allocation.size() > 0)
					{
						++buyingSelling;

						std::pair<AllocationAction, Property> nextAllocation = m_Allocation[0];
						m_Allocation.erase(m_Allocation.begin());

						Property newProperty = nextAllocation.second;
						newProperty.m_AssignedAction = nextAllocation.first;
						m_ownedProperties.push_back(newProperty);
						m_currentFunds -= newProperty.m_ValueToBuy;

						//move on to next employee
						break;
					}
				}
				else
				{
					//There was no work for this employee to perform
					employeesIdle = true;
					++buyingSelling;;
				}
			}
		}
	}

	//All employee have been assigned, pay employees
	m_currentFunds -= (renting * LANDLORD_MONTLY_COST) - (buyingSelling * REALTOR_MONTLY_COST) - (renovating * RENOVATOR_MONTLY_COST);

   //hire new ones if not underminumum funds and no employees have been idle
	if(m_currentFunds > m_MinimumFunds && !employeesIdle && m_currentEmployees < STARTING_EMPLOYEES + m_AdditionalEmployees)
	{
		m_currentEmployees++;
	}
}