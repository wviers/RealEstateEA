///  RealEstateEA.cpp
///
///	 Uses an evolutionary algorithm to run a real estate investment and 
///  management company.
///

#include "stdafx.h"

#include "Constants.h"
#include "Individual.h"
#include "Property.h"

#include <algorithm>
#include <assert.h>
#include <random>
#include <vector>

//Main func declarations
void CalculateMonth(std::vector<Individual>& population);
void EvolvePopulation(std::vector<Individual>& population, std::mt19937& generator);
int GenerateRandom(int from, int to, std::mt19937& generator);
void InitializeEnvironmentProperties(std::vector<Property>& environmentProperties);

std::random_device rand_dev;

int main()
{
	//Define algorithm elements
	std::vector<Individual> population;
	std::vector<Property> environmentProperties;

	InitializeEnvironmentProperties(environmentProperties);

	//Build population individuals, random generation handled in Individual contructor
	for(int i = 0; i < NUM_INDIVIDUALS; i++)
	{
		population.push_back(Individual(environmentProperties, std::mt19937(rand_dev())));
	}

	bool solutionFound = false;
	int count = 0;
	while(!solutionFound)
	{
		//simulate NUM_MONTHS of transactions 
		for (int i = 0; i < NUM_MONTHS; i++)
		{
			CalculateMonth(population);
		}

		//Calc fitness for all individuals
		for(int i = 0; i < NUM_INDIVIDUALS; i++)
		{
			population[i].CalculateFitness();
		}

		std::sort(population.begin(), population.end());

		if(abs(population[0].m_Fitness - population[NUM_INDIVIDUALS - 1].m_Fitness) < STARTING_FUNDS)
		{
			solutionFound = true;
			break;
		}

		//evolve pop
		EvolvePopulation(population, std::mt19937(rand_dev()));

		//Initialize all the individuals for the next run
		for(int i = 0; i < NUM_INDIVIDUALS; i++)
		{
			population[i].Initialize();
		}

		++count;
	}

	return 0;
}


void CalculateMonth(std::vector<Individual>& population)
{
	std::vector<Individual>::iterator individual = population.begin();
	for(individual; individual != population.end(); ++individual)
	{
		individual->CalculateMonth(std::mt19937(rand_dev()));
	}
}


void EvolvePopulation(std::vector<Individual>& population, std::mt19937& generator)
{
	//cut the first lower half of the population
	population.erase(population.begin(), population.begin() + NUM_INDIVIDUALS / 2);

	for(int i = 0; i < NUM_INDIVIDUALS / 2; ++i)
	{
		EvolutionAction action = (EvolutionAction) GenerateRandom((int)MUTATE, (int)SHUFFLE, generator);

		//choose which element of the allocation will be mutated
		int randomElement = GenerateRandom((int)MIN_FUNDS, (int)ALLOCATION, generator);
		int randomIndex = GenerateRandom(0, population[i].m_Allocation.size() - 1, generator);

		if(action == MUTATE)
		{
			if(randomElement == MIN_FUNDS)
			{
				population.push_back(Individual(GenerateRandom(0, STARTING_FUNDS, generator), population[i].m_AdditionalEmployees, population[i].m_Allocation));
			}
			else if(randomElement == ADDITIONAL_EMPLOYEES)
			{
				population.push_back(Individual(population[i].m_MinimumFunds, GenerateRandom(0, 10, generator), population[i].m_Allocation));
			}
			else
			{
				std::vector<std::pair<AllocationAction, Property>> tempAllocation = population[i].m_Allocation;
			   tempAllocation[randomIndex].first = (AllocationAction)GenerateRandom((int)RENT, (int)RENOVATE, generator);
				population.push_back(Individual(population[i].m_MinimumFunds, population[i].m_AdditionalEmployees, tempAllocation));
			}
		}
		else if(action == SWAP)
		{
			if(randomElement == MIN_FUNDS)
			{
				if(i > 0)
				{
					//Insert a new Individual with a mix of two Individuals allocations
					population.push_back(Individual(population[i - 1].m_MinimumFunds, population[i].m_AdditionalEmployees, population[i].m_Allocation));
				}
				else
				{
					//Insert a new Individual with a mix of two Individuals allocations
					population.push_back(Individual(population[i + 1].m_MinimumFunds, population[i].m_AdditionalEmployees, population[i].m_Allocation));
				}
			}
			else if(randomElement == ADDITIONAL_EMPLOYEES)
			{
				if(i > 0)
				{
					//Insert a new Individual with a mix of two Individuals allocations
					population.push_back(Individual(population[i].m_MinimumFunds, population[i - 1].m_AdditionalEmployees, population[i].m_Allocation));
				}
				else
				{
					//Insert a new Individual with a mix of two Individuals allocations
					population.push_back(Individual(population[i].m_MinimumFunds, population[i + 1].m_AdditionalEmployees, population[i].m_Allocation));
				}
			}
			else
			{
				//Swap an allocation between 2 properties in the individual
				int randomIndex2 = GenerateRandom(0, population[i].m_Allocation.size() - 1, generator);
				std::vector<std::pair<AllocationAction, Property>> temp = population[i].m_Allocation;
				AllocationAction tempAction = population[i].m_Allocation[randomIndex].first;
				temp[randomIndex].first = temp[randomIndex2].first;
				temp[randomIndex2].first = tempAction;
				population.push_back(Individual(population[i].m_MinimumFunds, population[i].m_AdditionalEmployees, temp));
			}
		}
		else
		{
			std::vector<std::pair<AllocationAction, Property>> tempToBeShuffled = population[i].m_Allocation;
			std::shuffle(tempToBeShuffled.begin(), tempToBeShuffled.end(), generator);
			population.push_back(Individual(population[i].m_MinimumFunds, population[i].m_AdditionalEmployees, tempToBeShuffled));
		}
	}
}


