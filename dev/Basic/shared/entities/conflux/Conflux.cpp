/*
 * Conflux.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: harish
 */

#include<map>
#include "Conflux.hpp"
using namespace sim_mob;
typedef Entity::UpdateStatus UpdateStatus;

void sim_mob::Conflux::addAgent(sim_mob::Agent* ag) {
	/**
	 * The agents always start at a node (for now).
	 * we will always add the agent to the road segment in "lane infinity".
	 */
	sim_mob::SegmentStats* rdSegStats = segmentAgents.at(ag->getCurrSegment());
	ag->setCurrLane(rdSegStats->laneInfinity);
	ag->distanceToEndOfSegment = ag->getCurrSegment()->computeLaneZeroLength();
	rdSegStats->addAgent(rdSegStats->laneInfinity, ag);
}

UpdateStatus sim_mob::Conflux::update(timeslice frameNumber) {
	currFrameNumber = frameNumber;

	if (sim_mob::StreetDirectory::instance().signalAt(*multiNode) != nullptr) {
		updateUnsignalized(frameNumber); //TODO: Update Signalized must be implemented
	}
	else {
		updateUnsignalized(frameNumber);
	}
	updateSupplyStats(frameNumber);
	UpdateStatus retVal(UpdateStatus::RS_CONTINUE); //always return continue. Confluxes never die.
	return retVal;
}

void sim_mob::Conflux::updateSignalized() {
	throw std::runtime_error("Conflux::updateSignalized() not implemented yet.");
}

void sim_mob::Conflux::updateUnsignalized(timeslice frameNumber) {
	initCandidateAgents();
	sim_mob::Agent* ag = agentClosestToIntersection();
	while (ag) {
		updateAgent(ag);
		// get next agent to update
		ag = agentClosestToIntersection();
	}
}

void sim_mob::Conflux::updateAgent(sim_mob::Agent* ag) {
	const sim_mob::RoadSegment* segBeforeUpdate = ag->getCurrSegment();
	const sim_mob::Lane* laneBeforeUpdate = ag->getCurrLane();

	UpdateStatus res = ag->update(currFrameNumber);
	if (res.status == UpdateStatus::RS_DONE) {
		//This agent is done. Remove from simulation. TODO: Check if this is valid for trip chaining. ~ Harish
		killAgent(ag, segBeforeUpdate, laneBeforeUpdate);
		return;
	} else if (res.status == UpdateStatus::RS_CONTINUE) {
		// TODO: I think there will be nothing here. Have to make sure. ~ Harish
	} else {
		throw std::runtime_error("Unknown/unexpected update() return status.");
	}

	const sim_mob::RoadSegment* segAfterUpdate = ag->getCurrSegment();
	const sim_mob::Lane* laneAfterUpdate = ag->getCurrLane();

	if((segBeforeUpdate != segAfterUpdate) || (laneBeforeUpdate == segmentAgents[segBeforeUpdate]->laneInfinity && laneBeforeUpdate != laneAfterUpdate))
	{
		segmentAgents[segBeforeUpdate]->dequeue(laneBeforeUpdate);

		if(segmentAgents.find(segAfterUpdate) != segmentAgents.end()) {
			if(laneAfterUpdate) {
				segmentAgents[segAfterUpdate]->addAgent(laneAfterUpdate, ag);
			}
			else {
				// If we don't know which lane the agent has to go to, we add him to lane infinity.
				// NOTE: One possible scenario for this is an agent who is starting on a new trip chain item.
				addAgent(ag);
			}
		}
		else if (segmentAgentsDownstream.find(segAfterUpdate) != segmentAgentsDownstream.end()) {
			if(laneAfterUpdate) {
				segmentAgentsDownstream[segAfterUpdate]->addAgent(laneAfterUpdate, ag);
			}
			else {
				// If we don't know which lane the agent has to go to, we add him to lane infinity.
				// NOTE: One possible scenario for this is an agent who is starting on a new trip chain item.
				sim_mob::SegmentStats* downStreamSegStats = segmentAgentsDownstream[segAfterUpdate];
				ag->setCurrLane(downStreamSegStats->laneInfinity);
				ag->distanceToEndOfSegment = ag->getCurrSegment()->computeLaneZeroLength();
				downStreamSegStats->addAgent(downStreamSegStats->laneInfinity, ag);
			}
		}
	}
}

double sim_mob::Conflux::getSegmentSpeed(const RoadSegment* rdSeg, bool hasVehicle){
	if (hasVehicle)
		return getSegmentAgents()[rdSeg]->getSegSpeed(hasVehicle);
	else
		return getSegmentAgents()[rdSeg]->getSegSpeed(hasVehicle);
}

