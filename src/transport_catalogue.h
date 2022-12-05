#pragma once
#include "geo.h"
#include "domain.h"
#include <map>
#include <string>
#include <deque>
#include <stdexcept>

namespace transport_catalogue
{

	namespace catalogue_base
	{
		using namespace transport_catalogue::units;

		// Класс, хранящий информацию о маршрутах(автобусах и остановках)
		class TransportCatalogue
		{
		public:
			TransportCatalogue() = default;

			void AddData(std::string &&query);
			const std::string &GetBaseBack(std::string &&query) const;

			void AddStops(std::unordered_map<std::string, Stop> &&stops);
			void AddBuses(std::unordered_map<std::string, Bus> &&buses);

			BusInfo BusFind(std::string &bus);
			std::optional<StopBuses> StopFind(std::string &stop);

			// Передаёт данные для визуализации карты
			BusMapInfo GetMapInfo();

		private:
			std::deque<std::string> base_;
			std::unordered_map<std::string, Stop> stops_;
			std::unordered_map<std::string, Bus> buses_;
		};

	}

	namespace detail
	{
		std::string_view Clear(std::string_view query);
	}
}
