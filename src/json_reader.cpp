#include "json_reader.h"

namespace transport_catalogue
{
	// Проверка наличия пути между остановками
	void CheckRouteExist(std::unordered_map<std::string, Stop> &stops, std::string &from, std::string &to)
	{
		if (auto from_exist = stops.find(from); from_exist == stops.end())
		{
			stops[from].stop_to_stop[to] = stops[to].stop_to_stop[from];
		}
		else
		{
			if (auto to_stop_route = from_exist->second.stop_to_stop.find(to); to_stop_route == from_exist->second.stop_to_stop.end())
				stops[from].stop_to_stop[to] =
					stops[to].stop_to_stop[from];
		}
	}

	void ParseRouteInfo(std::unordered_map<std::string, Bus> &all_bus, std::unordered_map<std::string, Stop> &stops,
						const std::string &bus, std::optional<graph::TransportRouter<PathTime> *> transport_router_)
	{

		int all_stops_bus;
		std::unordered_set<std::string_view> uniq_stops_bus;
		double route = 0;
		double curvature = 0;
		std::pair<double, double> prev_stop;
		std::pair<double, double> cur_stop;
		std::string prev_stop_name = "";
		std::string cur_stop_name = "";
		size_t bus_stops_count = all_bus.at(bus).stops.size();

		for (size_t i = 1; i < bus_stops_count + 1; ++i)
		{
			std::string stop = all_bus.at(bus).stops.at(i);
			uniq_stops_bus.insert(all_bus.at(bus).stops.at(i));

			if (i > 1)
			{
				cur_stop = stops.at(stop).ltd_lng;
				cur_stop_name = stop;
				route += geo::ComputeDistance({prev_stop.first, prev_stop.second},
											  {cur_stop.first, cur_stop.second});
				CheckRouteExist(stops, prev_stop_name, cur_stop_name);
				auto stops_route = stops[prev_stop_name].stop_to_stop[cur_stop_name];
				std::cout << " from " << prev_stop_name << " to " << cur_stop_name << " =>" << stops_route << " and it on map=>"
						  << geo::ComputeDistance({prev_stop.first, prev_stop.second}, {cur_stop.first, cur_stop.second}) << std::endl;
				curvature += stops_route;
				if (transport_router_)
				{
					short span_count = 1;
					(*transport_router_)->AddRoute(prev_stop_name, cur_stop_name, stops_route, bus, span_count);

					for (size_t j = i + 1; j < bus_stops_count + 1; ++j)
					{
						if (j == bus_stops_count && all_bus.at(bus).is_roundtrip_ && i == 2)
							continue;
						std::string stop_through = all_bus.at(bus).stops.at(j - 1);
						std::string stop_to = all_bus.at(bus).stops.at(j);
						CheckRouteExist(stops, stop_through, stop_to);
						stops_route += stops[stop_through].stop_to_stop[stop_to];
						++span_count;
						(*transport_router_)->AddRoute(prev_stop_name, stop_to, stops_route, bus, span_count);
					}
				}
			}

			prev_stop = stops.at(stop).ltd_lng;
			prev_stop_name = stop;
		}

		if (!all_bus.at(bus).is_roundtrip_)
		{
			all_stops_bus = bus_stops_count * 2 - 1;

			for (int i = bus_stops_count - 1; i > 0; --i)
			{
				std::string stop = std::string{all_bus.at(bus).stops.at(i)};
				cur_stop = stops.at(stop).ltd_lng;
				cur_stop_name = stop;
				route += geo::ComputeDistance({prev_stop.first, prev_stop.second},
											  {cur_stop.first, cur_stop.second});
				CheckRouteExist(stops, prev_stop_name, cur_stop_name);
				auto stops_route = stops[prev_stop_name].stop_to_stop[cur_stop_name];
				curvature += stops_route;
				if (transport_router_)
				{
					short span_count = 1;
					(*transport_router_)->AddRoute(prev_stop_name, cur_stop_name, stops_route, bus, span_count);

					for (size_t j = i - 1; j > 0; --j)
					{
						std::string stop_through = std::string{all_bus.at(bus).stops.at(j + 1)};
						std::string stop_to = std::string{all_bus.at(bus).stops.at(j)};
						CheckRouteExist(stops, stop_through, stop_to);
						stops_route += stops[stop_through].stop_to_stop[stop_to];
						++span_count;
						(*transport_router_)->AddRoute(prev_stop_name, stop_to, stops_route, bus, span_count);
					}
				}
				prev_stop = cur_stop;
				prev_stop_name = cur_stop_name;
			}
		}
		else
		{
			all_stops_bus = bus_stops_count;
		}

		all_bus[bus].info_.all_stops_ = all_stops_bus;
		all_bus[bus].info_.curvature_ = curvature;
		all_bus[bus].info_.route_ = static_cast<double>(curvature) / route;
		all_bus[bus].info_.unique_stops_ = uniq_stops_bus.size();
	}