void sim_mob::Conflux::initCandidateAgents() {
	resetCurrSegsOnUpLinks();
	typedef std::map<sim_mob::Link*, const sim_mob::RoadSegment* >::iterator currSegsOnUpLinksIt;
	sim_mob::Link* lnk = nullptr;
	const sim_mob::RoadSegment* rdSeg = nullptr;
	for (currSegsOnUpLinksIt i = currSegsOnUpLinks.begin(); i != currSegsOnUpLinks.end(); i++) {
		lnk = i->first;
		while (currSegsOnUpLinks[lnk]) {
			rdSeg = currSegsOnUpLinks[lnk];
			if(rdSeg == 0){
				throw std::runtime_error("Road Segment NULL");
			}
			segmentAgents[rdSeg]->resetFrontalAgents();
			candidateAgents[rdSeg] = segmentAgents[rdSeg]->getNext();
			if(!candidateAgents[rdSeg]) {
				// this road segment is deserted. search the next (which is, technically, the previous).
				const std::vector<sim_mob::RoadSegment*> segments = upstreamSegmentsMap.at(lnk);
				std::vector<sim_mob::RoadSegment*>::const_iterator rdSegIt = std::find(segments.begin(), segments.end(), rdSeg);
				if(rdSegIt != segments.begin()) {
					rdSegIt--;
					currSegsOnUpLinks.erase(lnk);
					currSegsOnUpLinks[lnk] = *rdSegIt;
				}
				else {
					currSegsOnUpLinks.erase(lnk);
					currSegsOnUpLinks[lnk] = nullptr;
				}
			}
			else { break; }
		}
	}
}

std::map<const sim_mob::Lane*, std::pair<unsigned int, unsigned int> > sim_mob::Conflux::getLanewiseAgentCounts(const sim_mob::RoadSegment* rdSeg) {
	return segmentAgents[rdSeg]->getAgentCountsOnLanes();
}

unsigned int sim_mob::Conflux::numMovingInSegment(const sim_mob::RoadSegment* rdSeg, bool hasVehicle) {
	return segmentAgents[rdSeg]->numMovingInSegment(hasVehicle);
}

void sim_mob::Conflux::resetCurrSegsOnUpLinks() {
	currSegsOnUpLinks.clear();
	for(std::map<sim_mob::Link*, const std::vector<sim_mob::RoadSegment*> >::iterator i = upstreamSegmentsMap.begin();
			i != upstreamSegmentsMap.end(); i++) {
		currSegsOnUpLinks.insert(std::make_pair(i->first, i->second.back()));
	}
}

sim_mob::Agent* sim_mob::Conflux::agentClosestToIntersection() {
	std::map<const sim_mob::RoadSegment*, sim_mob::Agent* >::iterator i = candidateAgents.begin();
	sim_mob::Agent* ag = nullptr;
	const sim_mob::RoadSegment* agRdSeg = nullptr;
	double minDistance = std::numeric_limits<double>::max();
	while(i != candidateAgents.end()) {
		if(i->second != nullptr) {
			if(minDistance == (i->second->distanceToEndOfSegment + lengthsOfSegmentsAhead[i->first])) {
				// If current ag and (*i) are at equal distance to the stop line, we toss a coin and choose one of them
				bool coinTossResult = ((rand() / (double)RAND_MAX) < 0.5);
				if(coinTossResult) {
					agRdSeg = i->first;
					ag = i->second;
				}
			}
			else if (minDistance > (i->second->distanceToEndOfSegment + lengthsOfSegmentsAhead[i->first])) {
				minDistance = i->second->distanceToEndOfSegment + lengthsOfSegmentsAhead[i->first];
				agRdSeg = i->first;
				ag = i->second;
			}
		}
		i++;
	}
	if(ag) {
		candidateAgents.erase(agRdSeg);
		const std::vector<sim_mob::RoadSegment*> segments = agRdSeg->getLink()->getSegments();
		sim_mob::Agent* nextAg = segmentAgents[agRdSeg]->getNext();
		std::vector<sim_mob::RoadSegment*>::const_iterator rdSegIt = std::find(segments.begin(), segments.end(), agRdSeg);
		while(!nextAg && rdSegIt != segments.begin()){
			rdSegIt--;
			nextAg = segmentAgents[*rdSegIt]->getNext();
		}
		candidateAgents[agRdSeg] = nextAg;
	}
	return ag;
}

