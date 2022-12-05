#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"
#include <iomanip>

namespace transport_catalogue
{

	using namespace transport_catalogue::units;

	// Разбор/подсчёт информации конкретного маршрута(автобуса)
	void ParseRouteInfo(std::unordered_map<std::string, Bus> &all_bus,
						std::unordered_map<std::string, Stop> &stops,
						const std::string &bus, std::optional<graph::TransportRouter<PathTime> *> transport_router_);

	namespace json_reader
	{

		using namespace units;

		// Класс сохраняющий информацию из json документа
		class JSONReader
		{
		public:
			JSONReader(std::istream &input, catalogue_base::TransportCatalogue &catalog);
			JSONReader(JSONReader &other) = delete;
			JSONReader(JSONReader &&other) = default;
			JSONReader &operator=(JSONReader &&other) = default;

			// Наполнение Транспортного каталога
			void FillingTransportBase(json::Node &json, catalogue_base::TransportCatalogue &catalog);

			// Добавлеяет информацию об остановке
			void AddStop(std::unordered_map<std::string, catalogue_base::Stop> &stops, json::Node &json);

			// Добавляет информацию об автобусе
			void AddBus(std::unordered_map<std::string, catalogue_base::Stop> &stops, std::unordered_map<std::string, catalogue_base::Bus> &all_bus, json::Node &json);

			// Наполняет базу запросов к Транспортному каталогу
			void FillingQueryBase(json::Node &json);

			// Сохраняет скорость транспорта и длительности автобусных остановок
			void SaveRouterSetting(json::Node &json);

			// Создаёт "визуализатор" карты, с указанными в json настройками(не заполняет карту)
			void CreateMapRender(json::Node &json);

			// Передаёт ссылку базу из которой создавался Транспортный каталог
			std::deque<std::string> &GetBase();

			// Передаёт базу запросов
			QueryBase &&GetQueryBase();

			// Передаёт указатель на "визуализатор" карты
			renderer::MapRenderer *GetMapRender();

			// Передаёт указатель на класс реализующий поиск кратчайшего пути между остановками
			std::optional<graph::TransportRouter<PathTime> *> GetTransportRouter();

			std::deque<std::string>::iterator begin();
			std::deque<std::string>::iterator end();
			size_t size();

		private:
			const std::vector<std::string> type_query_{"Stop", "Bus", "Route", "Map"};

			json::Node *json_;
			std::deque<std::string> transport_base;
			QueryBase base_;

			std::optional<renderer::MapRenderer *> renderer_;
			std::optional<graph::TransportRouter<PathTime> *> transport_router_;
		};
	} // namespace json_reader
} // namespace transport_catalogue
