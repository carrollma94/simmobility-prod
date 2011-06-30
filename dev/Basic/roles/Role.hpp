#pragma once

#include "../entities/Agent.hpp"

namespace sim_mob
{

/**
 * Role that a person may fulfill. Allows Person agents to swap out roles easily,
 * without re-creating themselves or maintaining temporarily irrelevant data.
 *
 * \note
 * For now, this class is very simplistic.
 */
class Role {
public:
	//NOTE: We will probably need chained constructors at some point. For now, parent should be set
	// to NULL by the default constructor (I think)
	/*Role() : parent(NULL) {
	}*/

	/// TODO: Think through what kind of data this function might need.
	/// Frame number? Elapsed time?
	virtual void update() = 0;

	void setParent(Agent* parent) {
		this->parent = parent;
	}

protected:
	Agent* parent; ///<The owner of this role. Usually a Person, but I could see it possibly being another Agent.
};



}
