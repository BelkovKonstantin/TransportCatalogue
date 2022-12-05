#include "request_handler.h"
#include <cassert>
#include <windows.h>

using namespace std;
using namespace transport_catalogue;

double RoutingParams::bus_velocity = 0;
double RoutingParams::bus_wait_time = 0;

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	catalogue_base::TransportCatalogue catalog;
	base_reader::RequestHandler transport_base(std::cin, catalog, std::cout);
	transport_base.GetMap(cout);
	return 0;
}