void sim_mob::Conflux::prepareLengthsOfSegmentsAhead() {
	for(std::map<sim_mob::Link*, const std::vector<sim_mob::RoadSegment*> >::iterator i = upstreamSegmentsMap.begin();
				i != upstreamSegmentsMap.end(); i++)
	{
		for(std::vector<sim_mob::RoadSegment*>::const_iterator j = i->second.begin();
				j != i->second.end(); j++)
		{
			double lengthAhead = 0.0;
			for(std::vector<sim_mob::RoadSegment*>::const_iterator k = j+1;
					k != i->second.end(); k++)
			{
				lengthAhead = lengthAhead + (*k)->computeLaneZeroLength();
			}
			lengthsOfSegmentsAhead.insert(std::make_pair(*j, lengthAhead));
		}
	}
}

unsigned int sim_mob::Conflux::numQueueingInSegment(const sim_mob::RoadSegment* rdSeg,
		bool hasVehicle) {
	std::cout << "rdSeg in Conflux: "<<rdSeg <<std::endl;
	return segmentAgents[rdSeg]->numQueueingInSegment(hasVehicle);
}

double sim_mob::Conflux::getOutputFlowRate(const Lane* lane) {
	return segmentAgents[lane->getRoadSegment()]->getLaneParams(lane)->getOutputFlowRate();
}

int sim_mob::Conflux::getOutputCounter(const Lane* lane) {
	return segmentAgents[lane->getRoadSegment()]->getLaneParams(lane)->getOutputCounter();
}

void sim_mob::Conflux::absorbAgentsAndUpdateCounts(sim_mob::SegmentStats* sourceSegStats) {
	if(segmentAgents.find(sourceSegStats->getRoadSegment()) != segmentAgents.end()
			&& sourceSegStats->hasAgents()) {
		segmentAgents[sourceSegStats->getRoadSegment()]->absorbAgents(sourceSegStats);
		std::map<const sim_mob::Lane*, std::pair<unsigned int, unsigned int> > laneCounts = segmentAgents[sourceSegStats->getRoadSegment()]->getAgentCountsOnLanes();
		sourceSegStats->setPrevTickLaneCountsFromOriginal(laneCounts);
		sourceSegStats->clear();
	}
}

double sim_mob::Conflux::getAcceptRate(const Lane* lane) {
	return segmentAgents[lane->getRoadSegment()]->getLaneParams(lane)->getAcceptRate();
}

void sim_mob::Conflux::updateSupplyStats(const Lane* lane, double newOutputFlowRate) {
	segmentAgents[lane->getRoadSegment()]->updateLaneParams(lane, newOutputFlowRate);
}

void sim_mob::Conflux::restoreSupplyStats(const Lane* lane) {
	segmentAgents[lane->getRoadSegment()]->restoreLaneParams(lane);
}

void sim_mob::Conflux::updateSupplyStats(timeslice frameNumber) {
	std::map<const sim_mob::RoadSegment*, sim_mob::SegmentStats*>::iterator it = segmentAgents.begin();
	for( ; it != segmentAgents.end(); ++it )
	{
		(it->second)->updateLaneParams(frameNumber);
		(it->second)->reportSegmentStats(frameNumber);
	}
}

std::pair<unsigned int, unsigned int> sim_mob::Conflux::getLaneAgentCounts(
		const sim_mob::Lane* lane) {
	return segmentAgents[lane->getRoadSegment()]->getLaneAgentCounts(lane);
}

unsigned int sim_mob::Conflux::getInitialQueueCount(const Lane* lane) {
	return segmentAgents[lane->getRoadSegment()]->getInitialQueueCount(lane);
}

void sim_mob::Conflux::killAgent(sim_mob::Agent* ag, const sim_mob::RoadSegment* prevRdSeg, const sim_mob::Lane* prevLane) {
	segmentAgents[prevRdSeg]->removeAgent(prevLane, ag);
}

void sim_mob::Conflux::handoverDownstreamAgents() {
	for(std::map<const sim_mob::RoadSegment*, sim_mob::SegmentStats*>::iterator i = segmentAgentsDownstream.begin();
			i != segmentAgentsDownstream.end(); i++)
	{
		i->first->getParentConflux()->absorbAgentsAndUpdateCounts(i->second);
	}
}

double sim_mob::Conflux::getLastAccept(const Lane* lane) {
	return segmentAgents[lane->getRoadSegment()]->getLaneParams(lane)->getLastAccept();
}

void sim_mob::Conflux::setLastAccept(const Lane* lane, double lastAcceptTime) {
	segmentAgents[lane->getRoadSegment()]->getLaneParams(lane)->setLastAccept(lastAcceptTime);
}