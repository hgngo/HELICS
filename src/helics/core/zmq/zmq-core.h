/*

Copyright (C) 2017, Battelle Memorial Institute
All rights reserved.

This software was co-developed by Pacific Northwest National Laboratory, operated by the Battelle Memorial Institute; the National Renewable Energy Laboratory, operated by the Alliance for Sustainable Energy, LLC; and the Lawrence Livermore National Laboratory, operated by Lawrence Livermore National Security, LLC.

*/
#ifndef _HELICS_ZEROMQ_CORE_
#define _HELICS_ZEROMQ_CORE_

#include "helics/config.h"
#include "helics/common/blocking_queue.h"
#include "helics/core/core.h"
#include "helics/core/helics-time.h"

#include <cstdint>
#include <mutex> 
#include <thread> 
#include <utility> 
#include <vector> 

#define CMD_CONNECT "HELLO"
#define CMD_DIE "DIE"
#define CMD_REG_FED "REG_FED"
#define CMD_REG_PUB "REG_PUB"
#define CMD_REG_SUB "REG_SUB"
#define CMD_REG_END "REG_END"
#define CMD_REG_SRC "REG_SRC"
#define CMD_REG_DST "REG_DST"
#define CMD_INIT "INIT"
#define CMD_EXEC "EXECUTE"
#define CMD_STOP "STOP"
#define CMD_TIME "TIME"
#define CMD_TIME_IT "TIME_IT"
#define CMD_SIZE "SIZE"
#define CMD_ACK "ACK"
#define CMD_PUB "PUB"
#define CMD_BYE "BYE"

namespace helics {

class ZeroMQHandle;
class ZeroMQFederateState;

class ZeroMQCore : public Core {

public:

  ZeroMQCore() : Core() {};
  virtual ~ZeroMQCore();
  virtual void initialize (const char *initializationString) override;
          void terminate();
  virtual bool isInitialized () override;
  virtual void error (federate_id_t federateID, int errorCode=-1) override;
  virtual void finalize (federate_id_t federateID) override;
  virtual void enterInitializingState (federate_id_t federateID) override;
  virtual bool enterExecutingState(federate_id_t federateID, bool iterationCompleted = true) override;
  virtual federate_id_t registerFederate (const char *name, const Core::FederateInfo &info) override;
  virtual const char* getFederateName (federate_id_t federateId) override;
  virtual federate_id_t getFederateId (const char *name) override;
  virtual void setFederationSize (unsigned int size);
  virtual unsigned int getFederationSize () override;
  virtual Time timeRequest (federate_id_t federateId, Time next) override;
  virtual std::pair<Time, bool> requestTimeIterative (federate_id_t federateId, Time next, bool localConverged) override;
  virtual uint64_t getCurrentReiteration (federate_id_t federateId) override;
  virtual void setMaximumIterations (federate_id_t federateId, uint64_t iterations) override;
  virtual void setTimeDelta (federate_id_t federateId, Time time) override;
  virtual void setLookAhead(federate_id_t federateId, Time timeLookAhead) override;
  virtual void setImpactWindow(federate_id_t federateId, Time timeImpact) override;
  virtual Handle registerSubscription (federate_id_t federateId, const char *key, const char *type, const char *units, bool required) override;
  virtual Handle getSubscription (federate_id_t federateId, const char *key) override;
  virtual Handle registerPublication (federate_id_t federateId, const char *key, const char *type, const char *units) override;
  virtual Handle getPublication (federate_id_t federateId, const char *key) override;
  virtual const char *getUnits (Handle handle) override;
  virtual const char *getType (Handle handle) override;
  virtual void setValue (Handle handle, const char *data, uint64_t len) override;
  virtual data_t* getValue (Handle handle) override;
  virtual void dereference(data_t *data) override;
  virtual void dereference(message_t *data) override;
  virtual const Handle* getValueUpdates (federate_id_t federateId, uint64_t *size) override;
  virtual Handle registerEndpoint (federate_id_t federateId, const char *name, const char *type) override;
  virtual Handle registerSourceFilter (federate_id_t federateId, const char *filterName, const char *source, const char *type_in) override;
  virtual Handle registerDestinationFilter (federate_id_t federateId, const char *filterName, const char *dest, const char *type_in) override;
  virtual void addDependency(federate_id_t federateId, const char *federateName) override;
  virtual void registerFrequentCommunicationsPair (const char *source, const char *dest) override;
  virtual void send (Handle sourceHandle, const char *destination, const char *data, uint64_t length) override;
  virtual void sendEvent (Time time, Handle sourceHandle, const char *destination, const char *data, uint64_t length) override;
  virtual void sendMessage (message_t *message) override;
  virtual uint64_t receiveCount (Handle destination) override;
  virtual message_t* receive (Handle destination) override;
  virtual std::pair<const Handle, message_t*> receiveAny (federate_id_t federateId) override;
  virtual uint64_t receiveCountAny (federate_id_t federateId) override;
  virtual void logMessage(federate_id_t federateId, int logCode, const char *logMessage) override;
  virtual void setFilterOperator(Handle filter, FilterOperator* callback) override;
/**
* Returns number of messages for all filters.
*/
virtual uint64_t receiveFilterCount(federate_id_t federateID) override;

/**
* Receives a message for any filter.
*/
virtual std::pair<const Handle, message_t*> receiveAnyFilter(federate_id_t federateID) override;
  friend void broker(ZeroMQCore *core);
private:
  unsigned int _max_cores;
  unsigned int _n_cores;
  unsigned int _n_feds;
  std::vector<ZeroMQHandle*> _handles;
  std::vector<Core::Handle> _pending_values;
  bool _initialized;
  std::vector<ZeroMQFederateState*> _federates;
  Time _time_granted;
  uint64_t _iter;
  uint64_t _max_iterations;
  std::thread _thread_broker;
  BlockingQueue<std::string> _queue;
  BlockingQueue<std::string> _queue_response;
  std::mutex _mutex;
  void queueMessage(message_t *message);
  std::string _broker_address;
};

std::string to_string(const Core::FederateInfo &info);
Core::FederateInfo to_info(const std::string &raw);

} // namespace helics
 
#endif /* _HELICS_ZEROMQ_CORE_ */