/*

Copyright (C) 2017, Battelle Memorial Institute
All rights reserved.

This software was co-developed by Pacific Northwest National Laboratory, operated by the Battelle Memorial Institute; the National Renewable Energy Laboratory, operated by the Alliance for Sustainable Energy, LLC; and the Lawrence Livermore National Laboratory, operated by Lawrence Livermore National Security, LLC.

*/
#ifndef ASYNC_FED_CALL_INFO_H_
#define ASYNC_FED_CALL_INFO_H_
#pragma once
#include "core/helics-time.h"
#include <future>
/** helper class for Federate info that holds the futures for async calls*/
class asyncFedCallInfo
{
public:
	std::future<void> initFuture; /**future for the init call*/
	std::future<bool> execFuture; /** future for the enter execution mode call*/
	std::future<helics::Time> timeRequestFuture; /** future for the timeRequest call*/
	std::future<std::pair<helics::Time, bool>> timeRequestIterativeFuture; /** future for the time request iterative call*/
};
#endif