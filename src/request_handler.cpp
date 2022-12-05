#include "request_handler.h"

namespace transport_catalogue
{

	namespace base_reader
	{

		RequestHandler::RequestHandler(std::istream &input, catalogue_base::TransportCatalogue &catalog, std::ostream &out) : catalog_(&catalog)
		{
			char c;
			input >> c;
			if (c == '{')
			{
				input.putback(c);
				json_reader::JSONReader temp(input, catalog);
				renderer_.emplace(&*(temp.GetMapRender()));
				if (units::RoutingParams::bus_velocity > 0)
				{
					transport_router_ = temp.GetTransportRouter();
				}
				ExecuteJsonQueries(temp.GetQueryBase(), out);
			}
			else
			{
				input.putback(c);
				std::string num;
				std::getline(input, num, input.widen('\n'));
				int num_query = stoi(num);
				for (int i = 0; i < num_query; ++i)
				{
					std::string query;
					std::getline(input, query, input.widen('\n'));
					transport_base.push_back(std::move(query));
				}
				ParseInputData();
				ReadInputQueries(input, out);
			}
		}

		void RequestHandler::ParseInputData()
		{
			std::unordered_map<std::string, Stop> stops;
			std::unordered_map<std::string, Bus> all_bus;

			for (std::string &query : transport_base)
			{
				auto it = query.find_first_not_of(' ');
				if (query[it] == 'S')
				{
					ParseStopData(stops, query);
				}
				else
				{
					ParseBusData(stops, all_bus, query);
				}
			}
			for (const auto &[bus, businfo] : all_bus)
			{
				ParseRouteInfo(all_bus, stops, bus, transport_router_);
			}

			catalog_->AddBuses(move(all_bus));
			catalog_->AddStops(move(stops));
		}

		void RequestHandler::ParseStopData(std::unordered_map<std::string, catalogue_base::Stop> &stops, std::string &query)
		{
			auto it_start_name = query.find_first_not_of(' ', query.find_first_of(' ', query.find_first_not_of(' ')));
			auto it_end_name = query.find_first_of(" :", it_start_name);
			std::string stop_ = query.substr(it_start_name, it_end_name - it_start_name);

			auto it = query.find_first_not_of(' ', query.find(':') + 1);
			auto it_2 = query.find_first_not_of(' ', query.find(',') + 1);
			auto it_3 = query.find_first_of(',', it_2 + 1);

			double stop_ltd = std::stod(query.substr(it, query.find_first_of(" ,", it)));

			if (it_3 != std::string::npos)
			{
				double stop_lng = std::stod(query.substr(it_2, query.find_first_of(" ,", it_2)));
				stops[stop_].ltd_lng = {stop_ltd, stop_lng};
				size_t it_1;

				while (it_2 != std::string::npos)
				{
					it_3 = query.find_first_not_of(' ', it_3 + 1);
					it_1 = query.find_first_of('m', it_3);
					int route_to_stop = std::stoi(query.substr(it_3, it_1 - it_3));

					it_1 = query.find_first_not_of(' ', query.find_first_of("to", it_1) + 2);
					it_3 = query.find_first_of(" ,", it_1);
					std::string next_stop = query.substr(it_1, it_3 - it_1);

					stops[stop_].stop_to_stop[next_stop] = route_to_stop;
					if (auto stop_exist = stops.find(next_stop); stop_exist == stops.end())
					{
						stops[next_stop].stop_to_stop[stop_] = route_to_stop;
					}
					else
					{
						if (auto route_exist = stop_exist->second.stop_to_stop.find(stop_); route_exist == stop_exist->second.stop_to_stop.end())
							stops[next_stop].stop_to_stop[stop_] = route_to_stop;
					}
					it_2 = query.find_first_of(',', it_3);
					it_3 = it_2;
				}
			}
			else
			{
				double stop_lng = std::stod(query.substr(it_2, query.find_first_of(' ', it_2)));
				stops[stop_].ltd_lng = {stop_ltd, stop_lng};
			}
		}