	namespace json_reader
	{

		using namespace renderer;
		JSONReader::JSONReader(std::istream &input, catalogue_base::TransportCatalogue &catalog)
		{
			json_ = new json::Node(json::Load(input).GetRoot());
			SaveRouterSetting(*json_);
			if (RoutingParams::bus_velocity > 0)
				transport_router_ = new graph::TransportRouter<PathTime>();
			FillingTransportBase(*json_, catalog);
			FillingQueryBase(*json_);
			CreateMapRender(*json_);
			if (transport_router_)
				(*transport_router_)->BuildRouter();
		}

		void JSONReader::FillingTransportBase(json::Node &json_doc, catalogue_base::TransportCatalogue &catalog)
		{
			std::unordered_map<std::string, catalogue_base::Stop> stops;
			std::unordered_map<std::string, catalogue_base::Bus> all_bus;
			for (auto base : json_doc.AsDict().at("base_requests").AsArray())
			{
				if (base.AsDict().at("type").AsString() == "Stop")
					AddStop(stops, base);
				else
					AddBus(stops, all_bus, base);
			}
			if (transport_router_)
				(*transport_router_)->BuildGraph();
			for (const auto &[bus, businfo] : all_bus)
			{
				ParseRouteInfo(all_bus, stops, bus, transport_router_);
			}
			catalog.AddBuses(move(all_bus));
			catalog.AddStops(move(stops));
		}

		void JSONReader::AddStop(std::unordered_map<std::string, catalogue_base::Stop> &stops, json::Node &json_doc)
		{
			std::string stop_;

			transport_base.push_back(json_doc.AsDict().at("name").AsString());
			stop_ = transport_base.back();
			stops[stop_].ltd_lng.first = json_doc.AsDict().at("latitude").AsDouble();
			stops[stop_].ltd_lng.second = json_doc.AsDict().at("longitude").AsDouble();
			for (auto [stop, route] : json_doc.AsDict().at("road_distances").AsDict())
			{
				transport_base.push_back(stop);
				stops[stop_].stop_to_stop[transport_base.back()] = route.AsInt();
			}
		}

		void JSONReader::AddBus(std::unordered_map<std::string, catalogue_base::Stop> &stops, std::unordered_map<std::string, catalogue_base::Bus> &all_bus, json::Node &json)
		{
			catalogue_base::Bus bus_info;
			std::string bus_;
			transport_base.push_back(json.AsDict().at("name").AsString());
			bus_ = transport_base.back();
			bus_info.is_roundtrip_ = json.AsDict().at("is_roundtrip").AsBool();

			int count_st = 1;
			for (auto stop : json.AsDict().at("stops").AsArray())
			{
				transport_base.push_back(stop.AsString());
				bus_info.stops[count_st] = transport_base.back();
				stops[transport_base.back()].buses.buses_.insert(bus_);
				++count_st;
				if (transport_router_)
					(*transport_router_)->InsertStop(transport_base.back());
			}

			all_bus[bus_] = std::move(bus_info);
		}

		void JSONReader::FillingQueryBase(json::Node &json)
		{
			for (auto query : json.AsDict().at("stat_requests").AsArray())
			{
				std::string_view type = query.AsDict().at("type").AsString();
				if (type == "Stop")
					base_.querys_.push_back(
						{type_query_[0], query.AsDict().at("id").AsInt(), query.AsDict().at("name").AsString()});
				else if (type == "Bus")
					base_.querys_.push_back(
						{type_query_[1], query.AsDict().at("id").AsInt(), query.AsDict().at("name").AsString()});
				else if (type == "Route")
					base_.querys_.push_back(
						{type_query_[2], query.AsDict().at("id").AsInt(), std::make_pair(query.AsDict().at("from").AsString(), query.AsDict().at("to").AsString())});
				else
					base_.querys_.push_back(
						{type_query_[3], query.AsDict().at("id").AsInt(), ""});
			}
		}

		void JSONReader::SaveRouterSetting(json::Node &json)
		{
			if (auto json_rout_set = json.AsDict().find("routing_settings"); json_rout_set != json.AsDict().end())
			{
				RoutingParams::bus_wait_time = static_cast<double>(json_rout_set->second.AsDict().at("bus_wait_time").AsInt());
				RoutingParams::bus_velocity = static_cast<double>(json_rout_set->second.AsDict().at("bus_velocity").AsInt()) * 1000.0 / 60.0;
			}
		}

