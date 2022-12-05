#include "map_renderer.h"

bool IsZero(double value)
{
	return std::abs(value) < EPSILON;
}

namespace transport_catalogue
{
	namespace renderer
	{
		svg::Point SphereProjector::operator()(geo::Coordinates coords) const
		{
			return {
				(coords.lng - min_lon_) * zoom_coeff_ + padding_,
				(max_lat_ - coords.lat) * zoom_coeff_ + padding_};
		}

		MapRenderer::MapRenderer(MapSetting map_info) : map_info_(map_info)
		{
			map_info_.bus_text_.SetFontFamily("Verdana").SetFontWeight("bold");
			map_info_.stop_text_.SetFontFamily("Verdana").SetFillColor("black");
		}

		void MapRenderer::PrintBus(SphereProjector &proj, units::BusMapInfo &map_base, Document &doc)
		{
			int count = 0;
			for (auto &bus : map_base.buses_)
			{
				if (map_base.buses_info_->at(bus).stops.size() != 0)
				{
					Polyline bus_line;
					for (size_t i = 1; i <= map_base.buses_info_->at(bus).stops.size(); ++i)
					{
						bus_line.AddPoint(
							proj({map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(i)).ltd_lng.first,
								  map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(i)).ltd_lng.second}));
					}
					if (!map_base.buses_info_->at(bus).is_roundtrip_)
					{
						for (size_t i = map_base.buses_info_->at(bus).stops.size() - 1; i > 0;
							 --i)
						{
							bus_line.AddPoint(
								proj({map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(i)).ltd_lng.first,
									  map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(i)).ltd_lng.second}));
						}
					}
					bus_line.SetStrokeWidth(map_info_.line_width_).SetStrokeColor(map_info_.color_palette_[count % map_info_.color_palette_.size()]).SetFillColor(NoneColor).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);
					doc.Add(bus_line);
					++count;
				}
			}
		}

		void MapRenderer::PrintBusName(SphereProjector &proj, units::BusMapInfo &map_base,
									   Document &doc)
		{
			int count = 0;
			Text bus_background = map_info_.bus_text_;
			bus_background.SetFillColor(map_info_.underlayer_color_).SetStrokeColor(map_info_.underlayer_color_).SetStrokeWidth(map_info_.underlayer_width_).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);
			count = 0;
			for (auto bus : map_base.buses_)
			{
				if (map_base.buses_info_->at(bus).stops.size() != 0)
				{
					bus_background.SetPosition(
									  proj({map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(1)).ltd_lng.first,
											map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(1)).ltd_lng.second}))
						.SetData(bus.data());
					doc.Add(bus_background);
					map_info_.bus_text_.SetPosition(
										   proj({map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(1)).ltd_lng.first,
												 map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(1)).ltd_lng.second}))
						.SetFillColor(map_info_.color_palette_[count % map_info_.color_palette_.size()])
						.SetData(bus.data());
					doc.Add(map_info_.bus_text_);
					if (!map_base.buses_info_->at(bus).is_roundtrip_ && (map_base.buses_info_->at(bus).stops.at(1) != map_base.buses_info_->at(bus).stops.at(
																														  map_base.buses_info_->at(bus).stops.size())))
					{
						bus_background.SetPosition(
										  proj({map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(map_base.buses_info_->at(bus).stops.size())).ltd_lng.first,
												map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(map_base.buses_info_->at(bus).stops.size())).ltd_lng.second}))
							.SetData(
								bus.data());
						doc.Add(bus_background);
						map_info_.bus_text_.SetPosition(
											   proj({map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(map_base.buses_info_->at(bus).stops.size())).ltd_lng.first,
													 map_base.stops_->at(map_base.buses_info_->at(bus).stops.at(map_base.buses_info_->at(bus).stops.size())).ltd_lng.second}))
							.SetFillColor(map_info_.color_palette_[count % map_info_.color_palette_.size()])
							.SetData(bus.data());
						doc.Add(map_info_.bus_text_);
					}
					++count;
				}
			}
		}

		void MapRenderer::PrintStop(SphereProjector &proj, std::set<std::string> &stops,
									units::BusMapInfo &map_base,
									Document &doc)
		{
			Circle stop_on_map;
			stop_on_map.SetFillColor("white").SetRadius(map_info_.stop_radius_);
			for (auto stop : stops)
			{
				stop_on_map.SetCenter(
					proj({map_base.stops_->at(stop).ltd_lng.first, map_base.stops_->at(stop).ltd_lng.second}));
				doc.Add(stop_on_map);
			}
		}

		void MapRenderer::PrintStopName(SphereProjector &proj, std::set<std::string> &stops,
										units::BusMapInfo &map_base, Document &doc)
		{
			Text stop_background = map_info_.stop_text_;
			stop_background.SetFillColor(map_info_.underlayer_color_).SetStrokeColor(map_info_.underlayer_color_).SetStrokeWidth(map_info_.underlayer_width_).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);
			for (auto &stop : stops)
			{
				stop_background.SetPosition(
								   proj({map_base.stops_->at(stop).ltd_lng.first, map_base.stops_->at(stop).ltd_lng.second}))
					.SetData(stop);
				doc.Add(stop_background);
				map_info_.stop_text_.SetPosition(proj({map_base.stops_->at(stop).ltd_lng.first, map_base.stops_->at(stop).ltd_lng.second}))
					.SetData(stop);
				doc.Add(map_info_.stop_text_);
			}
		}

		std::stringstream MapRenderer::DrawMap(units::BusMapInfo map_base)
		{
			std::stringstream out;
			std::vector<Coordinates> coord;
			for (std::pair<std::string_view, units::Bus> bus : *(map_base.buses_info_))
			{
				if (bus.second.stops.size() != 0)
				{
					for (size_t i = 1; i <= bus.second.stops.size(); ++i)
					{
						coord.push_back(
							{map_base.stops_->at(bus.second.stops.at(i)).ltd_lng.first,
							 map_base.stops_->at(bus.second.stops.at(i)).ltd_lng.second});
					}
				}
			}
			SphereProjector proj{coord.begin(), coord.end(), map_info_.map_size_.x,
								 map_info_.map_size_.y, map_info_.padding_};
			Document doc;
			PrintBus(proj, map_base, doc);
			PrintBusName(proj, map_base, doc);
			std::set<std::string> stops;
			for (std::pair<std::string, units::Stop> stop : *(map_base.stops_))
			{
				if (stop.second.buses.buses_.size() != 0)
					stops.insert(stop.first);
			}

			PrintStop(proj, stops, map_base, doc);
			PrintStopName(proj, stops, map_base, doc);

			doc.Render(out);
			return out;
		}
	} // namespace renderer
} // namespace transport_catalogue
