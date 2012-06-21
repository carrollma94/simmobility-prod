/* Copyright Singapore-MIT Alliance for Research and Technology */

/*
 * CF_Model.cpp
 *
 *  Created on: 2011-8-15
 *      Author: wangxy & Li Zhemin
 */

#include <boost/random.hpp>

#include <limits>

#include "entities/vehicle/Vehicle.hpp"
#include "CarFollowModel.hpp"
#include "Driver.hpp"

using std::numeric_limits;
using namespace sim_mob;


namespace {
//Random number generator
//TODO: We need a policy on who can get a generator and why.
//boost::mt19937 gen;

//Threshold defaults
const double hBufferUpper			=	  1.6;	 ///< upper threshold of headway
const double hBufferLower			=	  0.8;	 ///< lower threshold of headway

//Set default data for acceleration
const double maxAcceleration = 5.0;   ///< 5m/s*s, might be tunable later
const double normalDeceleration = -maxAcceleration*0.6;
const double maxDeceleration = -maxAcceleration;

//Simple conversion
double feet2Unit(double feet) {
	return feet*0.158;
}

//Simple struct to hold Car Following model parameters
struct CarFollowParam {
	double alpha;
	double beta;
	double gama;
	double lambda;
	double rho;
	double stddev;
};

//Car following parameters for this model.
const CarFollowParam CF_parameters[2] = {
//    alpha   beta    gama    lambda  rho     stddev
	{ 0.0400, 0.7220, 0.2420, 0.6820, 0.6000, 0.8250},
	{-0.0418, 0.0000, 0.1510, 0.6840, 0.6800, 0.8020}
};

const double targetGapAccParm[] = {0.604, 0.385, 0.323, 0.0678, 0.217,
		0.583, -0.596, -0.219, 0.0832, -0.170, 1.478, 0.131, 0.300};

//Acceleration mode
enum ACCEL_MODE {
	AM_VEHICLE = 0,
	AM_PEDESTRIAN = 1,
	AM_TRAFF_LIGHT = 2,
	AM_NONE = 3
};

double uRandom(boost::mt19937& gen)
{
	boost::uniform_int<> dist(0, RAND_MAX);
	long int seed_ = dist(gen);

	const long int M = 2147483647;  // M = modulus (2^31)
	const long int A = 48271;       // A = multiplier (was 16807)
	const long int Q = M / A;
	const long int R = M % A;
	seed_ = A * (seed_ % Q) - R * (seed_ / Q);
	seed_ = (seed_ > 0) ? (seed_) : (seed_ + M);
	return (double)seed_ / (double)M;
}

double nRandom(boost::mt19937& gen, double mean,double stddev)
{
	   double r1 = uRandom(gen), r2 = uRandom(gen);
	   double r = - 2.0 * log(r1);
	   if (r > 0.0) return (mean + stddev * sqrt(r) * sin(2 * 3.1415926 * r2));
	   else return (mean);
}
} //End anon namespace

double speed;//if reaction time disable, speed is current velocity
             //if reaction time enable, speed is perceivedFwdVelocity

double sim_mob::MITSIM_CF_Model::makeAcceleratingDecision(DriverUpdateParams& p, double targetSpeed, double maxLaneSpeed)
{
	speed = p.perceivedFwdVelocity/100;
	distanceToNormalStop(p);
	double acc = maxAcceleration;
	double aA = calcSignalRate(p);
	double aB = calcYieldingRate(p, targetSpeed, maxLaneSpeed);
	double aC = waitExitLaneRate(p);
	double aD = calcAdjacentRate(p);
	double aE = calcBackwardRate(p);
	double aF = calcForwardRate(p);
	double aG = carFollowingRate(p, targetSpeed, maxLaneSpeed, p.nvFwd);
	if(acc > aA) acc = aA;
	if(acc > aB) acc = aB;
	if(acc > aC) acc = aC;
	if(acc > aD) acc = aD;
	if(acc > aE) acc = aE;
	if(acc > aF) acc = aF;
	if(acc > aG) acc = aG;

	return acc;
}

double sim_mob::MITSIM_CF_Model::carFollowingRate(DriverUpdateParams& p, double targetSpeed, double maxLaneSpeed,NearestVehicle& nv)
{
//	p.space = nv.distance/100;
	p.space = p.perceivedDistToFwdCar;
	double res = 0;
	//If we have no space left to move, immediately cut off acceleration.
	if(p.space > 0) {
		if(!nv.exists()) {
			return accOfFreeFlowing(p, targetSpeed, maxLaneSpeed);
		}
		p.v_lead = p.perceivedFwdVelocityOfFwdCar/100;
		p.a_lead = p.perceivedAccelerationOfFwdCar/100;

		double dt	=	p.elapsedSeconds;
		double headway = 0;  //distance/speed
		if (speed == 0) {
			headway = 2 * p.space * 100000;
		} else {
			headway = 2 * p.space / (speed+speed+p.elapsedSeconds*maxAcceleration);
		}
		p.space_star	=	p.space + p.v_lead * dt + 0.5 * p.a_lead * dt * dt;

		if(headway < hBufferLower) {
			res = accOfEmergencyDecelerating(p);
		}
		if(headway > hBufferUpper) {
			res = accOfMixOfCFandFF(p, targetSpeed, maxLaneSpeed);
		}
		if(headway <= hBufferUpper && headway >= hBufferLower) {
			res = accOfCarFollowing(p);
		}
	}
	return res;
}