		void JSONReader::CreateMapRender(json::Node &json)
		{
			if (auto json_map_set = json.AsDict().find("render_settings"); json_map_set != json.AsDict().end())
			{
				MapSetting map_info;

				map_info.map_size_.x =
					json_map_set->second.AsDict().at("width").AsDouble();
				map_info.map_size_.y =
					json_map_set->second.AsDict().at("height").AsDouble();
				map_info.padding_ = json_map_set->second.AsDict().at("padding").AsDouble();
				map_info.stop_radius_ =
					json_map_set->second.AsDict().at("stop_radius").AsDouble();
				map_info.line_width_ =
					json_map_set->second.AsDict().at("line_width").AsDouble();
				map_info.bus_text_.SetFontSize(
									  json_map_set->second.AsDict().at("bus_label_font_size").AsInt())
					.SetOffset(
						{json_map_set->second.AsDict().at("bus_label_offset").AsArray()[0].AsDouble(),
						 json_map_set->second.AsDict().at("bus_label_offset").AsArray()[1].AsDouble()});
				map_info.stop_text_.SetFontSize(
									   json_map_set->second.AsDict().at("stop_label_font_size").AsInt())
					.SetOffset(
						{json_map_set->second.AsDict().at("stop_label_offset").AsArray()[0].AsDouble(),
						 json_map_set->second.AsDict().at("stop_label_offset").AsArray()[1].AsDouble()});

				if (json_map_set->second.AsDict().at("underlayer_color").IsString())
				{
					map_info.underlayer_color_ = json_map_set->second.AsDict().at("underlayer_color").AsString();
				}
				else
				{
					if (json_map_set->second.AsDict().at("underlayer_color").AsArray().size() == 3)
						map_info.underlayer_color_ = Rgb{
							static_cast<uint8_t>(json_map_set->second.AsDict().at("underlayer_color").AsArray()[0].AsInt()),
							static_cast<uint8_t>(json_map_set->second.AsDict().at("underlayer_color").AsArray()[1].AsInt()),
							static_cast<uint8_t>(json_map_set->second.AsDict().at("underlayer_color").AsArray()[2].AsInt())};
					else
						map_info.underlayer_color_ =
							Rgba{static_cast<uint8_t>(json_map_set->second.AsDict().at("underlayer_color").AsArray()[0].AsInt()),
								 static_cast<uint8_t>(json_map_set->second.AsDict().at("underlayer_color").AsArray()[1].AsInt()),
								 static_cast<uint8_t>(json_map_set->second.AsDict().at("underlayer_color").AsArray()[2].AsInt()),
								 json_map_set->second.AsDict().at("underlayer_color").AsArray()[3].AsDouble()};
				}
				map_info.underlayer_width_ = json_map_set->second.AsDict().at("underlayer_width").AsDouble();

				for (auto color : json_map_set->second.AsDict().at("color_palette").AsArray())
				{
					if (color.IsString())
					{
						map_info.color_palette_.push_back(color.AsString());
					}
					else
					{
						auto color_arr = color.AsArray();
						if (color_arr.size() == 3)
							map_info.color_palette_.push_back(Rgb{
								static_cast<uint8_t>(color_arr[0].AsInt()),
								static_cast<uint8_t>(color_arr[1].AsInt()),
								static_cast<uint8_t>(color_arr[2].AsInt())});
						else
							map_info.color_palette_.push_back(
								Rgba{static_cast<uint8_t>(color_arr[0].AsInt()),
									 static_cast<uint8_t>(color_arr[1].AsInt()),
									 static_cast<uint8_t>(color_arr[2].AsInt()),
									 color_arr[3].AsDouble()});
					}
				}

				MapRenderer *temp = new MapRenderer(std::move(map_info));
				renderer_.emplace(temp);
			}
		}

		QueryBase &&JSONReader::GetQueryBase()
		{
			return std::move(base_);
		}

		renderer::MapRenderer *JSONReader::GetMapRender()
		{
			return renderer_.value();
		}

		std::deque<std::string> &JSONReader::GetBase()
		{
			return transport_base;
		}

		std::optional<graph::TransportRouter<PathTime> *> JSONReader::GetTransportRouter()
		{
			return transport_router_;
		}

		std::deque<std::string>::iterator JSONReader::begin()
		{
			return transport_base.begin();
		}

		std::deque<std::string>::iterator JSONReader::end()
		{
			return transport_base.end();
		}

		size_t JSONReader::size()
		{
			return transport_base.size();
		}

	} // namespace json_reader
} // namespace transport_catalogue
