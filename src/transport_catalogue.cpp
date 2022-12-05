#include "transport_catalogue.h"
#include <iomanip>

namespace transport_catalogue
{

	namespace catalogue_base
	{

		void TransportCatalogue::AddData(std::string &&query)
		{
			base_.push_back(std::move(query));
		}

		const std::string &TransportCatalogue::GetBaseBack(std::string &&query) const
		{
			return base_.back();
		}

		void TransportCatalogue::AddStops(std::unordered_map<std::string, Stop> &&stops)
		{
			stops_ = std::move(stops);
		}

		void TransportCatalogue::AddBuses(std::unordered_map<std::string, Bus> &&buses)
		{
			buses_ = std::move(buses);
		}

		BusInfo TransportCatalogue::BusFind(std::string &bus)
		{
			if (auto bus_info = buses_.find(bus); bus_info == buses_.end() || bus_info->second.stops.size() == 0)
			{
				return {};
			}
			else
				return bus_info->second.info_;
		}

		std::optional<StopBuses> TransportCatalogue::StopFind(std::string &stop)
		{
			if (auto buses = stops_.find(stop); buses == stops_.end())
			{
				return std::nullopt;
			}
			else
				return buses->second.buses;
		}

		BusMapInfo TransportCatalogue::GetMapInfo()
		{
			BusMapInfo map_base_{{}, &stops_, &buses_};
			for (const auto [bus, bus_info] : buses_)
			{
				map_base_.buses_.insert(bus);
			}
			return map_base_;
		}
	}
	namespace detail
	{
		std::string_view Clear(std::string_view query)
		{
			std::string_view result = query;
			while (result.front() == ' ' || result.back() == ' ')
			{
				if (result.front() == ' ')
				{
					result.remove_prefix(1);
				}
				if (result.back() == ' ')
				{
					result.remove_suffix(1);
				}
			}
			return result;
		}
	}

}