double sim_mob::MITSIM_CF_Model::calcSignalRate(DriverUpdateParams& p)
{
	double minacc = maxAcceleration;
	double yellowStopHeadway = 1; //1 second
	double minSpeedYellow = 2.2352;//5 mph = 2.2352 m / s
    Signal::TrafficColor color;
    double distanceToTrafficSignal;
    distanceToTrafficSignal = p.perceivedDistToTrafficSignal;
    color = p.perceivedTrafficColor;
	if(distanceToTrafficSignal < 5000)
	{
	double dis = distanceToTrafficSignal/100;
#ifdef NEW_SIGNAL
		if(p.perceivedTrafficColor == sim_mob::Red)
				{
					double a = brakeToStop(p, dis);
					if(a < minacc)
						minacc = a;
				}
				else if(p.perceivedTrafficColor == sim_mob::Amber)
				{
					double maxSpeed = (speed>minSpeedYellow)?speed:minSpeedYellow;
					if(dis/maxSpeed > yellowStopHeadway)
					{
						double a = brakeToStop(p, dis);
						if(a < minacc)
							minacc = a;
					}
				}
				else if(p.perceivedTrafficColor == sim_mob::Green)
				{
					minacc = maxAcceleration;
				}
#else
		if(color == Signal::Red)
		{
			double a = brakeToStop(p, dis);
			if(a < minacc)
				minacc = a;
		}
		else if(color == Signal::Amber)
		{
			double maxSpeed = (speed>minSpeedYellow)?speed:minSpeedYellow;
			if(dis/maxSpeed > yellowStopHeadway)
			{
				double a = brakeToStop(p, dis);
				if(a < minacc)
					minacc = a;
			}
		}
		else if(color == Signal::Green)
		{
			minacc = maxAcceleration;
		}

#endif

	}
	return minacc;
}

double sim_mob::MITSIM_CF_Model::calcYieldingRate(DriverUpdateParams& p, double targetSpeed, double maxLaneSpeed)
{
	if(p.turningDirection == LCS_LEFT)
	{
		return carFollowingRate(p, targetSpeed, maxLaneSpeed, p.nvLeftFwd);
	}
	else if(p.turningDirection == LCS_RIGHT)
	{
		return carFollowingRate(p, targetSpeed, maxLaneSpeed, p.nvRightFwd);
	}
	return maxAcceleration;
}

double sim_mob::MITSIM_CF_Model::waitExitLaneRate(DriverUpdateParams& p)
{
	double dx = p.perceivedDistToFwdCar/100 - 5;
	if(p.turningDirection == LCS_SAME || dx > p.distanceToNormalStop)
		return maxAcceleration;
	else
		return brakeToStop(p, dx);
}

double sim_mob::MITSIM_CF_Model::calcForwardRate(DriverUpdateParams& p)
{
	if(p.turningDirection == LCS_SAME)
		return maxAcceleration;
	NearestVehicle& nv = (p.turningDirection == LCS_LEFT)?p.nvLeftFwd:p.nvRightFwd;
	if(!nv.exists())
		return maxAcceleration;
	double dis = nv.distance/100 + targetGapAccParm[0];
	double dv = nv.driver->fwdVelocity.get()/100 - speed;
	double acc = targetGapAccParm[1] * pow(dis, targetGapAccParm[2]);

	if (dv > 0) {
		acc *= pow(dv, targetGapAccParm[3]);
	} else if (dv < 0) {
		acc *= pow (-dv, targetGapAccParm[4]);
	}
	acc += targetGapAccParm[5] /0.824 ;
	return acc;
}

double sim_mob::MITSIM_CF_Model::calcBackwardRate(DriverUpdateParams& p)
{
	if(p.turningDirection == LCS_SAME)
		return maxAcceleration;
	NearestVehicle& nv = (p.turningDirection == LCS_LEFT)?p.nvLeftFwd:p.nvRightFwd;
	if(!nv.exists())
		return maxAcceleration;

	double dis = nv.distance/100 + targetGapAccParm[0];
	double dv = nv.driver->fwdVelocity.get()/100 - speed;

	double acc = targetGapAccParm[6] * pow(dis, targetGapAccParm[7]);

	if (dv > 0) {
		acc *= pow(dv, targetGapAccParm[8]);
	} else if (dv < 0) {
		acc *= pow (-dv, targetGapAccParm[9]);
	}
	acc += targetGapAccParm[10] / 0.824 ;
	return acc;
}

