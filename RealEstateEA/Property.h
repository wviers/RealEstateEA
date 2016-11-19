///  Property.h
///
///	 Defines a peice of real estate in terms of the simualtion 
///

#pragma once

enum AllocationAction
{
	UNASSIGNED = -1,
	RENT,
	BUY,
	RENOVATE
};


enum DataDesignation
{
	STARTER,
	GOOD_SELL_BAD_RENT,
	GREAT_RENOVATE,
	RISKY_SELL,
	GREAT_RENT_BAD_SELL,
	GOOD_DEAL_BOTH,
	BAD_DEAL
};


class Property
{
public:
	Property() : m_MinValueToSell(0), m_MaxValueToSell(0), m_ValueForRent(0), m_ValueToBuy(0), 
		m_Renovated(false), m_AssignedAction(UNASSIGNED) {}
	~Property() {}
   Property(int minSell, int maxSell, int rent, int buy, DataDesignation enumVal) : m_MinValueToSell(minSell),
		m_MaxValueToSell(maxSell), m_ValueForRent(rent), m_ValueToBuy(buy), m_Renovated(false), m_desig(enumVal)
	{m_AssignedAction = UNASSIGNED;}

	int m_MinValueToSell;
	int m_MaxValueToSell;
	int m_ValueForRent;
	int m_ValueToBuy;
	bool m_Renovated;
	AllocationAction m_AssignedAction;
	DataDesignation m_desig;
};