#include "STK.hpp"

#include <sstream>
#include <boost/assign/list_of.hpp>
#include <json/json.h>

#include "conf/ConfigManager.hpp"
#include "conf/ConfigParams.hpp"
#include "workers/Worker.hpp"

#include "entities/commsim/broker/base/Common.hpp"
#include "entities/commsim/comm_support/AgentCommUtility.hpp"
#include "entities/commsim/connection/ConnectionServer.hpp"
#include "entities/commsim/connection/ConnectionHandler.hpp"
#include "entities/commsim/event/subscribers/base/ClientHandler.hpp"
#include "entities/commsim/event/RegionsAndPathEventArgs.hpp"

#include "entities/commsim/client/derived/android/AndroidClientRegistration.hpp"
#include "entities/commsim/client/derived/ns3/NS3ClientRegistration.hpp"

#include "geospatial/RoadRunnerRegion.hpp"

//todo :temprorary
#include "entities/commsim/message/derived/gen-android/AndroidFactory.hpp"
#include "entities/commsim/message/derived/gen-ns3/Ns3Factory.hpp"

#include "entities/commsim/wait/WaitForAndroidConnection.hpp"
#include "entities/commsim/wait/WaitForNS3Connection.hpp"
#include "entities/commsim/wait/WaitForAgentRegistration.hpp"

sim_mob::STK_Broker::STK_Broker(const MutexStrategy& mtxStrat, int id ,std::string commElement_, std::string commMode_) :
Broker(mtxStrat, id, commElement_, commMode_)
{
//	//Various Initializations
//	connection.reset(new ConnectionServer(*this));
//	brokerCanTickForward = false;
//	m_messageReceiveCallback = boost::function<void(boost::shared_ptr<ConnectionHandler>, std::string)>
//		(boost::bind(&Broker::messageReceiveCallback,this, _1, _2));
//
////	Print() << "Creating Roadrunner Broker [" << this << "]" << std::endl;
////	configure(); //already done at the time of creation
}

void sim_mob::STK_Broker::configure()
{
	//Only configure once.
	if (!configured_.check()) {
		return;
	}

	const std::string & client_mode = ConfigManager::GetInstance().FullConfig().getCommSimMode(commElement);
//	ConfigManager::GetInstance().FullConfig().isCommSimEnabled("stk");
	sim_mob::Worker::GetUpdatePublisher().subscribe((event::EventId)sim_mob::event::EVT_CORE_AGENT_UPDATED, this, &STK_Broker::onAgentUpdate, (event::EventId)sim_mob::event::CXT_CORE_AGENT_UPDATE);

	BrokerPublisher* onlyLocationsPublisher = new BrokerPublisher();
	onlyLocationsPublisher->registerEvent(COMMEID_LOCATION);

	publishers.insert(std::make_pair(
		sim_mob::Services::SIMMOB_SRV_LOCATION,
		PublisherList::Value(onlyLocationsPublisher))
	);

	//NS-3 has its own publishers
	if(client_mode == "android-ns3") {
		BrokerPublisher* allLocationsPublisher = new BrokerPublisher();
		allLocationsPublisher->registerEvent(COMMEID_LOCATION);
		publishers.insert(std::make_pair(
			sim_mob::Services::SIMMOB_SRV_ALL_LOCATIONS,
			PublisherList::Value(allLocationsPublisher))
		);
	}

	BrokerPublisher* timePublisher = new BrokerPublisher();
	timePublisher->registerEvent(COMMEID_TIME);
	publishers.insert(std::make_pair(
		sim_mob::Services::SIMMOB_SRV_TIME,
		PublisherList::Value(timePublisher))
	);

	//current message factory
	//todo: choose a factory based on configurations not hardcoding
	if(client_mode == "android-ns3") {
		boost::shared_ptr<sim_mob::MessageFactory<std::vector<sim_mob::comm::MsgPtr>, std::string> >
			android_factory(new sim_mob::comm::AndroidFactory(true));
		boost::shared_ptr<sim_mob::MessageFactory<std::vector<sim_mob::comm::MsgPtr>, std::string> >
			ns3_factory(new sim_mob::comm::NS3_Factory());

		//note that both client types refer to the same message factory belonging to stk application. we will modify this to a more generic approach later-vahid
		messageFactories.insert(std::make_pair(comm::ANDROID_EMULATOR, android_factory));
		messageFactories.insert(std::make_pair(comm::NS3_SIMULATOR, ns3_factory));
	} else if (client_mode == "android-only") {
		boost::shared_ptr<sim_mob::MessageFactory<std::vector<sim_mob::comm::MsgPtr>, std::string> >
			android_factory(new sim_mob::comm::AndroidFactory(false));

		//note that both client types refer to the same message factory belonging to stk application. we will modify this to a more generic approach later-vahid
		messageFactories.insert(std::make_pair(comm::ANDROID_EMULATOR, android_factory));
	}

	// wait for connection criteria for this broker
	clientBlockers.insert(std::make_pair(comm::ANDROID_EMULATOR,
			boost::shared_ptr<WaitForAndroidConnection>(new WaitForAndroidConnection(*this,MIN_CLIENTS))));


	if(client_mode == "android-ns3") {
		clientBlockers.insert(std::make_pair(comm::NS3_SIMULATOR,
				boost::shared_ptr<WaitForNS3Connection>(new WaitForNS3Connection(*this))));

	}

	// wait for connection criteria for this broker
	agentBlockers.insert(std::make_pair(0,
			boost::shared_ptr<WaitForAgentRegistration>(new WaitForAgentRegistration(*this,MIN_AGENTS))));

	//	client registration handlers
	//	Also listen to publishers who announce registration of new clients...
	ClientRegistrationHandlerMap[comm::ANDROID_EMULATOR].reset(new sim_mob::AndroidClientRegistration());
	registrationPublisher.registerEvent(comm::ANDROID_EMULATOR);
//	(sim_mob::event::EventId id, sim_mob::event::Context context, sim_mob::event::EventPublisher* sender, const UpdateEventArgs& argums)
	registrationPublisher.subscribe((event::EventId)comm::ANDROID_EMULATOR, this, &STK_Broker::onClientRegister);

	if(client_mode == "android-ns3") {
		ClientRegistrationHandlerMap[comm::NS3_SIMULATOR].reset(new sim_mob::NS3ClientRegistration());
		registrationPublisher.registerEvent(comm::NS3_SIMULATOR);
		//listen to publishers who announce registration of new clients...
		registrationPublisher.subscribe((event::EventId)comm::NS3_SIMULATOR, this, &STK_Broker::onClientRegister);
	}
}



void sim_mob::STK_Broker::onAgentUpdate(sim_mob::event::EventId id, sim_mob::event::Context context, sim_mob::event::EventPublisher* sender, const UpdateEventArgs& argums)
{
	Broker::onAgentUpdate(id,context,sender,argums);
}

void sim_mob::STK_Broker::onClientRegister(sim_mob::event::EventId id, sim_mob::event::Context context, sim_mob::event::EventPublisher* sender, const ClientRegistrationEventArgs& argums)
{
	Broker::onClientRegister(id,context,sender,argums);
}