double sim_mob::MITSIM_CF_Model::calcAdjacentRate(DriverUpdateParams& p)
{
	if(p.nextLaneIndex == p.currLaneIndex)
		return maxAcceleration;
	NearestVehicle& av = (p.nextLaneIndex > p.currLaneIndex)?p.nvLeftFwd:p.nvRightFwd;
	NearestVehicle& bv = (p.nextLaneIndex > p.currLaneIndex)?p.nvLeftBack:p.nvRightBack;
	if(!av.exists())
		return maxAcceleration;
	if(!bv.exists())
		return normalDeceleration;
	double gap = bv.distance/100 + av.distance/100;
	double position = bv.distance/100;
	double acc = targetGapAccParm[11] * (targetGapAccParm[0] * gap - position);

	acc += targetGapAccParm[12] / 0.824 ;
	return acc;
}

double sim_mob::MITSIM_CF_Model::brakeToStop(DriverUpdateParams& p, double dis)
{
	double DIS_EPSILON =	0.001;
	if (dis > DIS_EPSILON) {
		double u2 = speed*speed;
		double acc = - u2 / dis * 0.5;
		if (acc <= normalDeceleration) return acc;
		double dt = p.elapsedSeconds;
		double vt = speed * dt;
		double a = dt * dt;
		double b = 2.0 * vt - normalDeceleration * a;
		double c = u2 + 2.0 * normalDeceleration * (dis - vt);
		double d = b * b - 4.0 * a * c;

		if (d < 0 || a <= 0.0) return acc;

		return (sqrt(d) - b) / a * 0.5;
	}
	else {

		double dt = p.elapsedSeconds;
		return (dt > 0.0) ? -speed / dt : maxDeceleration;
	}
}



double sim_mob::MITSIM_CF_Model::breakToTargetSpeed(DriverUpdateParams& p)
{
	double v 			=	speed;
	double dt			=	p.elapsedSeconds;

	//NOTE: This is the only use of epsilon(), so I just copied the value directly.
	//      See LC_Model for how to declare a private temporary variable. ~Seth
	if(p.space_star > numeric_limits<double>::epsilon()) {
		return  ((p.v_lead + p.a_lead * dt ) * ( p.v_lead + p.a_lead * dt) - v * v) / 2 / p.space_star;
	} else if ( dt <= 0 ) {
		return maxAcceleration;
	} else {
		return ( p.v_lead + p.a_lead * dt - v ) / dt;
	}
}

double sim_mob::MITSIM_CF_Model::accOfEmergencyDecelerating(DriverUpdateParams& p)
{
	double v 			=	speed;
	double dv			=	v-p.v_lead;
	double epsilon_v	=	0.001;
	double aNormalDec	=	normalDeceleration;

	double a;
	if( dv < epsilon_v ) {
		a = p.a_lead + 0.25*aNormalDec;
	} else if (p.space > 0.01 ) {
		a = p.a_lead - dv * dv / 2 / p.space;
	} else {
		a= breakToTargetSpeed(p);
	}
//	if(a<maxDeceleration)
//		return maxDeceleration;
//	else if(a>maxAcceleration)
//		return maxAcceleration;
//	else
		return a;
}



double sim_mob::MITSIM_CF_Model::accOfCarFollowing(DriverUpdateParams& p)
{
	const double density	=	1;		//represent the density of vehicles in front of the subject vehicle
										//now we ignore it, assuming that it is 1.
	double v				=	speed;
	int i = (v > p.v_lead) ? 1 : 0;
	double dv =(v > p.v_lead)?(v-p.v_lead):(p.v_lead - v);

	double res = CF_parameters[i].alpha * pow(v , CF_parameters[i].beta) /pow(p.nvFwd.distance/100 , CF_parameters[i].gama);
	res *= pow(dv , CF_parameters[i].lambda)*pow(density,CF_parameters[i].rho);
	res += feet2Unit(nRandom(p.gen, 0, CF_parameters[i].stddev));

	return res;
}

double sim_mob::MITSIM_CF_Model::accOfFreeFlowing(DriverUpdateParams& p, double targetSpeed, double maxLaneSpeed)
{
	double vn =	speed;
	if (vn < targetSpeed) {
		return (vn<maxLaneSpeed) ? maxAcceleration : normalDeceleration;
	} else if (vn > targetSpeed) {
		return normalDeceleration;
	}

	//If equal:
	return (vn<maxLaneSpeed) ? maxAcceleration: 0;
}

double sim_mob::MITSIM_CF_Model::accOfMixOfCFandFF(DriverUpdateParams& p, double targetSpeed, double maxLaneSpeed)
{
	if(p.space > p.distanceToNormalStop ) {
		return accOfFreeFlowing(p, targetSpeed, maxLaneSpeed);
	} else {
		return breakToTargetSpeed(p);
	}
}

void sim_mob::MITSIM_CF_Model::distanceToNormalStop(DriverUpdateParams& p)
{
	double minSpeed = 0.1;
	double minResponseDistance = 5;
	double DIS_EPSILON = 0.001;
	if (speed > minSpeed) {
		p.distanceToNormalStop = DIS_EPSILON -
				0.5 * speed * speed / normalDeceleration;
		if (p.distanceToNormalStop < minResponseDistance) {
			p.distanceToNormalStop = minResponseDistance;
		}
	} else {
		p.distanceToNormalStop = minResponseDistance;
	}
}
