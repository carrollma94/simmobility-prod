//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

/*
 * BusDriverFacets.hpp
 *
 *  Created on: May 16th, 2013
 *      Author: Yao Jin
 */

#pragma once

#include "BusDriver.hpp" 
#include "conf/settings/DisableMPI.h"
#include "entities/roles/passenger/Passenger.hpp"
#include "entities/roles/RoleFacets.hpp"
#include "entities/vehicle/Bus.hpp"


namespace sim_mob {

class BusDriver;

class BusDriverBehavior: public sim_mob::DriverBehavior {
public:
	explicit BusDriverBehavior(sim_mob::Person* parentAgent = nullptr);
	virtual ~BusDriverBehavior();

	//Virtual overrides
	virtual void frame_init();
	virtual void frame_tick();
	virtual std::string frame_tick_output();

	BusDriver* getParentBusDriver() const {
		return parentBusDriver;
	}

	void setParentBusDriver(BusDriver* parentBusDriver) {
		if(!parentBusDriver) {
			throw std::runtime_error("parentBusDriver cannot be NULL");
		}
		this->parentBusDriver = parentBusDriver;
	}

protected:
	BusDriver* parentBusDriver;
};

class BusDriverMovement: public sim_mob::DriverMovement {
public:
	explicit BusDriverMovement(sim_mob::Person* parentAgent = nullptr);
	virtual ~BusDriverMovement();

	//Virtual overrides
	virtual void frame_init();

	virtual void frame_tick();

	virtual std::string frame_tick_output();

	// mark startTimeand origin
	virtual TravelMetric & startTravelTimeMetric() {}

	//	mark the destination and end time and travel time
	virtual TravelMetric & finalizeTravelTimeMetric() {}

	BusDriver* getParentBusDriver() const {
		return parentBusDriver;
	}

	void setParentBusDriver(BusDriver* parentBusDriver) {
		if(!parentBusDriver) {
			throw std::runtime_error("parentBusDriver cannot be NULL");
		}
		this->parentBusDriver = parentBusDriver;
	}

	/// Return the distance (m) to the (next) bus stop.
	/// A negative return value indicates that there is no relevant bus stop nearby.
	double distanceToNextBusStop();

	/// get distance to bus stop of particular segment (meter)
	/// A negative return value indicates that there is no relevant bus stop nearby.
	double getDistanceToBusStopOfSegment(const RoadSegment* rs);

	bool isBusFarawayBusStop();

	bool isBusApproachingBusStop();

	bool isBusArriveBusStop();

	bool isBusLeavingBusStop();

	bool isBusGngtoBreakDown();

	double busAccelerating(DriverUpdateParams& p);

	///here passenger initially chooses which bus lines to board upon reaching bus stop
	///and board the bus when it approaches based on this initial choice
	void BoardingPassengers_Choice(Bus* bus);

	///here passenger makes decision to board bus when bus reaches bus stop
	///if the bus goes to the destination passenger decides to board

	void BoardingPassengers_Normal(Bus* bus);
	// new boarding method
	// individual boarding and alighting(Yao Jin)
	void IndividualBoardingAlighting_New(Bus* bus);

	// determine boarding and alighting MS for possible boarding and alighting persons(Yao Jin)
	void DetermineBoardingAlightingMS(Bus* bus);

	// start boarding and alighting based on the boarding and alighting MS
	// change passenger amount on the Bus(Yao Jin)
	void StartBoardingAlighting(Bus* bus);

	// reset some boarding and alighting variables after leaving the BusStop(Yao Jin)
	void resetBoardingAlightingVariables();

    void AlightingPassengers(Bus* bus);

    ///dwell time calculation module
 	virtual double dwellTimeCalculation(int A,int B,int delta_bay,int delta_full,int Pfront,int no_of_passengers); // dwell time calculation module

 	std::vector<const sim_mob::BusStop*> findBusStopInPath(const std::vector<const sim_mob::RoadSegment*>& path) const;

	// get total waiting time at the BusStop
	double getWaitTime_BusStop() { return busStopWaitTime; }

	// set total waiting time at the BusStop
	void setWaitTime_BusStop(double time) { busStopWaitTime = time; }

	// initialize Bus Path by BusTrip information
	Vehicle* initializePath_bus(bool allocateVehicle);

protected:
	BusDriver* parentBusDriver;

private:

	double lastTickDistanceToBusStop;

	bool demoPassengerIncrease;

	double dwellTimeRecord;// set by BusDriver(temporary), only needed by BusDriver

	//double xpos_approachingbusstop,ypos_approachingbusstop;
	bool firstBusStop;

	bool lastBusStop;

	size_t noPassengersBoarding;

	size_t noPassengersAlighting;

	// flag to indicate whether boarding and alighting is allowed, if it is false, boarding alighting frame is not determined(reset after BusDriver leaves the BusStop)
	bool allowBoardingAlightingFlag;

	// a tempoary boarding queue, will be cleared after BusDriver leaves the BusStop
	std::vector<sim_mob::Person*> virtualBoardingPersons;

	// record the BoardingNumPos map based on the boarding queue in the BusStopAgent, cleared after BusDriver leaves the BusStop
	std::map<int, int> BoardingNumPos;

	// record the AlightingNumPos map based on the passenger queue in the Bus, cleared after BusDriver leaves the BusStop
	std::map<int, int> AlightingNumPos;

	// boardingMSs for possible boarding persons, cleared after leaving the BusStop
	std::vector<uint32_t> boardingMSs;

	// alightingMSs for possible alighting persons, cleared after leaving the BusStop
	std::vector<uint32_t> alightingMSs;

	// the first boarding and alighting MS where bus will start boarding and alighting, reset after leaving the BusStop
	uint32_t firstBoardingAlightingMS;

	// the last boarding and alighting MS and bus will leaves the BusStop, reset after leaving the BusStop
	uint32_t lastBoardingAlightingMS;

	// temporary boardingmsOffset for boarding queue erase purpose, reset after leaving the BusStop
	int boardingmsOffset;

	// temporary alightingmsOffset for passenger queue erase purpose, reset after leaving the BusStop
	int alightingmsOffset;

	// holding time Secs
	double busStopHoldingTimeSec;

	// dwelltime(boarding and alighting time Secs)
	double busStopWaitBoardingAlightingSec;

	// total busStop list
	std::vector<const BusStop*> busStops;

	// waiting MS adding at the BusStop
	double waitAtStopMS;

	// total waiting time Secs(can be holding time or dwell time)
	double busStopWaitTime;
};
}