		void RequestHandler::ParseBusData(std::unordered_map<std::string, catalogue_base::Stop> &stops,
										  std::unordered_map<std::string, catalogue_base::Bus> &all_bus, std::string &query)
		{
			auto it_start_name = query.find_first_not_of(' ', query.find_first_of(' ', query.find_first_not_of(' ')));
			auto it_end_name = query.find_first_of(" :", it_start_name);
			auto it_1 = query.find(':');
			char type_path;
			std::string bus = query.substr(it_start_name, it_end_name - it_start_name);

			if (query.find('-') != std::string::npos)
			{
				all_bus[bus].is_roundtrip_ = false;
				type_path = '-';
			}
			else
			{
				all_bus[bus].is_roundtrip_ = true;
				type_path = '>';
			}

			int count = 1;
			std::string bad_chars = " " + type_path;
			it_1 = query.find_first_not_of(' ', it_1 + 1);
			auto it_2 = query.find_first_of(' ', it_1);

			while (it_2 != std::string::npos)
			{

				std::string stop = query.substr(it_1, it_2 - it_1);
				all_bus[bus].stops[count] = stop;
				stops[stop].buses.buses_.insert(bus);

				++count;
				it_1 = query.find_first_not_of(' ', query.find_first_of(type_path, it_2) + 1);
				it_2 = query.find_first_of(' ', it_1);
			}

			std::string stop = query.substr(it_1, it_2 - it_1);
			all_bus[bus].stops[count] = stop;
			stops[stop].buses.buses_.insert(bus);
		}

		void RequestHandler::ExecuteJsonStopQuery(std::string &stop, json::Builder &out_json)
		{
			std::optional<units::StopBuses> buses;
			buses = catalog_->StopFind(stop);
			if (!buses)
			{
				out_json.StartDict().Key(fq.id_str).Value(fq.id).Key(fq.error_str).Value(fq.not_str).EndDict();
			}
			else if ((*buses).buses_.size() == 0)
			{
				out_json.StartDict().Key("buses").StartArray().EndArray().Key(fq.id_str).Value(fq.id).EndDict();
			}
			else
			{
				std::vector<std::string> buses_;
				for (auto bus : (*buses).buses_)
				{
					buses_.push_back(bus.data());
				}
				out_json.StartDict().Key("buses").Value(json::Array{buses_.begin(), buses_.end()}).Key(fq.id_str).Value(fq.id).EndDict();
			}
		}

		void RequestHandler::ExecuteJsonBusQuery(std::string &bus, json::Builder &out_json)
		{
			units::BusInfo bus_info;
			bus_info = catalog_->BusFind(bus);
			if (bus_info.all_stops_ == 0)
			{
				out_json.StartDict().Key(fq.id_str).Value(fq.id).Key(fq.error_str).Value(fq.not_str).EndDict();
			}
			else
			{
				out_json.StartDict().Key("curvature").Value(bus_info.route_).Key(fq.id_str).Value(fq.id).Key("route_length").Value(bus_info.curvature_).Key("stop_count").Value(bus_info.all_stops_).Key("unique_stop_count").Value(bus_info.unique_stops_).EndDict();
			}
		}

		void RequestHandler::ExecuteJsonRouteQuery(std::pair<std::string, std::string> &from_to, json::Builder &out_json)
		{
			auto [from_id, to_id] = (*transport_router_)->GetStopId(from_to.first, from_to.second);
			if (from_id > -1 && to_id > -1)
			{
				auto route_info = (*transport_router_)->BuildRoute(from_id, to_id);
				if (route_info)
				{
					out_json.StartDict().Key("items").StartArray();
					for (auto i = 0; i < route_info.value().edges.size(); ++i)
					{
						auto edge = (*transport_router_)->GetEdge(route_info.value().edges[i]);
						out_json.StartDict().Key("stop_name").Value((*transport_router_)->GetStopName(edge.from).data()).Key("time").Value(RoutingParams::bus_wait_time).Key("type").Value("Wait").EndDict();
						out_json.StartDict().Key("bus").Value((*edge.weight.bus).data()).Key("span_count").Value(edge.weight.span_count).Key("time").Value(edge.weight.time - static_cast<double>(RoutingParams::bus_wait_time)).Key("type").Value("Bus").EndDict();
					}
					out_json.EndArray().Key(fq.id_str).Value(fq.id).Key("total_time").Value(route_info.value().weight.time).EndDict();
				}
				else
				{
					out_json.StartDict().Key(fq.id_str).Value(fq.id).Key(fq.error_str).Value(fq.not_str).EndDict();
				}
			}
			else
			{
				out_json.StartDict().Key(fq.id_str).Value(fq.id).Key(fq.error_str).Value(fq.not_str).EndDict();
			}
		}

