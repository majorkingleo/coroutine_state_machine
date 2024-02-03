/*
 * SimTemperatureMod.h
 *
 *  Created on: 03.02.2024
 *      Author: martin
 */

#ifndef SIMTEMPERATUREMOD_H_
#define SIMTEMPERATUREMOD_H_

#include <mutex>

class SimTemperatureMod
{
	std::mutex m_data;
	float target_temp {0};
	float current_temp {0};

public:

	void set( float new_target_temp ) {
		auto lock = std::scoped_lock( m_data );
		target_temp = new_target_temp;
	}

	float get() {
		auto lock = std::scoped_lock( m_data );
		return current_temp;
	}

	static SimTemperatureMod & instance();

	/* start thread */
	static void start();

private:
	void run();
};



#endif /* SIMTEMPERATUREMOD_H_ */
