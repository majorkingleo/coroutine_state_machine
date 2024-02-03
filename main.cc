/*
 * main.cc
 *
 *  Created on: 03.02.2024
 *      Author: martin
 */
#include "SimTemperatureMod.h"
#include <coroutine>
#include "CoGenerator.h"
#include <iostream>
#include <variant>
#include <thread>
#include <chrono>
#include <math.h>

using namespace std::chrono_literals;

struct MeasurementResult
{

};

struct MeasurementProcessError
{

};

struct MeasurementInProgress
{

};

typedef std::variant<MeasurementResult,
		MeasurementProcessError,
		MeasurementInProgress> MEASUREMENT_RESULT;

CoGenerator<bool> AdjustToTargetTemperature( float target_temperature )
{
	SimTemperatureMod::instance().set(target_temperature);

	for( float current_temp = SimTemperatureMod::instance().get();
			fabs(current_temp - target_temperature) > 0.0001;
			current_temp = SimTemperatureMod::instance().get() )
	{
		co_yield false;
	}

	co_yield true;
}

CoGenerator<bool> FillLiquid()
{
	for( unsigned i = 0; i < 10 ; i++ ) {
		co_yield false;
	}

	co_yield true;
}

CoGenerator<MEASUREMENT_RESULT> StartMeasurementProcess( float target_temperature, bool simple_fil )
{
	if( simple_fil ) {
		auto fill_stuff = FillLiquid();

		bool stuff_filled = false;

		for( stuff_filled = false;
		     fill_stuff && !stuff_filled;
		     co_yield MeasurementInProgress() ) {

			stuff_filled = fill_stuff();

		} // for

		if( !stuff_filled ) {
			co_yield MeasurementProcessError();
			co_return;
		}
	}




	auto adjust_to_target = AdjustToTargetTemperature( target_temperature );

	bool temperature_reached = false;

	for( temperature_reached = false;
		 adjust_to_target && !temperature_reached;
		 co_yield MeasurementInProgress() ) {
		temperature_reached = adjust_to_target();
	}

	if( !temperature_reached ) {
		co_yield MeasurementProcessError();
		co_return;
	}

	co_yield MeasurementResult();
}


int main()
{
	SimTemperatureMod::start();

	auto measurement = StartMeasurementProcess( 2, true );

	MEASUREMENT_RESULT res;

	while( measurement ) {
		res = measurement();

		if( MeasurementInProgress* progress; (progress = std::get_if<MeasurementInProgress>(&res)) != nullptr) {
			std::cout << "measurement in progress. Temp: " << SimTemperatureMod::instance().get() << std::endl;
			std::this_thread::sleep_for(500ms);
		}
	}


	if( MeasurementResult* success; (success = std::get_if<MeasurementResult>(&res)) != nullptr) {
		std::cout << "got a measurement result\n";
	}
	else if( MeasurementProcessError* error; (error = std::get_if<MeasurementProcessError>(&res)) != nullptr ) {
		std::cout << "error returned\n";
	}

	return 0;
}



