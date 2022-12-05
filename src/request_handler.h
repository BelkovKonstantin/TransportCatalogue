#pragma once
#include "json_reader.h"

namespace transport_catalogue
{

	namespace base_reader
	{
		using namespace transport_catalogue::units;

		/* Класс, занимающийся обработкой запросов
		Играет роль Фасада, который упрощает взаимодействие с транспортным каталогом */
		class RequestHandler
		{
		public:
			RequestHandler(std::istream &input, catalogue_base::TransportCatalogue &catalog, std::ostream &out);
			RequestHandler(RequestHandler &other) = delete;
			RequestHandler(RequestHandler &&other) = default;
			RequestHandler &operator=(RequestHandler &&other) = default;

			// Обработка введенных данных каталога
			void ParseInputData();
			void ParseStopData(std::unordered_map<std::string, catalogue_base::Stop> &stops, std::string &query);
			void ParseBusData(std::unordered_map<std::string, catalogue_base::Stop> &stops, std::unordered_map<std::string, catalogue_base::Bus> &all_bus, std::string &query);

			// Чтение запросов из ввода
			void ReadInputQueries(std::istream &input, std::ostream &out);

			// Выволнение/вывод запросов
			void ExecuteJsonStopQuery(std::string &stop, json::Builder &out_json);
			void ExecuteJsonBusQuery(std::string &bus, json::Builder &out_json);
			void ExecuteJsonRouteQuery(std::pair<std::string, std::string> &from_to, json::Builder &out_json);
			void ExecuteJsonMapQuery(json::Builder &out_json);
			void ExecuteJsonQueries(json_reader::QueryBase &&querys_base, std::ostream &out);
			void ExecuteQueries(std::ostream &out);

			// Выводит карту, если были введены render_setting
			void GetMap(std::ostream &out);
			std::deque<std::string> &GetBase();
			std::deque<std::string>::iterator begin();
			std::deque<std::string>::iterator end();
			size_t size();

		private:
			FailedQuery fq{};
			std::deque<std::string> transport_base;
			std::deque<std::string> query_base;
			catalogue_base::TransportCatalogue *catalog_;

			std::optional<renderer::MapRenderer *> renderer_;
			std::optional<graph::TransportRouter<PathTime> *> transport_router_;
		};
	} // namespace base_reader

} // namespace transport_catalogue
