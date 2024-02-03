/*
 * SimTemperatureMod.cc
 *
 *  Created on: 03.02.2024
 *      Author: martin
 */
#include "SimTemperatureMod.h"
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

void SimTemperatureMod::run()
{
	for( ;; std::this_thread::sleep_for(500ms) )
	{
		if( current_temp < target_temp ) {
			current_temp += 0.2;
		} else if( current_temp > target_temp ) {
			current_temp -= 0.2;
		}
	}
}

SimTemperatureMod & SimTemperatureMod::instance()
{
	static SimTemperatureMod sim;

	return sim;
}

void SimTemperatureMod::start()
{
	auto t = std::thread( []() {
		SimTemperatureMod::instance().run();
	} );

	t.detach();
}