int GenerateRandom(int from, int to, std::mt19937& generator)
{
	std::uniform_int_distribution<int> distr(from, to);

	return distr(generator);
}


void InitializeEnvironmentProperties(std::vector<Property>& environmentProperties)
{
	//Initialize environment of real estate properties available for individuals to process
	
	//sell  //rent //buy //renovated //rented

	//double a sufficiently varied environment
	for(int i = 0; i < 2; ++i)
	{
		//Good to sell not to rent
		environmentProperties.push_back(Property(20000, 25000, 300, 19000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(53000, 55000, 400, 50000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(70000, 90000, 500, 69000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(80000, 100000, 600, 78000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(180000, 200000, 800, 170000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(80000, 100000, 600, 78000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(180000, 200000, 800, 170000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(280000, 290000, 1600, 240000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(780000, 830000, 2900, 750000, GOOD_SELL_BAD_RENT));
		environmentProperties.push_back(Property(1040000, 1230000, 3100, 900000, GOOD_SELL_BAD_RENT));

		//Great to renovate (large difference between min sell and max sell)
		environmentProperties.push_back(Property(20000, 45000, 200, 19000, GREAT_RENOVATE));
		environmentProperties.push_back(Property(50000, 77000, 400, 46000, GREAT_RENOVATE));
		environmentProperties.push_back(Property(100000, 140000, 670, 99000, GREAT_RENOVATE));
		environmentProperties.push_back(Property(50000, 77000, 400, 46000, GREAT_RENOVATE));
		environmentProperties.push_back(Property(100000, 140000, 670, 99000, GREAT_RENOVATE));
		environmentProperties.push_back(Property(240000, 290000, 1100, 240000, GREAT_RENOVATE));
		environmentProperties.push_back(Property(480000, 530000, 1900, 480000, GREAT_RENOVATE));
		environmentProperties.push_back(Property(1500000, 2000000, 4000, 1500000, GREAT_RENOVATE));

		//High value avg risk
		environmentProperties.push_back(Property(10000, 70000, 100, 40000, RISKY_SELL));
		environmentProperties.push_back(Property(40000, 90000, 750, 60000, RISKY_SELL));
		environmentProperties.push_back(Property(120000, 270000, 1000, 190000, RISKY_SELL));
		environmentProperties.push_back(Property(420000, 670000, 6000, 540000, RISKY_SELL));

		//Great to rent bad to sell
		environmentProperties.push_back(Property(50000, 60000, 6000, 60000, GREAT_RENT_BAD_SELL));
		environmentProperties.push_back(Property(100000, 150000, 15000, 150000, GREAT_RENT_BAD_SELL));
		environmentProperties.push_back(Property(250000, 270000, 30000, 270000, GREAT_RENT_BAD_SELL));
		environmentProperties.push_back(Property(500000, 600000, 75000, 600000, GREAT_RENT_BAD_SELL));

		//Good Deal for selling and renting
		environmentProperties.push_back(Property(32000, 47000, 1000, 26000, GOOD_DEAL_BOTH));
		environmentProperties.push_back(Property(135000, 140000, 1900, 100000, GOOD_DEAL_BOTH));
		environmentProperties.push_back(Property(175000, 200000, 2300, 150000, GOOD_DEAL_BOTH));
		environmentProperties.push_back(Property(345000, 359000, 3800, 340000, GOOD_DEAL_BOTH));
		environmentProperties.push_back(Property(445000, 487000, 5400, 420000, GOOD_DEAL_BOTH));

		//bad deal
		environmentProperties.push_back(Property(45000, 46000, 200, 47000, BAD_DEAL));
		environmentProperties.push_back(Property(65000, 76000, 200, 76000, BAD_DEAL));
		environmentProperties.push_back(Property(90000, 100000, 200, 100000, BAD_DEAL));
		environmentProperties.push_back(Property(116000, 117000, 600, 117000, BAD_DEAL));
		environmentProperties.push_back(Property(125000, 136000, 600, 136000, BAD_DEAL));
		environmentProperties.push_back(Property(145000, 146000, 600, 147000, BAD_DEAL));
		environmentProperties.push_back(Property(235200, 237000, 800, 237000, BAD_DEAL));

		//38 properties
	}
}


