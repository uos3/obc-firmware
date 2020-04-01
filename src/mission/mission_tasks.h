/*
	Most tasks are dependant on the timers for interrupt. Exception is
	process_gs_signal, which uses the GPIO interrupt provided by RX tranciever
	(I believe).
*/


/*
	Checks the integrity of the 'configuation file' and the software image.
*/
void task_health_check(void);
/*
	requests data from the EPS and stores it in the buffer via the
	application layer
*/
void task_save_eps_health(void);

/*
	requests data from the GNSS module and saves it to the buffer, if
	there is a response
*/
void task_save_gps_data(void);

/*
	requests data from the IMU and saves it to the buffer
*/
void task_save_attitude(void);

/*
	DL-exclusive task that checks if the transmitter is ready; if yes,
	transmits the packets. Basically a repeat of the init function sans
	the config stuff.
*/
void task_transmit_buffer_data(void);

/*
	BU-exclusive task that changes mode to AD if A not D'd and NF/LP
	otherwise
*/
void task_exit_bu(void);

/*
	transmits the "most recent telemetry" CW packet. 
*/
void task_transmit_morse_telemetry(void);

/*
	transmits the "most recent telemetry" M/FSK (downlink modulation) packet
	using the packet format.
*/
void task_transmit_packet_telemetry(void);

/*
	AD-exclusive task that deploys the antenna. Does not switch mode, that's
	done by the RX interrupt for "process_gs_signal"
*/
void task_deploy_antenna(void);

/*
	PT-exclusive mode that takes a picture and stores it in the buffer
*/
void task_take_picture(void);

/*
	GPIO-interrupt fired task that is in all modes; read & stores the 
	packets incoming from the RX and then sets about processing &
	executing them.
*/
void task_process_gs_signal(void);

