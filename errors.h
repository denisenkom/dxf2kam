#include <stdexcept>
#include "opcodes.h"

namespace Kamea
{
	class parse_error : public std::runtime_error {
	public:
		unsigned cmd_indx;
		parse_error(const char *desc, unsigned cmd_indx)
			: std::runtime_error(desc), cmd_indx(cmd_indx)
		{
		}
	};

	class invalid_command_id : public parse_error
	{
	public:
		unsigned cmd_id;
		invalid_command_id(const char *desc, unsigned cmd_indx, unsigned cmd_id)
			: parse_error(desc, cmd_indx), cmd_id(cmd_id)
		{
		}

		/*virtual const char * what(void)
		{
			std::stringstream str;
			str << "#" << (cmd_indx+1) << ": Неправильный код команды: " << std::setbase(16) << cmd_id;
			return str.str().c_str();
		}*/
	};

	class invalid_command_length : public parse_error
	{
	public:
		ECmdId cmd_id;
		size_t cmd_length;
		invalid_command_length(const char *desc, unsigned cmd_indx, ECmdId cmd_id, size_t cmd_length)
			: parse_error(desc, cmd_indx), cmd_id(cmd_id), cmd_length(cmd_length)
		{
		}

		/*virtual const char * what(void)
		{
			std::stringstream str;
			str << "#" << (cmd_indx+1) << ": " << text_names_rus[cmd_id] << ": Неверная длина команды: " << cmd_length;
			return str.str().c_str();
		}*/
	};

	class invalid_command_format : public parse_error
	{
	public:
		ECmdId cmd_id;

		invalid_command_format(const char *desc, unsigned cmd_indx, ECmdId cmd_id)
			: parse_error(desc, cmd_indx), cmd_id(cmd_id)
		{
		}

		/*virtual const char * what(void)
		{
			std::stringstream str;
			str << "#" << (cmd_indx+1) << ": " << text_names_rus[cmd_id] << ": Неправильный формат команды";
			return str.str().c_str();
		}*/
	};

	class invalid_speed_value : public parse_error
	{
	public:
		ECmdId cmd_id;
		int spd;

		invalid_speed_value(const char *desc, unsigned cmd_indx, ECmdId cmd_id, int spd)
			: parse_error(desc, cmd_indx), cmd_id(cmd_id), spd(spd)
		{
		}

		/*virtual const char * what(void)
		{
			std::stringstream str;
			str << "#" << (cmd_indx+1) << ": " << text_names_rus[cmd_id] << ": Неверное значение скорости: " << spd;
			return str.str().c_str();
		}*/
	};

	class invalid_device : public parse_error
	{
	public:
		ECmdId cmd_id;
		unsigned device;

		invalid_device(const char *desc, unsigned cmd_indx, ECmdId cmd_id, unsigned device)
			: parse_error(desc, cmd_indx), cmd_id(cmd_id), device(device)
		{
		}

		/*virtual const char * what(void)
		{
			std::stringstream str;
			str << "#" << (cmd_indx+1) << ": " << text_names_rus[cmd_id] << ": Неверное устройство: " << device;
			return str.str().c_str();
		}*/
	};
}