		void RequestHandler::ExecuteJsonMapQuery(json::Builder &out_json)
		{
			std::stringstream svg_out = renderer_.value()->DrawMap(catalog_->GetMapInfo());
			out_json.StartDict().Key("map").Value(svg_out.str()).Key(fq.id_str).Value(fq.id).EndDict();
		}

		void RequestHandler::ExecuteJsonQueries(json_reader::QueryBase &&querys_base, std::ostream &out)
		{
			json::Builder out_json{};
			json_reader::QueryBase querys_base_(std::move(querys_base));

			out << std::setprecision(6);
			out_json.StartArray();
			for (auto [type, id, query] : querys_base_.querys_)
			{
				fq.id = id;
				if (type == "Stop")
				{
					ExecuteJsonStopQuery(std::get<std::string>(query), out_json);
				}
				else if (type == "Bus")
				{
					ExecuteJsonBusQuery(std::get<std::string>(query), out_json);
				}
				else if (type == "Route")
				{
					ExecuteJsonRouteQuery(std::get<std::pair<std::string, std::string>>(query), out_json);
				}
				else
				{
					ExecuteJsonMapQuery(out_json);
				}
			}
			out_json.EndArray();
			json::Print(json::Document{out_json.Build()}, out);
		}

		void RequestHandler::ReadInputQueries(std::istream &input, std::ostream &out)
		{
			std::string num_querys;
			std::getline(input, num_querys, input.widen('\n'));
			int num_query = std::stoi(num_querys);
			for (int i = 0; i < num_query; ++i)
			{
				std::string query;
				std::getline(input, query, input.widen('\n'));
				query_base.push_back(std::move(query));
			}
			ExecuteQueries(out);
		}

		void RequestHandler::ExecuteQueries(std::ostream &out)
		{
			size_t b_size = query_base.size();

			for (size_t i = 0; i < b_size; ++i)
			{
				auto it_start_name = query_base[i].find_first_not_of(' ', query_base[i].find_first_of(' ', query_base[i].find_first_not_of(' ')));
				if (query_base[i][query_base[i].find_first_not_of(' ')] == 'B')
				{
					auto bus = std::string{detail::Clear(query_base[i].substr(it_start_name))};
					auto result = catalog_->BusFind(bus);
					if (result.all_stops_ > 0)
					{
						out << "Bus " << bus << ": " << result.all_stops_
							<< " stops on route, " << result.unique_stops_ << " unique stops, "
							<< std::setprecision(6) << result.curvature_ << " route length, " << result.route_
							<< " curvature" << std::endl;
					}
					else
					{
						out << "Bus " << bus << ": not found" << std::endl;
					}
				}
				else
				{
					auto stop = std::string{detail::Clear(query_base[i].substr(it_start_name))};
					auto result = catalog_->StopFind(stop);
					if (!result)
					{
						out << "Stop " << stop << ": not found" << std::endl;
					}
					else
					{
						if ((*result).buses_.size() == 0)
						{
							out << "Stop " << stop << ": no buses" << std::endl;
						}
						else
						{
							out << "Stop " << stop << ": buses";
							for (auto bus : (*result).buses_)
							{
								out << ' ' << bus;
							}
							out << std::endl;
						}
					}
				}
			}
		}

		void RequestHandler::GetMap(std::ostream &out)
		{
			if (renderer_)
			{
				std::stringstream svg_out = renderer_.value()->DrawMap(catalog_->GetMapInfo());
				out << svg_out.str();
			}
		}

		std::deque<std::string> &RequestHandler::GetBase()
		{
			return transport_base;
		}

		std::deque<std::string>::iterator RequestHandler::begin()
		{
			return transport_base.begin();
		}

		std::deque<std::string>::iterator RequestHandler::end()
		{
			return transport_base.end();
		}

		size_t RequestHandler::size()
		{
			return transport_base.size();
		}

	} // namespace base_reader

} // namespace transport_catalogue
