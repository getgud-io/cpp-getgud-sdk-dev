#pragma once

#include <string>
#include <mutex>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk
{
	/**
	* Logger is responsible for logging everything from SDK to the file
	* specified by client
	**/
	class Logger
	{
	private:
		std::mutex write_mutex; // TODO: what is this?

		unsigned int messages_amount = 0; 
		unsigned int errors_amount = 0; 
		unsigned int warnings_amount = 0;
		unsigned int exceptions_amount = 0; 

	private:
		/**
		* write_to_file:
		* @out_string: file in which to write the output file
		* 
		* Write the whole log string into the output file so it can be later accessed 
		* by client for debugging purposes
		**/
		void write_to_file(std::string out_string);

	public:
		/**
		* write_message:
		* @message: message which will be added to the output file
		* 
		* Write the message log into the file so it can be later accessed by client
		* for debugging purposes
		**/
		void write_message(std::string message);

		/**
		* write_warning:
		* @warning: warning which will be added to the output file
		* 
		* Write the warning log into the file so it can be later accessed by client
		* for debugging purposes
		**/
		void write_warning(std::string warning);

		/**
		* write_error:
		* @error: error which will be added to the output file
		* 
		* Write the error log into the file so it can be later accessed by client
		* for debugging purposes
		**/
		void write_error(std::string error);

		/**
		* write_exception:
		* @exception: exception which will be added to the output file
		* 
		* Write the exception log into the file so it can be later accessed by client
		* for debugging purposes
		**/
		void write_exception(std::string exception);

		/**
		* log:
		* @_type: 
		* @_level:
		* @log_string:
		* 
		* Central logging operation, decides what to log, how to log and where to log
		**/
		void log(LogType _type, LogLevel _level, std::string log_string);
	};
}