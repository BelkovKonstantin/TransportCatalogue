#pragma once
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <iostream>
#include <string_view>
#include <variant>
#include <optional>
#include <vector>

namespace transport_catalogue
{

	namespace units
	{
		// catalogue units-----------------------------------------

		struct BusInfo
		{
			int all_stops_;
			int unique_stops_;
			double curvature_;
			double route_;
		};

		struct Bus
		{
			std::unordered_map<int, std::string> stops; // Остановки по порядку
			BusInfo info_;
			bool is_roundtrip_;
		};

		// Автобусы, которые проезжают через данную остановку
		struct StopBuses
		{
			std::set<std::string> buses_;
		};

		struct Stop
		{
			std::pair<double, double> ltd_lng; // Широта и долгота
			StopBuses buses;
			std::unordered_map<std::string, int> stop_to_stop; // Расстояние до остановки Х(метров)
		};

		struct BusMapInfo
		{
			std::set<std::string> buses_;
			const std::unordered_map<std::string, Stop> *stops_;
			const std::unordered_map<std::string, Bus> *buses_info_;
		};

		// catalogue units-------------------------------------------

		// json reader units-----------------------------------------

		struct Query
		{
			std::string_view type;
			int id;
			std::variant<std::string, std::pair<std::string, std::string>> query;
		};

		struct QueryBase
		{
			std::vector<Query> querys_;
		};
		// json reader units------------------------------------------

		// transport router units-------------------------------------

		// Хранит уникальные остановки из всех маршрутов
		struct ActualGraphStops
		{
			std::unordered_set<std::string_view> uniq_stops_{};
		};

		// Хранит скорость автобуса(метров в минуту) и время ожидания автобуса(минуты)
		struct RoutingParams
		{
			static double bus_wait_time;
			static double bus_velocity;
		};

		// Хранит "вес" расстояния между остановками
		struct PathTime
		{
			friend PathTime operator+(PathTime lhs, PathTime rhs);
			friend bool operator<(PathTime lhs, PathTime rhs);
			friend bool operator>(PathTime lhs, PathTime rhs);
			friend bool operator==(PathTime lhs, PathTime rhs);

			double time = 0;
			short span_count = 0;
			std::optional<std::string_view> bus;
			bool is_other_bus = false;

			constexpr PathTime() {}
			PathTime(double time_, short span_count_, std::string_view bus_, bool other_bus_) : time(time_), span_count(span_count_), bus(bus_), is_other_bus(other_bus_) {}
			constexpr PathTime(const PathTime &other) : time(other.time), span_count(other.span_count), bus(other.bus), is_other_bus(other.is_other_bus) {}

			PathTime operator=(PathTime other);
		};

		// transport router units-------------------------------------

		// request hundler units--------------------------------------

		// Шаблон для неудачного запроса("в базе нет информации по запросу")
		struct FailedQuery
		{
			std::string id_str = "request_id";
			std::string error_str = "error_message";
			std::string not_str = "not found";
			int id = 0;
		};
		// request hundler units--------------------------------------
	} // namespace units

} // namespace transport_catalogue
