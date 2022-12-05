#pragma once
#include "router.h"
#include "transport_catalogue.h"
#include <memory>
#include "log_duration.h"

namespace graph
{
    using namespace transport_catalogue::units;

    // Класс-обёртка, упрощающая взаимодействие с графом и маршутизатором
    template <typename Weight>
    class TransportRouter : public DirectedWeightedGraph<Weight>, public Router<Weight>
    {
    private:
        ActualGraphStops stops_{};
        std::unordered_map<std::string_view, size_t> stops_num_;

        // Присваивает остановкам уникальные id
        void GiveStopsUniqId()
        {
            size_t stop_num = 0;
            for (auto stop : stops_.uniq_stops_)
            {
                stops_num_[stop] = stop_num++;
            }
        }

    public:
        TransportRouter() {}

        // Добавляет уникальную остановку, через которую проезжает автобус
        void InsertStop(std::string_view stop);

        // Создаёт основу графа(резервирум место для путей)
        void BuildGraph();

        // Добавляет путь в граф
        void AddRoute(std::string_view from, std::string_view to, int route, std::string_view bus, short span_count);

        // Строит маршрутизатор, после того как заполнили граф
        void BuildRouter();

        std::pair<int, int> GetStopId(std::string_view from, std::string_view to);
        std::string_view GetStopName(size_t id);
    };

    template <typename Weight>
    void TransportRouter<Weight>::InsertStop(std::string_view stop)
    {
        stops_.uniq_stops_.insert(stop);
    }

    template <typename Weight>
    void TransportRouter<Weight>::BuildGraph()
    {
        this->AddVertexCount(stops_.uniq_stops_.size());
        GiveStopsUniqId();
    }

    template <typename Weight>
    void TransportRouter<Weight>::AddRoute(std::string_view from, std::string_view to, int route, std::string_view bus, short span_count)
    {
        this->AddEdge({stops_num_.at(from), stops_num_.at(to), PathTime{static_cast<double>(route / RoutingParams::bus_velocity) + static_cast<double>(RoutingParams::bus_wait_time), span_count, bus, false}});
    }

    template <typename Weight>
    void TransportRouter<Weight>::BuildRouter()
    {
        this->FillingRouter(&(*this));
    }

    template <typename Weight>
    std::pair<int, int> TransportRouter<Weight>::GetStopId(std::string_view from, std::string_view to)
    {
        if (auto from_id = stops_num_.find(from); from_id != stops_num_.end())
        {
            if (auto to_id = stops_num_.find(to); to_id != stops_num_.end())
            {
                return {from_id->second, to_id->second};
            }
        }
        return {-1, -1};
    }

    template <typename Weight>
    std::string_view TransportRouter<Weight>::GetStopName(size_t id)
    {
        return *(std::next(stops_.uniq_stops_.begin(), id));
    }

} // namespace graph