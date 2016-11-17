///  RealEstateEA.cpp
///
///	 Uses an evolutionary algorithm to run a real estate investment and 
///  management company.
///

#include "stdafx.h"

#include "Constants.h"
#include "Individual.h"
#include "Property.h"

#include <time.h>
#include <vector>

//Main func declarations
void CalculateMonth(std::vector<Individual>& population);

int main()
{
	//Initialize random seed
	srand(time(NULL));

	//Define algorithm elements
	std::vector<Individual> population;

	//Initialize environment of real estate properties available for individuals to process
	std::vector<Property> environmentProperties;
	environmentProperties.push_back(Property(180000, 200000, 1000, 0, false));
	environmentProperties.push_back(Property(20000, 25000, 300, 0, false));
	environmentProperties.push_back(Property(1800000, 2000000, 10000, 0, false));
	environmentProperties.push_back(Property(5000000, 600000, 100000, 0, false));
	environmentProperties.push_back(Property(135000, 140000, 1000, 0, false));
	environmentProperties.push_back(Property(145000, 146000, 600, 0, false));

	//Build population individuals, random generation handled in Individual contructor
	for(int i = 0; i < 500; i++)
	{
		population.push_back(Individual(environmentProperties));
	}


	//simulate NUM_MONTHS of transactions 
	for (int i = 0; i < NUM_MONTHS; i++)
	{
		CalculateMonth(population);
	}

	return 0;
}


void CalculateMonth(std::vector<Individual>& population)
{
	std::vector<Individual>::iterator individual = population.begin();
	for(individual; individual != population.end(); ++individual)
	{
		individual->
	}
}


